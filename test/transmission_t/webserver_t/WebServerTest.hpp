#ifndef SIMPLESYSTEM_WEBSERVERTEST_HPP
#define SIMPLESYSTEM_WEBSERVERTEST_HPP

#include "transmission/webserver/WebServer.h"

namespace transmission_t { namespace webserver_t {
    class WebServerTest {
    public:
        void test() {
            transmission::webserver::WebServer webServer(55555, 16, 0);
            webServer.addUserFunction("/",
                                      [](transmission::webserver::Http* user) {
                                      });
            webServer.addUserFunction("/test",
                                      [](transmission::webserver::Http* user) {
                                          cout << "@@@@@ Method = " << user->getMethod() << endl;
                                          cout << "@@@@@ Path = " << user->getPath() << endl;
                                          cout << "@@@@@ Version = " << user->getVersion() << endl;
                                          cout << "@@@@@ isKeepAlive = " << user->isKeepAlive() << endl;
                                      });
            webServer.start();
        }
    };
}}

#endif //SIMPLESYSTEM_WEBSERVERTEST_HPP
