#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <future>
#include <functional>
#include <memory>


class ThreadPool {
public:
    using Task = std::packaged_task<void()>;

    ThreadPool(int numThreads=std::thread::hardware_concurrency()):
               numThreads_(numThreads), stop_(false)
    {
        int num = std::thread::hardware_concurrency();
        numThreads_ = numThreads > num ? num : numThreads;
        start();
    }

    ~ThreadPool()
    {
        stop_ = true;
        cv_.notify_all();
        for (auto& t : pool_) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    template<class F, class... Args>
    auto commit_task(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        using return_type = decltype(f(args...));
        // using return_type = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        auto ret = task->get_future();

        {
            std::unique_lock<std::mutex> locker(mtx_);
            if (stop_) {
                throw std::runtime_error("commit task on stopped ThreadPool.");
            }
            tasks_.emplace([task]() {
                (*task)();
            });
        }
        cv_.notify_one();

        return ret;
    }
    

private:
    void start()
    {
        for (int i = 0; i < numThreads_; ++i) {
            pool_.emplace_back([this](){
                Task task;
                while (true) {
                    {
                        std::unique_lock<std::mutex> locker(this->mtx_);
                        this->cv_.wait(locker, [this]() {
                            // stop_为true或任务队列不为空，则不应该被阻塞
                            return this->stop_ || !this->tasks_.empty();
                        });
                        if (this->stop_ && tasks_.empty()) {
                            return;
                        }

                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }

                    task();
                }
            });
        }
    }


private:
    std::vector<std::thread> pool_;
    std::queue<Task> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<int> numThreads_;
    std::atomic<bool> stop_;
};


#endif  /* THREAD_POOL_H */ 