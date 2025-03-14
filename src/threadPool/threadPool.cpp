#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>

#include "threadPool.hpp"

ThreadPool::ThreadPool(size_t num_threads)
    : num_threads(num_threads), stop(false), active_tasks(0) {
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    condition.wait(lock, [this] {
                        return !tasks.empty() || stop;
                    });
                    if (stop && tasks.empty()) {
                        return;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                    ++active_tasks;
                }
                task();
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    --active_tasks;
                    if (tasks.empty() && active_tasks == 0) {
                        condition.notify_all();
                    }
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (auto& thread : threads) {
        thread.join();
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(std::move(task));
    }
    condition.notify_one();
}

void ThreadPool::wait_for_tasks() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    condition.wait(lock, [this] {
        return tasks.empty() && active_tasks == 0;
    });
}

size_t ThreadPool::getNumThreads() const {
    return num_threads;
}
