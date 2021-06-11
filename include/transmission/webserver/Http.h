#ifndef SIMPLESYSTEM_HTTP_H
#define SIMPLESYSTEM_HTTP_H

#include "utils/buffer/Buffer.h"
#include <arpa/inet.h> // iov
#include <sys/stat.h> // file stat
#include <unordered_map>
#include <functional>

namespace transmission { namespace webserver {
    class Http {
    public:
        // 当状态码为BAD_REQUEST或INTERNAL_ERROR时服务器关闭连接,避免缓冲区被污染
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
        // 基本组件
        bool m_debug; // 调试开关
        int m_fd; // 打印当前socket fd方便调试
        utils::buffer::Buffer m_readBuffer; // read buffer
        utils::buffer::Buffer m_writeBuffer; // write buffer
        // 请求报文
        std::string m_line; // 状态机每次要解析的行,为了方便直接设为成员变量
        PARSE_STATE m_state; // 状态机当前的状态(START, REQUEST_LINE...)
        std::string m_method;
        std::string m_path;
        std::string m_version;
        std::unordered_map<std::string, std::string> m_header; // 示例 Connection: Keep-Alive
        std::string m_content;
        // 响应报文
        int m_code;
        char* m_file; // 读取的文件
        struct stat m_fileStat; // 文件的基本信息
        int m_iovCnt;
        struct iovec m_iov[2]; // 0包括响应行和头部,1为响应内容(m_file)

    public:
        // 基本组件
        Http(int fd, bool debug = false);
        ~Http() = default;
        Http(const Http&) = delete;
        Http& operator = (const Http&) = delete;
        Http(Http&&) = delete;
        Http& operator = (Http&&) = delete;
        ssize_t readRequest(int fd, int *ern); // 从readBuffer中读取请求
        ssize_t writeResponse(int fd, int *ern); // 将响应报文写入writeBuffer
        void clearBuffer(); // 清空缓冲区,初始化时用
        // request & response
        void initNextHttp(); // 初始化状态机(注意这里不初始化buffer)
        // 处理用户的请求(调用api或读取文件)
        // true表示之后可以发送响应报文了(有错误的话code会相应地设为BAD_REQUEST,INTERNAL_ERROR等),false表示数据不完整,还需要继续等待数据到来
        // userFunction是用户自定义的api
        bool process(std::unordered_map<std::string, std::function<void(Http*)>> *userFunction);
        std::string getMethod() const;
        std::string getPath() const;
        std::string getVersion() const;
        std::string getContent() const;
        bool isKeepAlive() const; // 是否是长连接,短连接时server会在发送完响应报文后关闭连接
        int getCode() const;
        size_t toWriteBytes(); // iov中还需要发送的数据量,用来判断响应报文是否发🎴完
        bool hasError() const; // 上文提到过当状态码为BAD_REQUEST或INTERNAL_ERROR时服务器关闭连接,hasError就是为此作判断的
        void unMapFile(); // 用mmap后需要unmap

    private:
        // request & response
        bool readLine(); // true:get a line, false:数据不完整
        size_t getContentLength(); // 获取Content长度,用于判断是否需要继续处理content,现阶段只能处理GET就假设为0了
        bool parseRequestLine();
        bool parseHeader();
        bool parseContent(); // todo
        void execute(std::unordered_map<std::string, std::function<void(Http*)>> *userFunction);
        void addStateLine();
        void addHeader();
        void addContent();
    };
}}

// http报文格式示例:
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
