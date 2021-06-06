//
// Created by hzy on 6/1/21.
//

#ifndef SIMPLESYSTEM_THREADPOOLTEST_HPP
#define SIMPLESYSTEM_THREADPOOLTEST_HPP

#include "utils/concurrence/ThreadPool.h"
#include <iostream>
using std::cout;
using std::endl;

namespace utils_t { namespace concurrence_t {
    class ThreadPoolTest {
    public:
        void func(int id);
        void start();
    };

    void ThreadPoolTest::func(int id) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        cout << "task " << id << " working in thread " << std::this_thread::get_id() << endl;
    }

    void ThreadPoolTest::start() {
        utils::concurrence::ThreadPool threadPool(8, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        for (int i = 0; i < 16; i++) {
            threadPool.addTask([this, i]{
                return func(i);
            });
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}}

#endif //SIMPLESYSTEM_THREADPOOLTEST_HPP
