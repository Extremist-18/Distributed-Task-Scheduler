#pragma once
#include <chrono>
using namespace std;

enum class TaskType {CPU,IO,MIXED};
enum class Priority {HIGH,MEDIUM,LOW};

class Tasks{
    int id,usedId;
    TaskType type;
    Priority p;
    chrono::high_resolution_clock::time_point enqueue_time,start_time,end_time;
    public:
    Tasks(int id, int userId, TaskType type, Priority p):id(id),usedId(userId),type(type),p(p),enqueue_time(std::chrono::high_resolution_clock::now()){}

    void initiate_task(){
        start_time = chrono::high_resolution_clock::now();
    }
    void end_task(){
        end_time =chrono::high_resolution_clock::now();
    }
    long long getLatency() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(end_time - enqueue_time).count();
    }
    int getUserId() const{
        return usedId;
    }
    Priority get_Priority() const{
        return p;
    }
    TaskType get_TaskType() const{
        return type;
    }
};