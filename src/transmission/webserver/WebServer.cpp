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
using std::cout;
using std::endl;

namespace transmission { namespace webserver {
    WebServer::WebServer(int port, int trigMode, int threadNum, bool debug) :
    m_fd(-1), isClosed(false), m_trigMode(trigMode), m_event(0), m_userEvent(0),
    m_epoll(), m_threadPool(threadNum), m_userCnt(0), m_debug(debug) {
        // init socket
        int ret;
        if (port > 65535 || port < 1024) {
            utils::Error::Throw(utils::Error::PORT_RANGE_ERROR);
        }
        struct sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        struct linger ling = {0};
        ling.l_onoff = 1;
        ling.l_linger = 3; // 3s
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
        // bind
        ret = bind(m_fd, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            utils::Error::Throw(utils::Error::BIND_ERROR);
        }
        // 3 listen
        // max block: 8
        ret = listen(m_fd, 8);
        if (ret < 0) {
            utils::Error::Throw(utils::Error::LISTEN_ERROR);
        }
        // 4 init epoll
        m_event = EPOLLRDHUP;
        m_userEvent = EPOLLRDHUP | EPOLLONESHOT; // important!
        if (m_trigMode == 1) { // ET
            m_event |= EPOLLET;
            m_userEvent |= EPOLLET;
        }
        ret = m_epoll.addFd(m_fd, m_event|EPOLLIN);
        if (ret == 0) {
            utils::Error::Throw(utils::Error::INIT_EPOLL_ERROR);
        }
        setNonBlocking(m_fd); // important!
    }

    WebServer::~WebServer() {
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

    void WebServer::addUserFunction(const std::string& path,
                                const std::function<void(std::shared_ptr<Http>)>& func) {
        Http::userFunction[path] = func;
    }

    void WebServer::start() {
        while (!isClosed) {
            if (m_debug) {
                cout << "========================================" << endl;
            }
            int eventCnt = m_epoll.wait(-1); // blocking
            // process events
            for (int i = 0; i < eventCnt; i++) {
                int fd = m_epoll.getFd(i);
                uint32_t events = m_epoll.getEvent(i);
                if (fd == m_fd) {
                    addUser();
                } else if ((events & EPOLLRDHUP) || (events & EPOLLHUP) || (events & EPOLLERR)) {
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
        struct sockaddr_in addr{};
        socklen_t len = sizeof(addr);
        do {
            int fd = accept(m_fd, (struct sockaddr*)&addr, &len);
            if (fd < 0 || m_userCnt >= MAX_USER) {
                return;
            }
            assert(m_users.count(fd) == 0);
            setNonBlocking(fd);
            m_userCnt++;
            m_users[fd] = std::make_shared<Http>(m_debug);
            m_epoll.addFd(fd, m_userEvent | EPOLLIN);
            if (m_debug) {
                cout << "add user, fd = " << fd << endl;
                cout << "userCnt = " << m_userCnt << endl;
            }
        } while (m_trigMode == 1);
    }

    void WebServer::delUser(int fd) {
        assert(m_users.count(fd) > 0);
        std::shared_ptr<Http> user = m_users[fd];
        assert(user != nullptr);
        user->unMapFile();
        close(fd);
        m_userCnt--;
        m_users.erase(fd);
        m_epoll.delFd(fd);
        if (m_debug) {
            cout << "delete user, fd = " << fd << endl;
            cout << "userCnt = " << m_userCnt << endl;
        }
    }

    void WebServer::processRead(int fd) {
        assert(m_users.count(fd) > 0);
        std::shared_ptr<Http> user = m_users[fd];
        assert(user != nullptr);
        if (m_debug) {
            cout << "epoll in" << endl;
        }
        m_threadPool.addTask([this, fd, user] {
            // read request
            do {
                int ern = 0;
                ssize_t ret = user->readRequest(fd, &ern);
                if (ret <= 0) {
                    if (ern == EAGAIN) {
                        break;
                    }
                    delUser(fd);
                    return;
                }
            } while (m_trigMode == 1); // ET
            if (user->process()) {
                m_epoll.modFd(fd, m_userEvent | EPOLLOUT);
            } else {
                m_epoll.modFd(fd, m_userEvent | EPOLLIN);
            }
        });
    }

    void WebServer::processWrite(int fd) {
        assert(m_users.count(fd) > 0);
        std::shared_ptr<Http> user = m_users[fd];
        assert(user != nullptr);
        if (m_debug) {
            cout << "epoll out" << endl;
        }
        m_threadPool.addTask([this, fd, user] {
            do {
                int ern = 0;
                ssize_t ret = user->writeResponse(fd, &ern);
                if (ret <= 0) {
                    if (ern == EAGAIN) {
                        break;
                    }
                    delUser(fd);
                    return;
                }
            } while(m_trigMode == 1);
            if (user->toWriteBytes() == 0) {
                if (user->hasError() || !user->isKeepAlive()) {
                    delUser(fd);
                    return;
                }
                user->init();
                if (user->process()) {
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