//
// Created by zhangli on 2021/7/22.
//

#ifndef LIVE_BLOCKQUEUE_H
#define LIVE_BLOCKQUEUE_H

#include<mutex>
#include<list>
#include<condition_variable>
#include <android_log.h>

using namespace std;

template<typename T>
class BlockQueue {
public:
    BlockQueue(T defaultValue) {
        quited = false;
        this->defaultValue = defaultValue;
    }

    ~BlockQueue() {}

    void put(const T &x) {
        unique_lock<mutex> guard(m_mutex);
        if (quited) {
            return;
        }
        m_queue.push_back(x);
        m_cond.notify_all();
    }

    void put(T &&x) {
        unique_lock<mutex> guard(m_mutex);
        if (quited) {
            LOGI("put fail, queue has quit");
            return;
        }
        m_queue.push_back(move(x));
        m_cond.notify_all();
    }

    T take() {
        unique_lock<mutex> guard(m_mutex);
        while (m_queue.size() == 0 && !quited)
            m_cond.wait(guard);
        if (quited) {
            return defaultValue;
        }
        T front(move(m_queue.front()));
        m_queue.pop_front();
        return move(front);
    }

    size_t size() {
        unique_lock<mutex> guard(m_mutex);
        return m_queue.size();
    }

    void clearAll() {
        unique_lock<mutex> guard(m_mutex);
        m_queue.clear();
        m_cond.notify_all();
    }

    void clear(T msg) {
        unique_lock<mutex> guard(m_mutex);
        m_queue.remove(msg);
        m_cond.notify_all();
    }

    void quit() {
        unique_lock<mutex> guard(m_mutex);
        quited = true;
        m_queue.clear();
        m_cond.notify_all();
    }

    bool isQuit() {
        return quited;
    }

private:
    bool quited;
    T defaultValue;
    mutable mutex m_mutex;
    condition_variable m_cond;
    list<T> m_queue;
};

#endif //LIVE_BLOCKQUEUE_H
