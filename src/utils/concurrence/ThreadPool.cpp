#include "utils/concurrence/ThreadPool.h"

#include <cassert>
#include <iostream>
using std::cout;
using std::endl;

namespace utils { namespace concurrence {
    ThreadPool::ThreadPool(uint16_t threadNum, bool debug) : m_debug(debug),
    stop{false}, idleThreadNum{0} {
        assert(threadNum > 0);
        idleThreadNum.store(threadNum);
        for (int i = 0; i < threadNum; i++) {
            m_threadPool.emplace_back(std::thread([this] {
                if (m_debug) {
                    cout << "thread " << std::this_thread::get_id() << " start" << endl;
                }
                while (!stop.load()) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lk(m_mutex);
                        m_cond.wait(lk, [this] {
                            return stop.load() || !m_taskQueue.empty();
                        });
                        if (stop.load()) {
                            if (m_debug) {
                                cout << "thread " << std::this_thread::get_id() << " stop" << endl;
                            }
                            return;
                        }
                        task = std::move(m_taskQueue.front());
                        m_taskQueue.pop();
                    }
                    idleThreadNum--;
                    task();
                    idleThreadNum++;
                }
                if (m_debug) {
                    cout << "thread " << std::this_thread::get_id() << " stop" << endl;
                }
            }));
        }
    }

    ThreadPool::~ThreadPool() {
        stop.store(true);
        m_cond.notify_all();
        for (std::thread& thread : m_threadPool) {
            thread.join();
        }
    }

    void ThreadPool::addTask(const Task &task) { // 这里不知道怎么弄完美转移,用范型一直出错...
        if (stop.load()) return;
        {
            std::lock_guard<std::mutex> lkg(m_mutex);
            m_taskQueue.emplace(task);
        }
        m_cond.notify_one();
    }

    int ThreadPool::getIdleThreadNum() {
        return idleThreadNum;
    }
}}
