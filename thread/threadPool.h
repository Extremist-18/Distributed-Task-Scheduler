#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "../core/tasks.h"
#include "../core/scheduler.h"
#include "../execution/executor.h"
#include "../utils/rateLimiter.h"
#include "../utils/metrics.h"
#include <iostream>
using namespace std;

class ThreadPool{
    vector<thread> workers;
    Scheduler & sch;
    mutex mtx;
    condition_variable cv;
    RateLimiter rateLimiter;
    Metrics &metrics;
    bool stop;
    void worker(){
        while(true){
            Tasks t(0,0 ,TaskType::CPU, Priority::LOW);
            {
                unique_lock<mutex>lock(mtx);
                cv.wait(lock,[this](){
                    return stop || !sch.isEmpty();
                });
                if(stop && sch.isEmpty())   return;
            }

            // if(sch.isEmpty()) continue;
            t = sch.getNextTask();
            if(t.getUserId() == -1) continue; 

            t.initiate_task();
            Executor::execute(t);
            t.end_task();
            metrics .record(t);
        }
    }
    public:
        ThreadPool(int num, Scheduler &s,Metrics &m ,int mx_token,int refill_rate):sch(s),metrics(m),rateLimiter(mx_token,refill_rate),stop(false){
            for (int i=0;i<num;i++) {
                workers.emplace_back(&ThreadPool::worker, this);
            }
        }

        void submit(Tasks t){
            metrics.incSubmitted();
            bool allowed = false;
            int retries = 0;
            int max_retries = 2;

            while(!(allowed = rateLimiter.allow(t.getUserId())) && retries < max_retries){
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                retries++;
            }

            if(!allowed){
                metrics.incDropped();
                return;
            }

            {
                lock_guard<std::mutex> lock(mtx);
                sch.addTask(t);   
            }

            cv.notify_one();
        }

        void shutdown(){
            {
                lock_guard<std::mutex> lock(mtx);
                stop = true;
            }
            cv.notify_all();
            for (auto& t:workers) {
                if (t.joinable())   t.join();
            }
        }

    ~ThreadPool(){
        shutdown();
    }
};