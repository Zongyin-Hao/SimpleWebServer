//
// Created by hzy on 6/1/21.
//

#include "utils/buffer/Buffer.h"
#include <cstring>
#include <cassert>
#include <iostream>
using std::cout;
using std::endl;

namespace utils { namespace buffer {
    // recommend 1024
    Buffer::Buffer(int initBufferSize) : m_buffer(initBufferSize),
    m_readPos(0), m_writePos(0) {}

    // len of [0, readPos)
    inline size_t Buffer::availableBytes() const {
        return m_readPos;
    }

    // len of [readPos, writePos)
    inline size_t Buffer::readableBytes() const {
        return m_writePos - m_readPos;
    }

    // begin position
    inline char* Buffer::beginPos() const {
        return const_cast<char*>(&*m_buffer.begin());
    }

    // next read position
    inline char* Buffer::nextReadPos() const {
        return beginPos() + m_readPos;
    }

    // next write position
    inline char* Buffer::nextWritePos() const {
        return beginPos() + m_writePos;
    }

    // len of [writePos, bufferSize)
    inline size_t Buffer::writableBytes() const {
        return m_buffer.size() - m_writePos;
    }

    // for iov
    inline void Buffer::readBuffer_idx(size_t len) {
        assert(len <= readableBytes());
        m_readPos += len;
    }

    // for iov
    inline void Buffer::readBufferAll_idx() {
        m_readPos = 0;
        m_writePos = 0;
    }

    // for iov
    inline void Buffer::writeBuffer_idx(size_t len) {
        assert(len <= writableBytes());
        m_writePos += len;
    }

    // read [nextReadPos(), nextReadPos()+len)
    inline std::string Buffer::readBuffer(size_t len) {
        assert(len <= readableBytes());
        std::string str(nextReadPos(), len);
        m_readPos += len;
        return str;
    }

    // read [nextReadPos(), nextReadPos()+readableBytes())
    // may be more efficient
    inline std::string Buffer::readBufferAll() {
        std::string str(nextReadPos(), readableBytes());
        m_readPos = 0;
        m_writePos = 0;
        return str;
    }

    // write[nextWritePos(), nextWritePos()+str.length())
    void Buffer::writeBuffer(const char* str, size_t len) {
        // ensure writable
        if (writableBytes() < len) {
            expansion(len);
        }
        assert(writableBytes() >= len);
        std::copy(str, str+len, nextWritePos());
        m_writePos += len;
    }

    // expand to len
    void Buffer::expansion(size_t len) {
        if (writableBytes() + availableBytes() < len) {
            m_buffer.resize(m_writePos+len+1);
        } else {
            char* bp = beginPos();
            std::copy(bp+m_readPos, bp+m_writePos, bp);
            size_t rdb = m_writePos - m_readPos;
            m_readPos = 0;
            m_writePos = rdb;
        }
    }

    void Buffer::output() {
        cout << "--------------------" << endl;
        for (size_t i = m_readPos; i < m_writePos; i++) {
             cout << m_buffer[i];
        }
        cout << "----------" << endl;
    }
}}