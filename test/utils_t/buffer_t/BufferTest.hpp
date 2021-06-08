//
// Created by hzy on 6/4/21.
//

#ifndef SIMPLESYSTEM_BUFFERTEST_HPP
#define SIMPLESYSTEM_BUFFERTEST_HPP

#include "utils/buffer/Buffer.h"
#include <iostream>

namespace utils_t { namespace buffer_t {
    class BufferTest {
    public:
        std::string str = "abcdefghijklmnopqrstuvwxyz1234567890-=";
        int r = 0, w = 0, r_ = 0, w_ = 0, buffersz = 20;
        utils::buffer::Buffer buffer;
        BufferTest(): buffer(20) {}

        void point(int no) {
            bool flag = true;
            std::cout << "====================test" << no << "====================" << std::endl;
            std::cout << "available: " << buffer.availableBytes() << " " << r_ << std::endl;
            flag = (flag && (buffer.availableBytes() == r_));
            std::cout << "readable: " << buffer.readableBytes() << " " << w-r << std::endl;
            flag = (flag && (buffer.readableBytes() == (w-r)));
            std::cout << "writable: " << buffer.writableBytes() << " " << buffersz-w_ << std::endl;
            flag = (flag && (buffer.writableBytes() == (buffersz-w_)));
            if (buffer.nextReadPos() != buffer.nextWritePos()) {
                std::cout << "next_read_pos: " << *buffer.nextReadPos() << " " << str[r] << std::endl;
                flag = (flag && (*buffer.nextReadPos() == str[r]));
                std::cout << "next_write_pos: " << *(buffer.nextWritePos()-1) << " " << str[w-1] << std::endl;
                flag = (flag && (*(buffer.nextWritePos()-1) == str[w-1]));
            }
            if (flag) {
                std::cout << "passed" << std::endl;
            } else {
                std::cout << "error" << std::endl;
                exit(-1);
            }
        }

        void test() {
            bool flag = true;
            // test1
            buffer.writeBuffer(str.substr(0, 12).c_str(), 12);
            w += 12;
            w_ += 12;
            point(1);
            // test2
            r += 8;
            r_ += 8;
            buffer.readBuffer(8);
            point(2);
            // test3
            buffer.writeBuffer(str.substr(w, w+6).c_str(), 6);
            w += 6;
            w_ += 6;
            point(3);
            // test4
            r += 2;
            r_ += 2;
            buffer.readBuffer(2);
            point(4);
            // test5
            buffer.writeBuffer(str.substr(w, w+4).c_str(), 4);
            w += 4;
            w_ += 4;
            // r = 10, w = 22
            // r_ = 10, w_ = 22
            r_ = 0, w_ = 12;
            point(5);
            // test6
            r += 2;
            r_ += 2;
            buffer.readBuffer(2);
            point(6);
            // test7
            buffer.writeBuffer(str.substr(w, w+12).c_str(), 12);
            w += 12;
            w_ += 12;
            // r = 12, 2 = 34
            // r_ = 2, w_ = 24
            buffersz = 25;
            point(7);
            // test8
            r = w;
            r_ = 0, w_ = 0;
            buffer.readBufferAll();
            point(8);
            // test9
            buffer.writeBuffer(str.substr(w, w+2).c_str(), 2);
            w += 2;
            w_ += 2;
            point(9);
            // test10
            r += 1;
            r_ += 1;
            buffer.readBuffer(1);
            point(10);
        }
    };
}}

#endif //SIMPLESYSTEM_BUFFERTEST_HPP
