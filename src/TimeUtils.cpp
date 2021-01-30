#include "TimeUtils.h"

std::set<size_t> TimeUtils::s_timeout_functions;
std::set<size_t> TimeUtils::s_interval_functions;
size_t TimeUtils::timeout_counter = 0;
size_t TimeUtils::interval_counter = 0;
std::mutex TimeUtils::s_timeout_mutex;
std::mutex TimeUtils::s_interval_mutex;

void TimeUtils::clearTimeout(const int &timeoutId) {
    if (!s_timeout_functions.contains(timeoutId)) {
        throw std::runtime_error("timeoutId not exists");
    }
    s_timeout_mutex.lock();
    if (s_timeout_functions.contains(timeoutId)) {
        s_timeout_functions.erase(timeoutId);
    } else {
        throw std::runtime_error("timeoutId not exists");
    }
    s_timeout_mutex.unlock();
}

void TimeUtils::clearInterval(const int &intervalId) {
    if (!s_interval_functions.contains(intervalId)) {
        throw std::runtime_error("intervalId not exists");
    }
    s_interval_mutex.lock();
    s_interval_functions.erase(intervalId);
    s_interval_mutex.unlock();
}
