#pragma once 

#include<queue>
#include<vector>
#include<thread>
#include<condition_variable>

#include<utility>
#include<memory>

#include<functional>
#include<future>

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <unistd.h>
#endif

// Get the processor core number, same as the max threads number.
#ifdef _WIN32
SYSTEM_INFO sysInfo;
GetSystemInfo(&sysInfo);
static int core_numb = sysInfo.dwNumberOfProcessors;
#elif __linux__
static int core_numb = sysconf(_SC_NPROCESSORS_ONLN);
#endif

class Threadpool {
private:
    /* data */
    bool pool_stop = false;
    
    std::vector<std::thread> pool;

    std::mutex queue_mtx;
    std::condition_variable queue_cv;
    std::queue<std::function<void()>> tasks;

public:
    Threadpool(int threads_numb = core_numb);
    ~Threadpool();

    template<typename F, typename... Args>
    decltype(auto) enq_task(F&& foo, Args&&... args);
};


template<typename F, typename... Args>
decltype(auto) Threadpool::enq_task(F&& foo, Args&&... args) {
    using ReturnType = typename std::result_of<F(Args...)>::type; 

    auto todo_task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<F>(foo), std::forward<Args>(args)...));
    std::future<ReturnType> res = todo_task->get_future(); 
    {
        std::unique_lock<std::mutex> enq_lock(queue_mtx);
        if(pool_stop) 
            throw std::runtime_error("pool stop:task adding still.");
        // add the task into tasks queue.
        tasks.emplace([todo_task](){
            (*todo_task)();
        }); 
    }
    queue_cv.notify_all();

    return res;
}


