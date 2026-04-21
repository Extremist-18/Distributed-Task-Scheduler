#pragma once
using namespace std;
#include "tasks.h"

class Scheduler{
    public:
        virtual void addTask(Tasks task)=0;
        virtual Tasks getNextTask() = 0;
        virtual bool isEmpty() = 0;
        virtual ~Scheduler() {}  
};