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

namespace transmission { namespace webserver {
    static std::unordered_map<std::string, std::string> TYPE = {
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
    const std::unordered_map<int, std::string> STATUS = {
            { Http::OK, "OK" },
            { Http::BAD_REQUEST, "Bad Request" },
            { Http::FORBIDDEN, "Forbidden" },
            { Http::NOT_FOUND, "Not Found" },
            {Http::INTERNAL_ERROR, "Internal Error"},
    };

    // ==============================basic==============================
    Http::Http(bool debug) : m_readBuffer(1024),
    m_writeBuffer(1024), m_debug(debug) {
        init();
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
        if (len <= 0) {
            *ern = errno;
        } else {
            if (static_cast<size_t>(len) <= wtb) {
                m_readBuffer.writeBuffer_idx(static_cast<size_t>(len));
            } else {
                m_readBuffer.writeBuffer(buffer, len-wtb);
            }
            if (m_debug) {
                m_readBuffer.output();
            }
        }
        return len;
    }

    ssize_t Http::writeResponse(int fd, int *ern) {
        ssize_t len = writev(fd, m_iov, m_iovCnt);
        if (len <= 0) {
            *ern = errno;
        }
        else {
            if (static_cast<size_t>(len) > m_iov[0].iov_len) {
                m_iov[1].iov_base = (uint8_t*)m_iov[1].iov_base+(len-m_iov[0].iov_len);
                m_iov[1].iov_len -= (len - m_iov[0].iov_len);
                if (m_iov[0].iov_len != 0) {
                    m_writeBuffer.readBufferAll_idx();
                    m_iov[0].iov_len = 0;
                }
            } else {
                m_iov[0].iov_base = (uint8_t*)m_iov[0].iov_base+len;
                m_iov[0].iov_len -= len;
                m_writeBuffer.readBuffer_idx(len);
            }
        }
        return len;
    }

    inline size_t Http::toWriteBytes() {
        return m_iov[0].iov_len + m_iov[1].iov_len;
    }

    inline bool Http::hasError() const {
        return (m_code == BAD_REQUEST || m_code == INTERNAL_ERROR);
    }

    void Http::unMapFile() {
        if(m_file != nullptr) {
            munmap(m_file, m_fileStat.st_size);
            m_file = nullptr;
        }
    }

    // ==============================request & response==============================
    void Http::init() {
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
        m_fileStat = {0};
        m_iovCnt = 0;
    }

    // return false means incomplete
    bool Http::process() {
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
                break;
            } else {
                m_code = INTERNAL_ERROR;
                break;
            }
        }
        if (m_code == OK) {
            execute();
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

    // true:get a line, false:incomplete
    inline bool Http::readLine() {
        const char CRLF[] = "\r\n";
        // search the first position of CRLF in [readPos, writePos)
        const char* lineEnd = std::search(m_readBuffer.nextReadPos(),
                                          m_readBuffer.nextWritePos(),
                                          CRLF, CRLF+2);
        // data incomplete
        if (lineEnd == m_readBuffer.nextWritePos()) {
            return false;
        }
        // get a line
        m_line = m_readBuffer.readBuffer(lineEnd - m_readBuffer.nextReadPos());
        m_readBuffer.readBuffer(2); // CRLF;
        return true;
    }

    inline size_t Http::getContentLength() {
        if (m_header.count("Content-Length") != 0) {
            char *stop;
            return strtol(m_header["Content-Length"].c_str(), &stop, 10);
        }
        return 0;
    }

    bool Http::parseRequestLine() {
        std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
        std::smatch sMatch;
        if (std::regex_match(m_line, sMatch, patten)) {
            m_method = sMatch[1];
            m_path = sMatch[2];
            m_version = sMatch[3];
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
            return true;
        } else {
            return false; // internal error
        }
    }

    // todo
    bool parseContent(const std::string& line) {
        return false;
    }

    void Http::execute() {
        if (userFunction.count(m_path) != 0) {
            auto func = userFunction[m_path];
            func(std::make_shared<Http>(this));
        } else {
            std::string path = "../www"+m_path;
            if (stat(path.c_str(), &m_fileStat) < 0 || S_ISDIR(m_fileStat.st_mode)) {
                m_code = NOT_FOUND;
                return;
            }
            if (!(m_fileStat.st_mode & S_IROTH)) {
                m_code = FORBIDDEN;
                return;
            }
            int fd = open(path.c_str(), O_RDONLY);
            if (fd < 0) {
                m_code = NOT_FOUND;
                return;
            }
            int* ret = (int*) mmap(nullptr, m_fileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (*ret == -1) {
                m_code = NOT_FOUND;
                return;
            }
            m_file = (char*)ret;
            close(fd);
        }
    }

    void Http::addStateLine() {
        std::string status;
        if (STATUS.count(m_code) == 1) {
            status = STATUS.find(m_code)->second;
        } else {
            m_code = INTERNAL_ERROR;
            status = STATUS.find(m_code)->second;
        }
        std::string msg = "HTTP/1.1 " +
                std::to_string(m_code) +
                " " + status + "\r\n";
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
        if (idx == std::string::npos) {
            std::string suffix = m_path.substr(idx);
            if (TYPE.count(suffix) == 1) {
                tp = TYPE.find(suffix)->second;
            }
        }
        msg += "Content-type: " + tp + "\r\n";
        msg += "\r\n";
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