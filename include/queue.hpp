#ifndef CODE_QUEUE_HPP
#define CODE_QUEUE_HPP

#include "../include/type.hpp"

template<class T>
class Queue {
    T *storage = nullptr;
    Index size = 0;
    Index front = 0, rear = 0;
public:
    explicit Queue(const Index &size_) : size(size_) {
        storage = new T[size_];
    }

    ~Queue() {
        delete[] storage;
    }

    bool Empty() {
        if (rear == front)return true;
        return false;
    }

    bool Full() {
        if ((rear + 1) % size == front)return true;
        return false;
    }

    T GetHead() {
        return storage[(front + 1) % size];
    }

    void DeQueue() {
        front = (front + 1) % size;
    }

    //return index in storage
    Index EnQueue(const T &ele) {
        rear = (rear + 1) % size;
        storage[rear] = ele;
        return rear;
    }

    T GetEle(const Index &ind) {
        return storage[ind];
    }

    void Modify(const T &ele, const Index &ind) {
        storage[ind] = ele;
    }

    void Clear() {
        front = rear = 0;
    }

    void HeadRear(Index &front_, Index &rear_) const {
        front_ = front;
        rear_ = rear;
    }
};

#endif //CODE_QUEUE_HPP
