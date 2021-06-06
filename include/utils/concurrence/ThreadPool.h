//
// Created by hzy on 5/30/21.
//

#ifndef SIMPLESYSTEM_THREADPOOL_H
#define SIMPLESYSTEM_THREADPOOL_H

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <queue>
#include <vector>

namespace utils { namespace concurrence {
    class ThreadPool {
    private:
        bool m_debug = false; // debug switch
        typedef std::function<void()> Task;
        std::queue<Task> m_taskQueue;
        std::vector<std::thread> m_threadPool;
        std::mutex m_mutex;
        std::condition_variable m_cond;
        std::atomic<bool> stop;
        std::atomic<int> idleThreadNum;
    public:
        explicit ThreadPool(uint16_t threadNum, bool debug = false);
        ~ThreadPool();
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator = (const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator = (ThreadPool&&) = delete;
        void addTask(const Task &task);
        int getIdleThreadNum();
    };
}}


#endif //SIMPLESYSTEM_THREADPOOL_H
