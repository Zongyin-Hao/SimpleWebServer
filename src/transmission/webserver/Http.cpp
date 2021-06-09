//
// Created by hzy on 6/2/21.
//

#include "transmission/webserver/Http.h"
#include "utils/Error.h"
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <algorithm>
#include <string>
#include <regex>
#include <iostream>

namespace transmission { namespace webserver {
    static std::unordered_map<std::string, std::string> HTTP_TYPE = {
            { ".html",  "text/html" },
            { ".xml",   "text/xml" },
            { ".xhtml", "application/xhtml+xml" },
            { ".txt",   "text/plain" },
            { ".rtf",   "application/rtf" },
            { ".pdf",   "application/pdf" },
            { ".word",  "application/nsword" },
            { ".png",   "image/png" },
            { ".gif",   "image/gif" },
            { ".jpg",   "image/jpeg" },
            { ".jpeg",  "image/jpeg" },
            { ".au",    "audio/basic" },
            { ".mpeg",  "video/mpeg" },
            { ".mpg",   "video/mpeg" },
            { ".avi",   "video/x-msvideo" },
            { ".gz",    "application/x-gzip" },
            { ".tar",   "application/x-tar" },
            { ".css",   "text/css "},
            { ".js",    "text/javascript "},
    };
    static std::unordered_map<int, std::string> HTTP_STATUS = {
            { Http::OK, "OK" },
            { Http::BAD_REQUEST, "Bad Request" },
            { Http::FORBIDDEN, "Forbidden" },
            { Http::NOT_FOUND, "Not Found" },
            {Http::INTERNAL_ERROR, "Internal Error"},
    };

    // ==============================basic==============================
    Http::Http(int fd, bool debug) : m_fd(fd), m_readBuffer(1024),
    m_writeBuffer(1024), m_debug(debug) {
        clearBuffer();
        initNextHttp();
    }

    ssize_t Http::readRequest(int fd, int *ern) {
        char buffer[65536];
        struct iovec iov[2];
        size_t wtb = m_readBuffer.writableBytes();
        iov[0].iov_base = m_readBuffer.nextWritePos();
        iov[0].iov_len = wtb;
        iov[1].iov_base = buffer;
        iov[1].iov_len = sizeof(buffer);
        ssize_t len = readv(fd, iov, 2);
        if (len < 0) {
            *ern = errno;
        } else if (len > 0) {
            if (static_cast<size_t>(len) <= wtb) {
                m_readBuffer.writeBuffer_idx(static_cast<size_t>(len));
            } else {
                m_readBuffer.writeBuffer_idx(static_cast<size_t>(wtb));
                m_readBuffer.writeBuffer(buffer, len-wtb);
            }
            if (m_debug) {
                std::cout << "[fd:" << m_fd << "] ";
                m_readBuffer.output();
            }
        }
        return len;
    }

    ssize_t Http::writeResponse(int fd, int *ern) {
        ssize_t len = writev(fd, m_iov, m_iovCnt);
        if (len < 0) {
            *ern = errno;
        }
        else if (len > 0) {
            if (static_cast<size_t>(len) > m_iov[0].iov_len) {
                m_iov[1].iov_base = (char*)m_iov[1].iov_base+(len-m_iov[0].iov_len);
                m_iov[1].iov_len -= (len - m_iov[0].iov_len);
                if (m_iov[0].iov_len != 0) {
                    m_writeBuffer.readBufferAll_idx();
                    m_iov[0].iov_len = 0;
                }
            } else {
                m_iov[0].iov_base = (char*)m_iov[0].iov_base+len;
                m_iov[0].iov_len -= len;
                m_writeBuffer.readBuffer_idx(len);
            }
        }
        return len;
    }

    size_t Http::toWriteBytes() {
        if (m_iovCnt == 0) return 0;
        else if (m_iovCnt == 1) return m_iov[0].iov_len;
        return m_iov[0].iov_len + m_iov[1].iov_len;
    }

    void Http::clearBuffer() {
        m_readBuffer.readBufferAll_idx();
        m_writeBuffer.readBufferAll_idx();
    }

    // ==============================request & response==============================
    void Http::initNextHttp() {
        // request
        m_line = "";
        m_state = START;
        m_method = "";
        m_path = "";
        m_version = "";
        m_header.clear();
        m_content = "";
        // response
        m_code = OK;
        unMapFile();
        m_iovCnt = 0;
        m_iov[0].iov_base = nullptr;
        m_iov[0].iov_len = 0;
        m_iov[1].iov_base = nullptr;
        m_iov[1].iov_len = 0;
    }

    // return false means incomplete
    bool Http::process(std::unordered_map<std::string, std::function<void(Http*)>> *userFunction) {
        // main state machine
        // notice that buffer data may be incomplete,
        // or there may be more than one request
        while (true) {
            // choose follow state machine
            // we use if-else to facilitate the use of (continue, break)
            if (m_state == START) {
                if (!readLine()) {
                    return false;
                }
                m_state = REQUEST_LINE;
            } else if (m_state == REQUEST_LINE) {
                if (!parseRequestLine()) {
                    m_code = BAD_REQUEST;
                    break;
                }
                m_state = HEADER;
                if (!readLine()) {
                    return false;
                }
            } else if (m_state == HEADER) {
                if (m_line.empty()) {
                    if (getContentLength() == 0) {
                        m_state = FINISH;
                    } else {
                        m_state = CONTENT;
                        if (!readLine()) {
                            return false;
                        }
                    }
                    continue;
                }
                if (!parseHeader()) {
                    m_code = BAD_REQUEST;
                    break;
                }
                if (!readLine()) {
                    return false;
                }
            } else if (m_state == CONTENT) {
                utils::Error::Throw(utils::Error::SORRY);
            } else if (m_state == FINISH) {
                if (m_debug) {
                    std::cout << "[fd:" << m_fd << "] " << "finished" << std::endl;
                }
                break;
            } else {
                m_code = INTERNAL_ERROR;
                break;
            }
        }
        if (m_code == OK) {
            execute(userFunction);
        }
        addStateLine();
        addHeader();
        addContent();
        return true;
    }

    std::string Http::getMethod() const {
        return m_method;
    }

    std::string Http::getPath() const {
        return m_path;
    }

    std::string Http::getVersion() const {
        return m_version;
    }

    std::string Http::getContent() const {
        return m_content;
    }

    bool Http::isKeepAlive() const {
        if (m_header.count("Connection") == 1) {
            return m_header.find("Connection")->second == "keep-alive";
        }
        return false;
    }

    int Http::getCode() const {
        return m_code;
    }

    bool Http::hasError() const {
        return (m_code == BAD_REQUEST || m_code == INTERNAL_ERROR);
    }

    void Http::unMapFile() {
        if(m_file != nullptr) {
            munmap(m_file, m_fileStat.st_size);
            m_file = nullptr;
        }
        m_fileStat = {0};
    }

    // true:get a line, false:incomplete
    bool Http::readLine() {
        const char CRLF[] = "\r\n";
        // search the first position of CRLF in [readPos, writePos)
        const char* lineEnd = std::search(m_readBuffer.nextReadPos(),
                                          m_readBuffer.nextWritePos(),
                                          CRLF, CRLF+2);
        // data incomplete
        if (lineEnd == m_readBuffer.nextWritePos()) {
            if (m_debug) {
                std::cout << "[fd:" << m_fd << "] " << "incomplete" << std::endl;
                m_readBuffer.output();
            }
            return false;
        }
        // get a line
        m_line = m_readBuffer.readBuffer(lineEnd - m_readBuffer.nextReadPos());
        m_readBuffer.readBuffer(2); // CRLF;
        if (m_debug) {
            std::cout << "[fd:" << m_fd << "] " << "line = " << m_line << std::endl;
        }
        return true;
    }

    size_t Http::getContentLength() {
        size_t len = 0;
        if (m_header.count("Content-Length") != 0) {
            char *stop;
            len = strtol(m_header["Content-Length"].c_str(), &stop, 10);
        }
        if (m_debug) {
            std::cout << "[fd:" << m_fd << "] " << "content-length = " << len << std::endl;
        }
        return len;
    }

    bool Http::parseRequestLine() {
        std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
        std::smatch sMatch;
        if (std::regex_match(m_line, sMatch, patten)) {
            m_method = sMatch[1];
            m_path = sMatch[2];
            m_version = sMatch[3];
            if (m_debug) {
                std::cout << "[fd:" << m_fd << "] " << "method = " << m_method << std::endl;
                std::cout << "[fd:" << m_fd << "] " << "path = " << m_path << std::endl;
                std::cout << "[fd:" << m_fd << "] " << "version = " << m_version << std::endl;
            }
            return true;
        } else {
            return false; // internal error
        }
    }

    bool Http::parseHeader() {
        std::regex patten("^([^:]*): ?(.*)$");
        std::smatch sMatch;
        if (std::regex_match(m_line, sMatch, patten)) {
            m_header[sMatch[1]] = sMatch[2];
            if (m_debug) {
                std::cout << "[fd:" << m_fd << "] " << "header = " << sMatch[1] << ": " << sMatch[2] << std::endl;
            }
            return true;
        } else {
            return false; // internal error
        }
    }

    // todo
    bool parseContent(const std::string& line) {
        return false;
    }

    void Http::execute(std::unordered_map<std::string, std::function<void(Http*)>> *userFunction) {
        if (userFunction->count(m_path) == 1) {
            if (m_debug) {
                std::cout << "[fd:" << m_fd << "] " << "call user function" << std::endl;
            }
            auto func = (*userFunction)[m_path];
            func(this);
        } else {
            if (m_debug) {
                std::cout << "[fd:" << m_fd << "] " << "read file" << std::endl;
            }
            std::string path = "../www"+m_path;
            if (stat(path.c_str(), &m_fileStat) < 0 || S_ISDIR(m_fileStat.st_mode)) {
                if (m_debug) {
                    std::cout << "[fd:" << m_fd << "] " << "not found1" << std::endl;
                }
                m_code = NOT_FOUND;
                return;
            }
            if (!(m_fileStat.st_mode & S_IROTH)) {
                if (m_debug) {
                    std::cout << "[fd:" << m_fd << "] " << "forbidden" << std::endl;
                }
                m_code = FORBIDDEN;
                return;
            }
            int fd = open(path.c_str(), O_RDONLY);
            if (fd < 0) {
                if (m_debug) {
                    std::cout << "[fd:" << m_fd << "] " << "not found2" << std::endl;
                }
                m_code = NOT_FOUND;
                return;
            }
            int* ret = (int*) mmap(nullptr, m_fileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (*ret == -1) {
                if (m_debug) {
                    std::cout << "[fd:" << m_fd << "] " << "not found3" << std::endl;
                }
                m_code = NOT_FOUND;
                return;
            }
            m_file = (char*)ret;
            close(fd);
            if (m_debug) {
                std::cout << "[fd:" << m_fd << "] " << "read successfully" << std::endl;
            }
        }
    }

    void Http::addStateLine() {
        std::string status;
        if (HTTP_STATUS.count(m_code) == 1) {
            status = HTTP_STATUS.find(m_code)->second;
        } else {
            m_code = INTERNAL_ERROR;
            status = HTTP_STATUS.find(m_code)->second;
        }
        std::string msg = "HTTP/1.1 " +
                std::to_string(m_code) +
                " " + status + "\r\n";
        if (m_debug) {
            std::cout << "[fd:" << m_fd << "] " << msg << "=====" << std::endl;
        }
        m_writeBuffer.writeBuffer(msg.c_str(), msg.length());
    }

    void Http::addHeader() {
        std::string msg = "Connection: ";
        if (isKeepAlive()) {
            msg += "keep-alive\r\n";
            msg += "keep-alive: max=6, timeout=120\r\n";
        } else {
            msg += "close\r\n";
        }
        msg += "Content-length: " + std::to_string(m_fileStat.st_size) + "\r\n";
        std::string tp = "text/plain";
        std::string::size_type idx = m_path.find_last_of('.');
        if (idx != std::string::npos) {
            std::string suffix = m_path.substr(idx);
            if (HTTP_TYPE.count(suffix) == 1) {
                tp = HTTP_TYPE.find(suffix)->second;
            }
        }
        msg += "Content-type: " + tp + "\r\n";
        msg += "\r\n";
        if (m_debug) {
            std::cout << "[fd:" << m_fd << "] " << msg << "=====" << std::endl;
        }
        m_writeBuffer.writeBuffer(msg.c_str(), msg.length());
        m_iov[0].iov_base = m_writeBuffer.nextReadPos();
        m_iov[0].iov_len = m_writeBuffer.readableBytes();
        m_iovCnt = 1;
    }

    void Http::addContent() {
        if (m_file != nullptr) {
            m_iov[1].iov_base = m_file;
            m_iov[1].iov_len = m_fileStat.st_size;
            m_iovCnt = 2;
        }
    }
}}