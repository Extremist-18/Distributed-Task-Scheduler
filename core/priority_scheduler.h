#pragma once
#include "scheduler.h"
#include <queue>
#include <mutex>

class PriorityScheduler: public Scheduler{
    queue<Tasks> high, medium, low;
    mutex mtx;
    public:
        void addTask(Tasks task) override{
            lock_guard<mutex> lock(mtx);
            if(task.get_Priority()== Priority::HIGH)    high.push(task);
            else if(task.get_Priority()==Priority::MEDIUM)  medium.push(task);
            else low.push(task); 
        }

        Tasks getNextTask() override{
            lock_guard<mutex> lock(mtx);
            if(!high.empty()){
                Tasks t = high.front(); high.pop();
                return t;
            }   
            if(!medium.empty()){
                Tasks t=medium.front(); medium.pop();
                return t;
            }
            if(!low.empty()){
                Tasks t =low.front(); low.pop();
                return t;
            }
            return Tasks(-1, -1, TaskType::CPU, Priority::LOW);
        }

        bool isEmpty() override{
            lock_guard<mutex> lock(mtx);
            return (high.empty() && medium.empty() && low.empty());
        }
};
