#include "utils/Error.h"

#include <string>

namespace utils {
    void Error::Throw(int errCode) {
        std::string errmsg;
        switch (errCode) {
            case PORT_RANGE_ERROR:
                errmsg = "port_range_error";
                break;
            case CREATE_SOCKET_ERROR:
                errmsg = "create socket error";
                break;
            case CFG_SOCKET_ERROR:
                errmsg = "config socket error";
                break;
            case BIND_ERROR:
                errmsg = "bind error";
                break;
            case LISTEN_ERROR:
                errmsg = "listen error";
                break;
            case INIT_EPOLL_ERROR:
                errmsg = "init epoll error";
                break;
            case EPOLL_UNEXPECTED_ERROR:
                errmsg = "epoll unexpected error";
                break;
            case SORRY:
                errmsg = "sorry";
                break;
            default:
                errmsg = "undefined error";
                break;
        }
        throw errmsg;
    }
}