#include "transmission/webserver/Http.h"
#include "utils/Error.h"
#include <unistd.h> // close
#include <fcntl.h> // O_RDONLY
#include <sys/uio.h> // readv, writev
#include <sys/mman.h> // mmap
#include <string>
#include <regex>
#include <iostream>

namespace transmission { namespace webserver {
    // content type,不太全,可以去网上粘,也可以直接用我的
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
            {".mp4",    "video/mpeg4"},
            { ".gz",    "application/x-gzip" },
            { ".tar",   "application/x-tar" },
            { ".css",   "text/css" },
            { ".js",    "text/javascript" },
    };
    static std::unordered_map<int, std::string> HTTP_STATUS = {
            { Http::OK, "OK" },
            { Http::BAD_REQUEST, "Bad Request" },
            { Http::FORBIDDEN, "Forbidden" },
            { Http::NOT_FOUND, "Not Found" },
            {Http::INTERNAL_ERROR, "Internal Error"},
    };

    // ==============================basic==============================
    // 初始化buffer和状态机
    Http::Http(int fd, bool debug) : m_fd(fd), m_readBuffer(1024),
    m_writeBuffer(1024), m_debug(debug) {
        clearBuffer();
        initNextHttp();
    }

    // 从readBuffer中读取请求
    // ET模式下WebServer会循环调用这个函数
    ssize_t Http::readRequest(int fd, int *ern) {
        char buffer[65536]; // 内核缓冲区比readBuffer剩余空间大时先存到这里,然后再对buffer进行扩容
        struct iovec iov[2];
        size_t wtb = m_readBuffer.writableBytes();
        iov[0].iov_base = m_readBuffer.nextWritePos();
        iov[0].iov_len = wtb;
        iov[1].iov_base = buffer;
        iov[1].iov_len = sizeof(buffer);
        ssize_t len = readv(fd, iov, 2); // 等于0貌似是对端关闭连接来着,在WebServer里也要断开连接
        if (len < 0) {
            *ern = errno;
        } else if (len > 0) {
            // 这里有的直接改buffer,有的只改下标,不太好看,不过为了效率就这么写吧...
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

    // 将响应报文写入writeBuffer
    // ET模式下WebServer会循环调用这个函数
    ssize_t Http::writeResponse(int fd, int *ern) {
        ssize_t len = writev(fd, m_iov, m_iovCnt); // 这个等于0没什么影响,一般就是内核缓冲区写不进去了
        if (len < 0) {
            *ern = errno;
        }
        else if (len > 0) {
            if (static_cast<size_t>(len) > m_iov[0].iov_len) {
                m_iov[1].iov_base = (char*)m_iov[1].iov_base+(len-m_iov[0].iov_len);
                m_iov[1].iov_len -= (len - m_iov[0].iov_len);
                if (m_iov[0].iov_len != 0) {
                    m_writeBuffer.readBufferAll_idx(); // 相当于清空writeBuffer
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

    // 清空缓冲区,初始化时用
    void Http::clearBuffer() {
        m_readBuffer.readBufferAll_idx();
        m_writeBuffer.readBufferAll_idx();
    }

    // ==============================request & response==============================
    // 初始化状态机(注意这里不初始化buffer)
    void Http::initNextHttp() {
        // 请求报文相关
        m_line = "";
        m_state = START;
        m_method = "";
        m_path = "";
        m_version = "";
        m_header.clear();
        m_content = "";
        // 响应报文相关
        m_code = OK;
        unMapFile();
        m_iovCnt = 0;
        m_iov[0].iov_base = nullptr;
        m_iov[0].iov_len = 0;
        m_iov[1].iov_base = nullptr;
        m_iov[1].iov_len = 0;
    }

    // 处理用户的请求(调用api或读取文件)
    // true表示之后可以发送响应报文了(有错误的话code会相应地设为BAD_REQUEST,INTERNAL_ERROR等),false表示数据不完整,还需要继续等待数据到来
    // userFunction是用户自定义的api
    bool Http::process(std::unordered_map<std::string, std::function<void(Http*)>> *userFunction) {
        // 注意缓冲区里的请求可能不完整,也可能有多条请求,这些都要考虑在内
        // 请求不完整时return false继续等数据就好,维护好状态机的全局状态
        // 多条请求时要在response发完后重新调用process,形成一个环路
        while (true) {
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
                if (m_line.empty()) { // header后一定有个空行,接着根据是否有content进行状态转移
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
                // todo
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
            execute(userFunction); // 处理请求
        }
        // 制作响应报文
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

    // 是否是长连接,短连接时server会在发送完响应报文后关闭连接
    bool Http::isKeepAlive() const {
        if (m_header.count("Connection") == 1) {
            return m_header.find("Connection")->second == "keep-alive";
        }
        return false;
    }

    int Http::getCode() const {
        return m_code;
    }

    // iov中还需要发送的数据量,用来判断响应报文是否发完
    size_t Http::toWriteBytes() {
        if (m_iovCnt == 0) return 0;
        else if (m_iovCnt == 1) return m_iov[0].iov_len;
        return m_iov[0].iov_len + m_iov[1].iov_len;
    }

    // 当状态码为BAD_REQUEST或INTERNAL_ERROR时服务器关闭连接, hasError就是为此作判断的
    bool Http::hasError() const {
        return (m_code == BAD_REQUEST || m_code == INTERNAL_ERROR);
    }

    // 用mmap后需要unmap
    void Http::unMapFile() {
        if(m_file != nullptr) {
            munmap(m_file, m_fileStat.st_size);
            m_file = nullptr;
        }
        m_fileStat = {0};
    }

    // true:get a line, false:数据不完整
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

    // 获取Content长度,用于判断是否需要继续处理content,现阶段只能处理GET就假设为0了
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

    // 其实用户api从WebServer那里传参过来也不怎么优雅...
    void Http::execute(std::unordered_map<std::string, std::function<void(Http*)>> *userFunction) {
        if (userFunction->count(m_path) == 1) {
            // 根据路径调用用户api
            if (m_debug) {
                std::cout << "[fd:" << m_fd << "] " << "call user function" << std::endl;
            }
            auto func = (*userFunction)[m_path];
            func(this);
        } else {
            // 使用mmap将文件映射到m_file
            if (m_debug) {
                std::cout << "[fd:" << m_fd << "] " << "read file" << std::endl;
            }
            std::string path = "../www"+m_path; // 我是默认在bin下面运行的,这个路径可以自己改一下
            if (stat(path.c_str(), &m_fileStat) < 0 || S_ISDIR(m_fileStat.st_mode)) {
                if (m_debug) {
                    std::cout << "[fd:" << m_fd << "] " << "not found1" << std::endl;
                }
                m_code = NOT_FOUND;
                return;
            }
            if (!(m_fileStat.st_mode & S_IROTH)) { // 权限判断
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