/*
default runtime utility
*/
#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>
#include <memory>

namespace tsr::runtime
{
    class ThreadPool
    {
      public:
        explicit ThreadPool(std::size_t worker_count)
        {
            workers_.reserve(worker_count);

            for (std::size_t i = 0; i < worker_count; i++)
            {
                workers_.emplace_back([this] { WorkerLoop(); });// start workers
            }
        }

        ~ThreadPool()
        {
            {
                std::lock_guard lock(mutex_);
                stop_ = true;
            }

            cv_.notify_all();

            for (auto& worker : workers_)
            {
                if (worker.joinable())
                {
                    worker.join();
                }
            }
        }

        template <typename F>
        auto Submit(F&& f)
        {
            using result_t = std::invoke_result_t<F>;

            auto task = std::make_shared<std::packaged_task<result_t()>>(std::forward<F>(f));

            auto future = task->get_future();

            {
                std::lock_guard lock(mutex_);

                if (stop_)
                {
                    throw std::runtime_error("Submit on stopped ThreadPool");
                }

                tasks_.push([task] { (*task)(); });
            }

            cv_.notify_one();

            return future;
        }

      private:
        void WorkerLoop()
        {
            while (true)
            {
                std::function<void()> task;

                {// get task
                    std::unique_lock lock(mutex_);
                    cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

                    if (stop_ && tasks_.empty())
                    {
                        return;
                    }

                    task = std::move(tasks_.front());
                    tasks_.pop();
                }

                task();// execute task
            }
        };

      private:
        std::vector<std::thread> workers_;

        std::queue<std::function<void()>> tasks_;

        std::mutex mutex_;

        std::condition_variable cv_;

        bool stop_ = false;
    };

} // namespace tsr::runtime