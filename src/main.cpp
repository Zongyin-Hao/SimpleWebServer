#include <iostream>
#include "transmission/webserver/WebServer.h"
using std::cout;
using std::endl;

int main() {
    transmission::webserver::WebServer webServer(55555, 16, false);
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
    return 0;
}
