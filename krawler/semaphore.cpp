#include "semaphore.hpp"

Semaphore::Semaphore(int count) : count(count) {}

void Semaphore::acquire() {
    /*
     * Decrement counter if there are "slots" available for threads to use,
     * else wait for a slot to be available (count > 0)
     */

    // Critical region
    {
        std::unique_lock<std::mutex> lock(mutex);

        /* If semaphore filled all slots (count == 0), wait.
         * Resume only if (count > 0)
         */
        cv.wait(lock, [&]{return count > 0;});
        /*             |
         *             -> make all variables in current scope available to the wait() method
         *                by reference
         */

        // Else, decrement slots available
        count--;
    }
}

void Semaphore::release() {
    /*
     * Increment counter and notify one thread that it can continue execution
     */

    // Critical region
    {
        std::unique_lock<std::mutex> lock(mutex);

        // Now, Semaphore has at least one slot available
        count++;
    }
    /* Tell one thread to continue.
     * Since wait() does not have effect when count <= 0,
     * minimum count is 0 and every increment frees a slot for one thread to continue.
     */
    cv.notify_one();

}


