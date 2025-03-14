#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>

class ThreadPool {
    public:
        ThreadPool(size_t num_threads);
        ~ThreadPool();

        void enqueue(std::function<void()> task);
        void wait_for_tasks();

        size_t getNumThreads() const;

    private:
        size_t num_threads;
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable condition;
        std::atomic<size_t> active_tasks;
        bool stop;
};

#endif