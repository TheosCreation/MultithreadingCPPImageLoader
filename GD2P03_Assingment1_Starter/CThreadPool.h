#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <future>

class CThreadPool {
public:
    explicit CThreadPool(size_t threads) : stop(false), tasksCompleted(0), totalTasks(0) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                    {
                        std::lock_guard<std::mutex> lock(this->completed_mutex);
                        ++tasksCompleted;
                        if (tasksCompleted == totalTasks) {
                            promise.set_value(); // Notify the main thread that all tasks are completed
                        }
                    }
                }
                });
        }
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(f));
            ++totalTasks;
        }
        condition.notify_one();
    }

    ~CThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers)
            worker.join();
    }

    std::future<void> getFuture() {
        return promise.get_future();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
    std::mutex completed_mutex;
    std::atomic<size_t> tasksCompleted;
    size_t totalTasks;
    std::promise<void> promise;
};