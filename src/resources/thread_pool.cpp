//
// Created by yashas on 6/26/25.
//

#include "thread_pool.h"

ThreadPool::ThreadPool(const size_t numThreads) {
  workers.reserve(numThreads);
  for (size_t i = 0; i < numThreads; ++i) {
    workers.emplace_back(&ThreadPool::worker_loop, this);
  }
}

ThreadPool::~ThreadPool() {
  job_queue.stop();
  for (auto& worker : workers) {
    if(worker.joinable()) {
      worker.join();
    }
  }
}

void ThreadPool::worker_loop() {
  while (true) {
    std::function<void()> job = job_queue.pop();
    if (job) {
      job();
    } else {
      break;
    }
  }
}

void ThreadPool::submit(std::function<void()> job) {
  job_queue.push(std::move(job));
}