#pragma once 

#include<queue>
#include<vector>
#include<functional>
#include<thread>
#include<condition_variable>

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

    void enq_task(std::function<void()>);



};




