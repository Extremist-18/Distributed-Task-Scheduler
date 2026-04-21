#pragma once
using namespace std;
#include "../core/tasks.h"
#include <thread>

class Executor{
public:
    static void execute(Tasks& t) {
        switch (t.get_TaskType()){
            case TaskType::CPU:
                std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
                break;
            case TaskType::IO:
                std::this_thread::sleep_for(std::chrono::milliseconds(3));  
                break;
            case TaskType::MIXED:
                std::this_thread::sleep_for(std::chrono::milliseconds(2));  
                break;
        }
    }
};