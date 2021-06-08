//
// Created by hzy on 6/2/21.
//

#include "transmission/webserver/WebServer.h"
#include "utils/Error.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <iostream>
#include <cassert>

namespace transmission { namespace webserver {
    WebServer::WebServer(int port, int threadNum, bool debug) :
    m_fd(-1), isClosed(false), m_event(0), m_userEvent(0),
    m_epoll(), m_threadPool(threadNum), m_userCnt(0), m_debug(debug) {
        // initNextHttp socket
        int ret;
        if (port > 65535 || port < 1024) {
            utils::Error::Throw(utils::Error::PORT_RANGE_ERROR);
        }
        struct sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(port);
        struct linger ling = {0};
        ling.l_onoff = 1;
        ling.l_linger = 1; // 1s
        // create socket
        m_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_fd < 0) {
            utils::Error::Throw(utils::Error::CREATE_SOCKET_ERROR);
        }
        // config socket
        ret = setsockopt(m_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
        if (ret < 0) {
            utils::Error::Throw(utils::Error::CFG_SOCKET_ERROR);
        }
        int optval = 1; // !!!
        ret = setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
        if(ret == -1) {
            utils::Error::Throw(utils::Error::CFG_SOCKET_ERROR);
        }
        // bind
        ret = bind(m_fd, (struct sockaddr *)&address, sizeof(address));
        if (ret < 0) {
            utils::Error::Throw(utils::Error::BIND_ERROR);
        }
        // 3 listen
        // max block: 8
        ret = listen(m_fd, 8);
        if (ret < 0) {
            utils::Error::Throw(utils::Error::LISTEN_ERROR);
        }
        // 4 initNextHttp epoll
        m_event = EPOLLRDHUP | EPOLLET;
        m_userEvent = EPOLLRDHUP | EPOLLET| EPOLLONESHOT; // important!
        ret = m_epoll.addFd(m_fd, m_event | EPOLLIN);
        if (ret == 0) {
            utils::Error::Throw(utils::Error::INIT_EPOLL_ERROR);
        }
        setNonBlocking(m_fd); // important!
    }

    WebServer::~WebServer() {
        for (auto & m_user : m_users) {
            delete m_user.second;
            m_user.second = nullptr;
        }
        if (!isClosed) {
            isClosed = true;
            close(m_fd);
        }
    }

    // non blocking
    void WebServer::setNonBlocking(int fd) {
        assert(fd > 0);
        int oldOption = fcntl(fd, F_GETFL);
        int newOption = oldOption | O_NONBLOCK;
        fcntl(fd, F_SETFL, newOption);
    }

    void WebServer::start() {

//        // test
//        m_threadPool.addTask([this]{
//            std::this_thread::sleep_for(std::chrono::seconds(10));
//            std::cout << this->m_userCnt << std::endl;
//        });

        while (!isClosed) {
            if (m_debug) {
                std::cout << "========================================" << std::endl;
            }
            // blocking
            int eventCnt = m_epoll.wait(-1);
            // process events
            for (int i = 0; i < eventCnt; i++) {
                int fd = m_epoll.getFd(i);
                uint32_t events = m_epoll.getEvent(i);
                if (fd == m_fd) {
                    addUser();
                } else if (events & (EPOLLRDHUP|EPOLLHUP|EPOLLERR)) {
                    delUser(fd);
                } else if (events & EPOLLIN) {
                    processRead(fd);
                } else if (events & EPOLLOUT) {
                    processWrite(fd);
                } else {
                    utils::Error::Throw(utils::Error::EPOLL_UNEXPECTED_ERROR);
                }
            }
        }
    }

    void WebServer::addUser() {
        struct sockaddr_in address = {};
        socklen_t len = sizeof(address);
        while (true) {
            int fd = accept(m_fd, (struct sockaddr*)&address, &len);
            if (fd < 0 || m_userCnt >= MAX_USER) {
                return;
            }
            setNonBlocking(fd);
            if (m_users.count(fd) == 0) {
                m_users[fd] = new Http(fd, m_debug);
            } else {
                Http* user = m_users[fd];
                user->clearBuffer();
                user->initNextHttp();
            }
            m_userCnt++;
            m_epoll.addFd(fd, m_userEvent | EPOLLIN);
            if (m_debug) {
                std::cout << "add user, fd = " << fd << std::endl;
                std::cout << "userCnt = " << m_userCnt << std::endl;
            }
        }
    }

    void WebServer::delUser(int fd) {
        assert(m_users.count(fd) > 0);
        Http* user = m_users[fd];
        assert(user != nullptr);
        m_userCnt--;
        m_epoll.delFd(fd);
        close(fd);
        if (m_debug) {
            std::cout << "delete user, fd = " << fd << std::endl;
            std::cout << "userCnt = " << m_userCnt << std::endl;
        }
    }

    void WebServer::processRead(int fd) {
        assert(m_users.count(fd) > 0);
        Http* user = m_users[fd];
        assert(user != nullptr);
        if (m_debug) {
            std::cout << "epoll in" << std::endl;
        }
        m_threadPool.addTask([this, fd, user] {
            // read request
            while (true) {
                int ern = 0;
                ssize_t ret = user->readRequest(fd, &ern);
                if (ret < 0) {
                    if (ern == EAGAIN || ern == EWOULDBLOCK) {
                        break;
                    }
                    if (m_debug) {
                        std::cout << "processRead:ret < 0:delete user" << std::endl;
                    }
                    delUser(fd);
                    return;
                } else if (ret == 0) {
                    if (m_debug) {
                        std::cout << "processRead:ret == 0:delete user" << std::endl;
                    }
                    delUser(fd);
                    return;
                }
            }
            if (user->process(&userFunction)) {
                m_epoll.modFd(fd, m_userEvent | EPOLLOUT);
            } else {
                m_epoll.modFd(fd, m_userEvent | EPOLLIN);
            }
        });
    }

    void WebServer::processWrite(int fd) {
        assert(m_users.count(fd) > 0);
        Http* user = m_users[fd];
        assert(user != nullptr);
        if (m_debug) {
            std::cout << "epoll out" << std::endl;
        }
        m_threadPool.addTask([this, fd, user] {
            while (true) {
                int ern = 0;
                ssize_t ret = user->writeResponse(fd, &ern);
                if (ret < 0) {
                    if (ern == EAGAIN || ern == EWOULDBLOCK) {
                        break;
                    }
                    if (m_debug) {
                        std::cout << "processWrite:ret < 0:delete user" << std::endl;
                    }
                    delUser(fd);
                } else if (ret == 0) {
//                    assert(false);
                    break;
                }
            }
            if (user->toWriteBytes() == 0) {
                // buffer dirty
                if (user->hasError()) {
                    if (m_debug) {
                        std::cout << "processWrite:has error:delete user" << std::endl;
                    }
                    delUser(fd);
                    return;
                }
                if (!user->isKeepAlive()) {
                    if (m_debug) {
                        std::cout << "processWrite:keep alive:delete user" << std::endl;
                    }
                    delUser(fd);
                    return;
                }
                user->initNextHttp();
                if (user->process(&userFunction)) {
                    m_epoll.modFd(fd, m_userEvent | EPOLLOUT);
                } else {
                    m_epoll.modFd(fd, m_userEvent | EPOLLIN);
                }
            } else {
                m_epoll.modFd(fd, m_userEvent | EPOLLOUT);
            }
        });
    }
}}