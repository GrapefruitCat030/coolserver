#include "Threadpool.h"
#include <assert.h>

#define JUMPOUT_CONDITION(X) [this](){return (X);}

Threadpool::Threadpool(int threads_numb) {
    for(int i = 0; i < threads_numb; i++) {
        pool.emplace_back(std::thread([this](){
            while(true) { 
                std::function<void()> temp_task;
                {
                    // this lock only control task retaining. 
                    std::unique_lock<std::mutex> q_lock(queue_mtx);
                    queue_cv.wait(q_lock, JUMPOUT_CONDITION(pool_stop || !tasks.empty()));
                    if(pool_stop) {
                        if(tasks.empty()) return;
                        else assert(0);    
                    }  

                    // extract the task from tasks queue.
                    temp_task = tasks.front();
                    tasks.pop();
                }
                temp_task();
            }
        }));
    }
}

Threadpool::~Threadpool() {

    {
        std::unique_lock<std::mutex> temp_lock(queue_mtx);
        pool_stop = true;
    }
    queue_cv.notify_all();
    for(auto &t: pool) {
        if(t.joinable())
            t.join();
    }


}


void Threadpool::enq_task(std::function<void()> todo_task) {
    {
        std::unique_lock<std::mutex> enq_lock(queue_mtx);
        if(pool_stop) 
            assert(0);
        // add the task into tasks queue.
        tasks.emplace(todo_task); 
        
        queue_cv.notify_all();
    }
}
