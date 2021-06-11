#include "utils/nio/Epoll.h"
#include <cassert>
#include <unistd.h>

namespace utils { namespace nio {
    Epoll::Epoll(int maxEvent) : m_epollFd(epoll_create(1024)),
    m_events(maxEvent) {
        // epoll_create(int size);
        assert(m_epollFd >= 0 && !m_events.empty());
    }

    Epoll::~Epoll() {
        close(m_epollFd);
    }

    bool Epoll::addFd(int fd, uint32_t events) {
        if (fd < 0) {
            return false;
        }
        // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
        // success: return 0 / failed: return -1
        // typedef union epoll_data
        // {
        //     void *ptr;
        //     int fd;
        //     __uint32_t u32;
        //     __uint64_t u64
        // }epoll_data_t;
        // struct epoll_event
        // {
        //   __uint32_t events;  /* Epoll events */
        //   epoll_data_t data;  /* User data variable */
        // };
        epoll_event ev = {};
        ev.data.fd = fd;
        ev.events = events;
        return 0 == epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev);
    }

    bool Epoll::modFd(int fd, uint32_t events) {
        if (fd < 0) {
            return false;
        }
        epoll_event ev = {};
        ev.data.fd = fd;
        ev.events = events;
        return 0 == epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &ev);
    }

    bool Epoll::delFd(int fd) {
        if (fd < 0) {
            return false;
        }
        epoll_event ev = {};
        ev.data.fd = fd;
        return 0 == epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &ev);
    }

    int Epoll::getFd(int idx) const {
        assert(0 <= idx && idx < m_events.size());
        return m_events[idx].data.fd;
    }

    uint32_t Epoll::getEvent(int idx) const {
        assert(0 <= idx && idx < m_events.size());
        return m_events[idx].events;
    }

    int Epoll::wait(int timeoutMs) {
        // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
        // return the number of events witch need to be processed
        return epoll_wait(m_epollFd, &m_events[0], static_cast<int>(m_events.size()), timeoutMs);
    }
}}