#ifndef TIMEUTILS_H
#define TIMEUTILS_H
#include <functional>
#include <thread>
#include <set>
#include <stdexcept>
#include <mutex>

using namespace std::chrono_literals;
class TimeUtils {
public:
    template <typename _Function, class Rep, class Period, typename ..._Types>
    static int setTimeout(_Function &&callback, const std::chrono::duration<Rep, Period>& timeout, _Types ...args);
    static void clearTimeout(const int &timeoutId);

    template <typename _Function, class Rep, class Period, typename ..._Types>
    static int setInterval(_Function &&callback, const std::chrono::duration<Rep, Period>& timeout, _Types ...args);
    static void clearInterval(const int &intervalId);

private:
    static std::set<size_t> s_timeout_functions;
    static std::set<size_t> s_interval_functions;
    static size_t timeout_counter;
    static size_t interval_counter;
    static std::mutex s_timeout_mutex;
    static std::mutex s_interval_mutex;

private:
    explicit TimeUtils() = delete;
    explicit TimeUtils(const TimeUtils&) = delete;
    virtual ~TimeUtils() = delete;
};

template <typename _Function, class Rep, class Period, typename ..._Types>
int TimeUtils::setTimeout(_Function &&callback, const std::chrono::duration<Rep, Period>& timeout, _Types ...args) {
    if constexpr (!std::is_function_v<_Function>) { static_assert("not a function"); }
    s_timeout_mutex.lock();
    s_timeout_functions.insert(++timeout_counter);
    s_timeout_mutex.unlock();
    std::thread thread {[&, counter = timeout_counter]() {
        std::this_thread::sleep_for(timeout);
        if (s_timeout_functions.contains(counter)) {
            callback(args...);
            s_timeout_mutex.lock();
            if (s_timeout_functions.contains(counter)) {
                s_timeout_functions.erase(counter);
            }
            s_timeout_mutex.unlock();
        }
    }};
    thread.detach();
    return timeout_counter;
}

template <typename _Function, class Rep, class Period, typename ..._Types>
int TimeUtils::setInterval(_Function &&callback, const std::chrono::duration<Rep, Period>& timeout, _Types ...args) {
    s_interval_mutex.lock();
    s_interval_functions.insert(++interval_counter);
    s_interval_mutex.unlock();
    std::thread thread {[&, counter = interval_counter ]() {
        do {
            std::this_thread::sleep_for(timeout);
            if (!s_interval_functions.contains(counter)) { break; }
            callback(args...);
        } while (true);
    }};
    thread.detach();
    return interval_counter;
}

#endif // TIMEUTILS_H
