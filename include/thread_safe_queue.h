// In thread_safe_queue.h
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> internal_queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stop_flag{false};
public:
    ThreadSafeQueue() = default;
    
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void push(T item) {
        std::lock_guard<std::mutex> lock(mtx);
        internal_queue.push(std::move(item));
        cv.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !internal_queue.empty() || stop_flag; });
        if (internal_queue.empty() && stop_flag) {
            return T{};
        }
        T item = std::move(internal_queue.front());
        internal_queue.pop();
        return item;
    }

    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lock(mtx);
        if (internal_queue.empty()) {
            return false;
        }
        item = std::move(internal_queue.front());
        internal_queue.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return internal_queue.empty();
    }

    void stop() {
        std::lock_guard<std::mutex> lock(mtx);
        stop_flag = true;
        cv.notify_all();
    }
};