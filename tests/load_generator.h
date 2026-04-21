#pragma once
#include "../core/tasks.h"
#include "../thread/threadPool.h"
#include <random>
using namespace std;

class LoadGenerator{
    public:
        static void generator(ThreadPool &th, int num){
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> priority_dis(0,2);
            uniform_int_distribution<> type_dis(0,2);

            for(int i=0;i<num;i++){
                Priority p = static_cast<Priority>(priority_dis(gen));
                TaskType t = static_cast<TaskType>(type_dis(gen));
                
                int userId = i % 7;  
                Tasks task(i,userId,t,p);
                th.submit(task);
                std::this_thread::sleep_for(std::chrono::microseconds(250));
            }
        }
};