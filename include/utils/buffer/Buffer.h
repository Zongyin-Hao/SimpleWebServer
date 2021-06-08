//
// Created by hzy on 6/1/21.
//

#ifndef SIMPLESYSTEM_BUFFER_H
#define SIMPLESYSTEM_BUFFER_H

#include <vector>
#include <string>

namespace utils { namespace buffer {
    class Buffer {
    private:
//        int test;
        std::vector<char> m_buffer;
        size_t m_readPos;
        size_t m_writePos;
        // [0---m_readPos)->has been read
        // [m_readPos, m_writePos)->has data, but has not been read
        // [m_writePos, bufferSize)->has not data, to be written

    public:
        explicit Buffer(int initBufferSize = 1024); // recommend 1024
        ~Buffer() = default;
        Buffer(const Buffer&) = delete;
        Buffer& operator = (const Buffer&) = delete;
        Buffer(Buffer&&) = delete;
        Buffer& operator = (Buffer&&) = delete;

        size_t availableBytes() const; // len of [0, readPos)
        size_t readableBytes() const; // len of [readPos, writePos)
        size_t writableBytes() const; // len of [writePos, bufferSize)
        char* beginPos() const; // begin position
        char* nextReadPos() const; // next read position
        char* nextWritePos() const; // next write position
        void readBuffer_idx(size_t len); // for iov
        void readBufferAll_idx(); // for iov
        void writeBuffer_idx(size_t len); // for iov
        std::string readBuffer(size_t len); // read [nextReadPos(), nextReadPos()+len)
        std::string readBufferAll(); // read [nextReadPos(), nextReadPos()+readableBytes()), may be more efficient
        void writeBuffer(const char* str, size_t len); // write[nextWritePos(), nextWritePos()+str.length())
        void expansion(size_t len); // expand to len
        void output();
    };
}}


#endif //SIMPLESYSTEM_BUFFER_H
