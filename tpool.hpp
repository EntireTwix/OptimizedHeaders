#pragma once
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <functional>

template <uint_fast8_t threads = 0>
class ThreadPool final
{
private:
    bool stopped = false, paused = true;

    std::mutex *threadLocks;                 //lock for using jobs que
    std::condition_variable *jobListener;    //listener for waiting for jobs update
    std::queue<std::function<void()>> *jobs; //a queue for each thread
    std::thread *workers;                    //threads
    uint_fast8_t threadCount;

    void Stop()
    {
        stopped = true;
        for (uint_fast8_t i = 0; i < threadCount; ++i)
        {
            jobListener[i].notify_one();
        }
    }

public:
    ThreadPool() noexcept
    {
        threadCount = threads + (!threads * std::thread::hardware_concurrency()); //branchless turnary
        threadLocks = new std::mutex[threadCount];
        jobListener = new std::condition_variable[threadCount];
        jobs = new std::queue<std::function<void()>>[threadCount];
        workers = new std::thread[threadCount];

        for (uint_fast8_t i = 0; i < threadCount; ++i)
            workers[i] = std::thread([this, i]() {
                std::function<void()> job;
                while (!stopped)
                {
                    {
                        std::unique_lock<std::mutex> jobsAccess{threadLocks[i]}; //grab ownership
                        if (jobs[i].empty() && !stopped)                         //wait for new jobs
                        {
                            jobListener[i].wait(jobsAccess, [this, i]() { return !jobs[i].empty() || stopped; });
                        }
                        job = jobs[i].front(); //set up new job
                    }

                    //wait while paused
                    while (paused && !stopped)
                        ;

                    {
                        std::unique_lock<std::mutex> jobsAccess{threadLocks[i]};
                        if (!paused && job)
                        {
                            job();         //do work
                            jobs[i].pop(); //pop job because its done
                        }
                    }
                }
            });
    }

    void AddTask(std::function<void()> &&func) noexcept
    {
        //finding worker with least jobs
        size_t smallest = -1;
        uint_fast8_t index;
        for (uint_fast8_t i = 0; i < threadCount; ++i)
        {
            std::unique_lock<std::mutex> jobsAccess{threadLocks[i]};
            if (jobs[i].size() < smallest)
            {
                smallest = jobs[i].size();
                index = i;
                if (!smallest) //auto-assign if smallest is 0
                {
                    break;
                }
            }
        }

        //adding to its job queue
        {
            std::unique_lock<std::mutex> jobsAccess{threadLocks[index]};
            jobs[index].push(func);
        }
        jobListener[index].notify_all();
    }
    size_t Jobs() const noexcept
    {
        size_t sum = 0;
        for (uint_fast8_t i = 0; i < threadCount; ++i)
        {
            std::unique_lock<std::mutex> jobsAccess{threadLocks[i]};
            sum += jobs[i].size();
        }
        return sum;
    }
    bool Working() const noexcept
    {
        for (uint_fast8_t i = 0; i < threadCount; ++i)
        {
            std::unique_lock<std::mutex> jobsAccess{threadLocks[i]};
            if (jobs[i].size())
            {
                return true;
            }
        }
        return false;
    }

    int Workers() const noexcept
    {
        return threadCount;
    }

    //this function should be called when you want to use the thread pool
    void Start() noexcept
    {
        paused = false;
    }

    //this function should be called when you're done using the thread pool
    void Pause() noexcept
    {
        paused = true;
    }

    ~ThreadPool()
    {
        Stop();
        //closing threads properly
        for (size_t i = 0; i < threadCount; ++i)
        {
            if (workers[i].joinable())
                workers[i].join();
        }
        delete[] threadLocks;
        delete[] jobListener;
        delete[] jobs;
        delete[] workers;
    }
};

template <typename ForwardIt, typename UnaryFunction, uint_fast8_t threads>
void asyncfor_each(ForwardIt first, ForwardIt last, UnaryFunction &&f, ThreadPool<threads> &engine)
{
    size_t step_sz = (last - first) / engine.Workers();
    step_sz += (bool)((last - first) % engine.Workers()); //branchless correction for remainder in step size
    if (!step_sz)
    {
        for (ForwardIt i = first; i < last; ++i)
        {
            engine.AddTask([i, &f]() {
                f(*i);
            });
        }
    }
    else
    {
        for (ForwardIt i = first; i < last; i += step_sz)
        {
            engine.AddTask([i, &step_sz, &f]() {
                std::for_each(i, i + step_sz, f);
            });
        }
    }
    engine.Start();
    while (engine.Working())
        ;
    engine.Pause();
}
