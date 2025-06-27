//
// Created by yashas on 6/26/25.
//

#pragma once

#include <vector>
#include <thread>
#include <functional>
#include "thread_safe_queue.h"

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();
    void submit(std::function<void()> job);

private:
    void worker_loop();

    std::vector<std::thread> workers;
    ThreadSafeQueue<std::function<void()>> job_queue;
};