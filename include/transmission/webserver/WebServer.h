//
// Created by hzy on 6/2/21.
//

#ifndef SIMPLESYSTEM_WEBSERVER_H
#define SIMPLESYSTEM_WEBSERVER_H

#include "transmission/webserver/Http.h"
#include "utils/concurrence/ThreadPool.h"
#include "utils/nio/Epoll.h"
#include <atomic>

namespace transmission { namespace webserver {
    class WebServer {
    private:
        bool m_debug;
        // socket
        int m_fd;
        bool isClosed;
        // epoll(ET)
        uint32_t m_event;
        uint32_t m_userEvent;
        utils::nio::Epoll m_epoll;
        // thread pool
        utils::concurrence::ThreadPool m_threadPool;
        // user information
        static const int MAX_USER = 65536;
        std::atomic<int> m_userCnt;
        std::unordered_map<int, Http*> m_users; // remember to delete pointer
        // user function
        std::unordered_map<std::string, std::function<void(Http*)> > userFunction;

    public:
        WebServer(int port, int threadNum, bool debug = false);
        ~WebServer();
        void addUserFunction(const std::string& path,
                                    const std::function<void(Http*)>& func) {
            userFunction[path] = func;
        }
        void start();

    private:
        static void setNonBlocking(int fd); // non blocking
        void addUser();
        void delUser(int fd);
        void processRead(int fd); // create a thread to process a request
        void processWrite(int fd); // create a thread to send a response
    };
}}


#endif //SIMPLESYSTEM_WEBSERVER_H
