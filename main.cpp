#include <iostream>
#include <thread>
#include <chrono>
#include "core/priority_scheduler.h"
#include "thread/threadPool.h"
#include "utils/metrics.h"
#include "tests/load_generator.h"

int main(){
    int num_threads, num_wokers, mx_token, refill_rate;
    num_threads = 8;
    num_wokers = 50000;
    mx_token = 1000;
    refill_rate = 200;
    PriorityScheduler scheduler;
    Metrics metrics;
    ThreadPool pool(num_threads,scheduler, metrics, mx_token, refill_rate);

    cout<<"\n\n";
    cout<<"Total Requests to be served: "<<num_wokers<<'\n';
    cout<<"Max token: "<<mx_token<<" and Refill Rate: "<<refill_rate<<'\n';
    cout<<"Starting Load...\n";
    LoadGenerator::generator(pool,num_wokers);

    while(metrics.getProcessed() + metrics.getDropped() < num_wokers){
        this_thread::sleep_for(chrono::milliseconds(5));
    }
    pool.shutdown();
    metrics.print();
    return 0;
}

