关于fd非阻塞:
主套接字非阻塞:while(){accept}
从套接字非阻塞:while(){read or write}

EPOLLONESHOT:一个fd只触发一次event,避免多线程处理导致并法错误.
注意每次epoll到这个事件后一定要重置

优雅的关闭:(0,-):等待数据发送完,正常FIN.(1,0):丢弃,RST.(1,x:>0):在xs内尝试正常FIN,超时RST
struct linger ling = {0};
ling.l_onoff = 1;
ling.l_linger = 3; // 3s

EPOLLRDHUP:识别对端close(EPOLLHUP和EPOLLERR不用注册)

EAGAIN:ET模式重要宏,代表内核缓冲区已经没数据了,跳出while