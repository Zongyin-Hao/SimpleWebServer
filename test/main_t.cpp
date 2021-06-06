#include "utils_t/concurrence_t/ThreadPoolTest.hpp"
#include "utils_t/buffer_t/BufferTest.hpp"
#include "transmission_t/webserver_t/WebServerTest.hpp"
#include <iostream>
using std::cout;
using std::endl;

int main() {
//    utils_t::concurrence_t::ThreadPoolTest threadPoolTest;
//    threadPoolTest.start();
//    utils_t::buffer_t::BufferTest bufferTest;
//    bufferTest.test();
    transmission_t::webserver_t::WebServerTest webServerTest;
    webServerTest.test();
    return 0;
}
