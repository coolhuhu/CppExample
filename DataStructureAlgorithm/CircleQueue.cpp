#include <iostream>
#include <vector>



/**
 * 长度固定的循环队列，底层使用vector实现。
*/
template<typename T>
class CircleQueue {
public:
    CircleQueue(int capacity): capacity_(capacity), size_(0) {
        if (capacity < 0) {
            throw std::length_error("cannot create a circular queue with a capacity less than 0");
            // std::abort();
        }
        data_.resize(capacity_ + 1);
        head_ = tail_ = 0;
    }

    ~CircleQueue() = default;

    /**
     * FIXME: T&&
    */
    bool push(T value) {
        if (full()) {
            return false;
        }
        data_[tail_] = value;
        tail_ = (tail_ + 1) % data_.size();
        ++size_;
        return true;
    }

    bool pop() {
        if (empty()) {
            return false;
        }
        head_ = (head_ + 1) % data_.size();
        --size_;
        return true;
    }

    T front() {
        if (empty()) {

        }
        return data_[head_];
    }

    T rear() {
        if (empty()) {
            
        }
        return data_[(tail_ - 1) % data_.size()];
    }

    bool empty() {
        return tail_ == head_;
    }

    bool full() {
        return ((tail_ + 1) % data_.size()) == head_;
    }

    int capacity() const {
        return capacity_;
    }

    int size() const {
        return size_;
    }

private:
    int capacity_;
    int size_;
    std::vector<T> data_;
    int head_;
    int tail_;
};



void TestCircleQueue()
{
    CircleQueue<int> que(10);
    for (int i = 1; i <= 5; ++i) {
        que.push(i);
    }
    std::cout << "que.size() = " << que.size() << std::endl;
    for (int i = 0; i < 3; ++i) {
        que.pop();
    }
    std::cout << "que.size() = " << que.size() << std::endl;
    for (int i = 6; i <= 11; ++i) {
        que.push(i);
    }
    std::cout << "que.size() = " << que.size() << std::endl;
    while (!que.empty()) {
        auto val = que.front();
        que.pop();
        std::cout << val << " ";
    }
    std::cout << std::endl;
    std::cout << "que.size() = " << que.size() << std::endl;
}



int main()
{
    TestCircleQueue();
}