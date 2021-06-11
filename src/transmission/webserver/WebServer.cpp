#include "transmission/webserver/WebServer.h"
#include "utils/Error.h"
#include <sys/socket.h> // socket
#include <netinet/in.h> // AF_INET
#include <unistd.h> // close
#include <fcntl.h> // F_SETFL
#include <iostream>
#include <cassert>

namespace transmission { namespace webserver {
    WebServer::WebServer(int port, int threadNum, bool debug) :
    m_fd(-1), isClosed(false), m_event(0), m_userEvent(0),
    m_epoll(), m_threadPool(threadNum), m_userCnt(0), m_debug(debug) {
        // init socket
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
        // create socket - tcp
        m_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_fd < 0) {
            utils::Error::Throw(utils::Error::CREATE_SOCKET_ERROR);
        }
        // config socket
        // 1s内尝试FIN, 超时RST
        ret = setsockopt(m_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
        if (ret < 0) {
            utils::Error::Throw(utils::Error::CFG_SOCKET_ERROR);
        }
        int optval = 1; // 不设置这个的话程序异常结束重启时socket会间歇性绑定失败
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
        // EPOLLRDHUP是为了捕获对端关闭事件(EPOLLHUP和EPOLLERR不用注册)
        // EPOLLONESHOT保证线程安全!
        m_event = EPOLLRDHUP | EPOLLET;
        m_userEvent = EPOLLRDHUP | EPOLLET| EPOLLONESHOT;
        ret = m_epoll.addFd(m_fd, m_event | EPOLLIN);
        if (ret == 0) {
            utils::Error::Throw(utils::Error::INIT_EPOLL_ERROR);
        }
        setNonBlocking(m_fd); // 非阻塞
    }

    WebServer::~WebServer() {
        // 手动释放内存(智能指针用的时候不是很方便就放弃了)
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
        // 由于设置了oneshot每个fd只会被一个线程处理,因此每个事件内部是无数据竞争的
        // 但不同事件间可能有数据竞争,这个在delUser里作了处理(分析一下会发现就delUser会有问题,其余函数都是线程安全的)
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
            // 举个例子,fd=4被close以后下次accept的fd可能还是4,也就是说一个序号会被重复利用
            // 因此我没有delete掉Http,而是每次重新初始化,这样空间会耗费一些,但就不用每次都new了,会快一点
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
        assert(m_users[fd] != nullptr);
        // 由于多个任务可能并发调用delUser,这里会发生数据竞争,所以用了原子变量
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
            // 处理请求
            if (user->process(&userFunction)) {
                // 注意由于设置了oneshot这里要更新一下才能继续使用这个fd
                m_epoll.modFd(fd, m_userEvent | EPOLLOUT);
            } else {
                // 请求不完整,继续等待数据
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
                // 短连接时server会在发送完响应报文后关闭连接
                if (!user->isKeepAlive()) {
                    if (m_debug) {
                        std::cout << "processWrite:keep alive:delete user" << std::endl;
                    }
                    delUser(fd);
                    return;
                }
                // 如process中描述的那样, 多条请求时要在response发完后重新调用process,形成一个环路
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