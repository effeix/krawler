#ifndef __KRAWLERP_H__
#define __KRAWLERP_H__

#include "semaphore.hpp"
#include <fstream>
#include <mutex>
#include <string>
#include <vector>

class KrawlerP {
public:
    KrawlerP(int n_prod, int n_cons);
    void crawl(std::string url);
    void producer(Semaphore &filled_slots, Semaphore &empty_slots, std::vector<std::string> urls, int first_url, int last_url);
    void consumer(Semaphore &filled_slots, Semaphore &empty_slots);
    void buffer_put(std::string payload);
    std::string buffer_get();

    int n_prod, n_cons;
    std::vector<std::string> urls;
    std::vector<std::string> payloads;
    std::mutex buffer_lock;
    std::ofstream time_per_product;
};

#endif/*__KRAWLERP_H__*/
