#pragma once
#include <vector>
#include <mutex>
#include <atomic>
#include "../core/tasks.h"
#include <iostream>
#include <algorithm>

using namespace std;

class Metrics{
    vector<long long> h,m,l,p99;
    mutex mtx;
    atomic<int> dropped{0}, submitted{0}, processed{0};

public:
    void incSubmitted(){ submitted++; }
    void incDropped(){ dropped++; }

    void record(const Tasks &t){
        {
            lock_guard<mutex> lock(mtx);
            
            p99.push_back(t.getLatency());
            if(t.get_Priority() == Priority::HIGH)
                h.push_back(t.getLatency());
            else if(t.get_Priority() == Priority::MEDIUM)
                m.push_back(t.getLatency());
            else
                l.push_back(t.getLatency());
        }
        processed++; 
    }

    int getProcessed(){ return processed.load(); }
    int getDropped(){ return dropped.load(); }

    double dropRate(){
        if(submitted == 0) return 0;
        return (double)dropped*100/submitted;
    }

    double calcAvg(vector<long long>&v){
        if(v.empty()) return 0;
        long long sum = 0;
        for(auto i:v) sum += i;
        return (double)sum/v.size();
    }

    long long calcMax(vector<long long>&v){
        long long mx=0;
        for(auto i:v) if(i>mx) mx=i;
        return mx;
    }

    long long getP99(){
        sort(p99.begin(),p99.end());
        long long N = p99.size();
        int idx = (99*N+1)/100 -1;
        return p99[idx];
    }
    long long getP95(){
        sort(p99.begin(),p99.end());
        long long N = p99.size();
        int idx = (95*N+1)/100 -1;
        return p99[idx];
    }
    void print(){
        cout << "Submitted: " << submitted << "\n";
        cout << "Dropped: " << dropped << "\n";
        cout << "Processed: " << processed << "\n";
        cout << "Drop %: " << dropRate() << "%\n";
        cout << "-----------------------------\n";

        cout << "HIGH Priority:\n";
        cout << "Count: " << h.size() << "\n";
        cout << "Avg Latency: " << calcAvg(h) << " us\n";
        cout << "Max Delay: " << calcMax(h) << " us\n\n";

        cout << "MEDIUM Priority:\n";
        cout << "Count: " << m.size() << "\n";
        cout << "Avg Latency: " << calcAvg(m) << " us\n";
        cout << "Max Delay: " << calcMax(m) << " us\n\n";

        cout << "LOW Priority:\n";
        cout << "Count: " << l.size() << "\n";
        cout << "Avg Latency: " << calcAvg(l) << " us\n";
        cout << "Max Delay: " << calcMax(l) << " us\n";


        cout<<"p99 latency: "<<getP99()<<" us \n";
        cout<<"p95 latency: "<<getP95()<<" us \n";

    }
};