cmake_minimum_required(VERSION 3.19)
project(SimpleSystem)

# config cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
# set executable program path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# get *.cpp in src
file(GLOB_RECURSE SRC src/*.cpp)
# generate executable program
add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/include)