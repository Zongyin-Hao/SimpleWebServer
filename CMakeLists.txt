cmake_minimum_required(VERSION 3.19)
project(SimpleSystem)

# config cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
# set executable program path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# generate executable program
add_executable(${PROJECT_NAME}
        #src/main.cpp
        test/main_t.cpp

        src/transmission/webserver/Http.cpp
        src/transmission/webserver/WebServer.cpp
        src/utils/Config.cpp
        src/utils/Error.cpp
        src/utils/concurrence/ThreadPool.cpp
        src/utils/nio/Epoll.cpp
        include/utils/buffer/Buffer.h src/utils/buffer/Buffer.cpp

        test/utils_t/concurrence_t/ThreadPoolTest.hpp
        test/utils_t/buffer_t/BufferTest.hpp
        test/transmission_t/webserver_t/WebServerTest.hpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/test
        )