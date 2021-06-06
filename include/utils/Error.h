#ifndef SIMPLESYSTEM_ERROR_H
#define SIMPLESYSTEM_ERROR_H


namespace utils {
    class Error {
    public:
        enum {
            // transmission
            // webserver
            PORT_RANGE_ERROR, CREATE_SOCKET_ERROR, CFG_SOCKET_ERROR,
            BIND_ERROR, LISTEN_ERROR, INIT_EPOLL_ERROR, EPOLL_UNEXPECTED_ERROR,
            SORRY,
        };
        static void Throw(int errCode);
    };
}


#endif //SIMPLESYSTEM_ERROR_H
