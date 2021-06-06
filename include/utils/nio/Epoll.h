//
// Created by hzy on 5/31/21.
//

#ifndef SIMPLESYSTEM_EPOLL_H
#define SIMPLESYSTEM_EPOLL_H

#include <sys/epoll.h>
#include <cstdint>
#include <vector>

namespace utils { namespace nio {
    class Epoll {
    public:
        explicit Epoll(int maxEvent = 1024);
        ~Epoll();
        Epoll(const Epoll&) = delete;
        Epoll& operator = (const Epoll&) = delete;
        Epoll(Epoll&&) = delete;
        Epoll& operator = (Epoll&&) = delete;
        bool addFd(int fd, uint32_t events);
        bool modFd(int fd, uint32_t events);
        bool delFd(int fd);
        int getFd(int idx) const;
        uint32_t getEvent(int idx) const;
        int wait(int timeoutMs);
    private:
        int m_epollFd;
        std::vector<struct epoll_event> m_events;
    };
}}

#endif //SIMPLESYSTEM_EPOLL_H
