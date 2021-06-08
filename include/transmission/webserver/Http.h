//
// Created by hzy on 6/2/21.
//

#ifndef SIMPLESYSTEM_HTTP_H
#define SIMPLESYSTEM_HTTP_H

#include "utils/buffer/Buffer.h"
#include <arpa/inet.h>
#include <unordered_map>
#include <functional>
#include <memory>
#include <sys/stat.h>

namespace transmission { namespace webserver {
    class Http {
    public:
        // close connection when (BAD_REQUEST, INTERNAL_ERROR)
        enum CODE {
            OK = 200,
            BAD_REQUEST = 400,
            FORBIDDEN = 403,
            NOT_FOUND = 404,
            INTERNAL_ERROR = 500
        };
        enum PARSE_STATE {
            START, REQUEST_LINE, HEADER, CONTENT, FINISH,
        };

    private:
        // basic
        bool m_debug;
        int m_fd; // debug
        utils::buffer::Buffer m_readBuffer; // read buffer
        utils::buffer::Buffer m_writeBuffer; // write buffer
        int m_iovCnt;
        struct iovec m_iov[2];
        // request
        std::string m_line;
        PARSE_STATE m_state;
        std::string m_method;
        std::string m_path;
        std::string m_version;
        std::unordered_map<std::string, std::string> m_header;
        std::string m_content;
        // response
        int m_code;
        char* m_file;
        struct stat m_fileStat;

    public:
        // basic
        Http(int fd, bool debug = false);
        ~Http() = default;
        Http(const Http&) = delete;
        Http& operator = (const Http&) = delete;
        Http(Http&&) = delete;
        Http& operator = (Http&&) = delete;
        ssize_t readRequest(int fd, int *ern);
        ssize_t writeResponse(int fd, int *ern);
        size_t toWriteBytes();
        void clearBuffer();
        // request & response
        void initNextHttp();
        bool process(std::unordered_map<std::string, std::function<void(Http*)>> *userFunction);
        std::string getMethod() const;
        std::string getPath() const;
        std::string getVersion() const;
        std::string getContent() const;
        bool isKeepAlive() const;
        int getCode() const;
        bool hasError() const;
        void unMapFile();

    private:
        // request & response
        bool readLine(); // true:get a line, false:incomplete
        size_t getContentLength();
        bool parseRequestLine();
        bool parseHeader();
        bool parseContent();
        void execute(std::unordered_map<std::string, std::function<void(Http*)>> *userFunction);
        void addStateLine();
        void addHeader();
        void addContent();
    };
}}

//    1 REQUEST:
//    1.1 GET
//    GET /562f25980001b1b106000338.jpg HTTP/1.1
//    Host:img.mukewang.com
//    User-Agent:Mozilla/5.0 (Windows NT 10.0; WOW64)
//    Accept:image/webp,image/*,*/*;q=0.8
//    Referer:http://www.imooc.com/
//    Accept-Encoding:gzip, deflate, sdch
//    Accept-Language:zh-CN,zh;q=0.8
//    blank
//    1.2 POST
//    POST / HTTP/1.1
//    Host:www.wrox.com
//    User-Agent:Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)
//    Content-Type:application/x-www-form-urlencoded
//    Content-Length:40
//    Connection: Keep-Alive
//    blank
//    name=Professional%20Ajax&publisher=Wiley
//    2 RESPONSE
//    HTTP/1.1 200 OK
//    Date: Fri, 22 May 2009 06:07:21 GMT
//    Content-Type: text/html; charset=UTF-8
//    空行
//    <html>
//          <head></head>
//          <body>
//                <!--body goes here-->
//          </body>
//    </html>

#endif //SIMPLESYSTEM_HTTP_H
