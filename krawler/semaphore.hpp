#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <condition_variable>
#include <mutex>

class Semaphore {
public:
    Semaphore(int count);
    void acquire();
    void release();

private:
    int count;
    std::condition_variable cv;
    std::mutex mutex;
};

#endif/*__SEMAPHORE_H__*/

