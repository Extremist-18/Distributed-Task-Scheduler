#pragma once
#include <unordered_map>
#include <chrono>
#include <mutex>
using namespace std;

// using token bucket Algorithm
class RateLimiter {
    int mx_token;
    double refill_rate_per_us;  
    std::unordered_map<int, double> tokens;
    std::unordered_map<int, std::chrono::steady_clock::time_point> last_refill;
    std::mutex mtx;

public:
    RateLimiter(int limit, int refill_per_sec):mx_token(limit), refill_rate_per_us(refill_per_sec / 1'000'000.0) {}

    bool allow(int userId) {
        std::lock_guard<std::mutex> lock(mtx);
        auto now = std::chrono::steady_clock::now();
        if (tokens.find(userId) == tokens.end()) {
            tokens[userId] = mx_token;
            last_refill[userId] = now;
        }
        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(now - last_refill[userId]).count();

        tokens[userId] = std::min<double>(mx_token,tokens[userId] + elapsed_us * refill_rate_per_us);
        last_refill[userId] = now;
        if (tokens[userId] >= 1.0) {
            tokens[userId] -= 1.0;
            return true;
        }
        return false;
    }
};