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

    std::mutex* threadLocks; //lock for using jobs que
    std::condition_variable* jobListener; //listener for waiting for jobs update
    std::queue<std::function<void()> >* jobs; //a queue for each thread
    std::thread* workers; //threads
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
    ThreadPool()
    {
        threadCount = threads?threads : std::thread::hardware_concurrency();
        threadLocks = new std::mutex[threadCount];
        jobListener = new std::condition_variable[threadCount];
        jobs = new std::queue<std::function<void()> >[threadCount];
        workers = new std::thread[threadCount];

        for (uint_fast8_t i = 0; i < threadCount; ++i)
            workers[i] = std::thread([this, i]() {
            std::function<void()> job;

            while (!stopped)
            {
                //grab ownership
                std::unique_lock<std::mutex> jobsAccess{ threadLocks[i] };

                //wait for new jobs
                if (jobs[i].empty() && !stopped)
                {
                    jobListener[i].wait(jobsAccess, [this, i]() { return !jobs[i].empty() || stopped; });
                }

                //set up new job
                job = jobs[i].front();

                //wait while paused
                while (paused && !stopped)
                {
                }

                if (!paused && job)
                {
                    //do work
                    job();
                    //pop job because its done
                    jobs[i].pop();
                }

            }
                });
    }


    void AddTask(std::function<void()> func)
    {
        //finding worker with least jobs
        size_t smallest = -1;
        uint_fast8_t index;
        for (uint_fast8_t i = 0; i < threadCount; ++i)
        {
            std::unique_lock<std::mutex> jobsAccess{ threadLocks[index] };
            if (jobs[i].size()<smallest)
            {
                smallest = jobs[i].size();
                index = i;
            }
        }

        //adding to its job queue
        {
            std::unique_lock<std::mutex> jobsAccess{ threadLocks[index] };
            jobs[index].push(func);
        }
        jobListener[index].notify_all();
    }
    size_t Jobs()
    {
        size_t sum = 0;
        for (uint_fast8_t i = 0; i < threadCount; ++i)
        {
            std::unique_lock<std::mutex> jobsAccess{ threadLocks[i] };
            sum += jobs[i].size();
        }
        return sum;
    }

    void Start()
    {
        paused = false;
    }
    void Pause()
    {
        paused = true;
    }

    ~ThreadPool()
    {
        Stop();
        //closing threads properly 
        for (size_t i = 0; i < threadCount; ++i)
            if (workers[i].joinable()) workers[i].join();
        delete[] threadLocks;
        delete[] jobListener;
        delete[] jobs;
        delete[] workers;
    }
};
