//
// Created by hzy on 6/5/21.
//

#ifndef SIMPLESYSTEM_WEBSERVERTEST_HPP
#define SIMPLESYSTEM_WEBSERVERTEST_HPP

#include "transmission/webserver/WebServer.h"

namespace transmission_t { namespace webserver_t {
    class WebServerTest {
    public:
        void test() {
            transmission::webserver::WebServer::addUserFunction("/test",
            [](std::shared_ptr<transmission::webserver::Http> user) {
                cout << "@@@@@ Method = " << user->getMethod() << endl;
                cout << "@@@@@ Path = " << user->getPath() << endl;
                cout << "@@@@@ Version = " << user->getVersion() << endl;
                cout << "@@@@@ isKeepAlive = " << user->isKeepAlive() << endl;
            });
            transmission::webserver::WebServer webServer(55555, 1, 16, true);
            webServer.start();
        }
    };
}}

#endif //SIMPLESYSTEM_WEBSERVERTEST_HPP
