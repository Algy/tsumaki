#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <chrono>
#include <iostream>

namespace tsumaki::threadutil {
    template<typename T>
    class UniquePtrQueue {
    private:
        const unsigned int max_size;
        std::mutex mutex;
        std::condition_variable cv_not_full;
        std::condition_variable cv_not_empty;
        std::deque<std::unique_ptr<T>> buffer;

    public:
        UniquePtrQueue(unsigned int max_size);
        bool put(std::unique_ptr<T> &ptr, unsigned int timeout);
        void put_replace(std::unique_ptr<T> ptr);
        std::unique_ptr<T> get(unsigned int timeout);
        bool empty();
    };

    template <typename T>
    UniquePtrQueue<T>::UniquePtrQueue(unsigned int max_size) : max_size(max_size) {};

    template <typename T>
    bool UniquePtrQueue<T>::put(std::unique_ptr<T> &ptr, unsigned int timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        if (buffer.size() >= max_size) {
            if (cv_not_full.wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout) {
                return false;
            };
        }
        buffer.push_back(ptr);
        lock.unlock();
        cv_not_empty.notify_one();
        return true;
    }

    template <typename T>
    void UniquePtrQueue<T>::put_replace(std::unique_ptr<T> ptr) {
        std::unique_lock<std::mutex> lock(mutex);
        if (buffer.size() >= max_size) {
            // replace if queue is empty
            buffer.pop_front();
        }
        buffer.push_back(std::move(ptr));
        lock.unlock();
        cv_not_empty.notify_one();
    }

    template <typename T>
    std::unique_ptr<T> UniquePtrQueue<T>::get(unsigned int timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        if (buffer.empty()) {
            if (cv_not_empty.wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout) {
                return nullptr;
            }
        }
        std::unique_ptr<T> result = std::move(buffer.front());
        buffer.pop_front();
        lock.unlock();
        cv_not_full.notify_one();
        return result;
    }

    template <typename T>
    bool UniquePtrQueue<T>::empty() {
        std::unique_lock<std::mutex> lock(mutex);
        return buffer.empty();
    }
};
