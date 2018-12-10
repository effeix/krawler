#ifndef __KRAWLERS_H__
#define __KRAWLERS_H__

#include "product.hpp"
#include "semaphore.hpp"

#include <chrono>
#include <string>
#include <vector>
#include <mutex>

#include <boost/regex.hpp>

typedef std::chrono::high_resolution_clock Time;

class KrawlerS {
public:
    KrawlerS();
    void producer(
        Semaphore& filled_slots,
        Semaphore& empty_slots,
        std::vector<std::string> urls,
        int first_url,
        int last_url
    );
    void consumer(Semaphore& empty_slots, Semaphore& filled_slots);
    void buffer_put(std::string url);
    std::string buffer_get();
    std::vector<std::string> crawl_par(std::vector<std::string> urls, int n_cons, int n_prod);
    std::vector<std::string> crawl(
        std::vector<std::string> urls, double& total_idle_time);
    std::vector<std::string> get_pages(std::string url);
    std::vector<std::string> product_info(std::string product_url);
    std::string product_category(std::string product_url);
    std::string search(std::string& page_content, std::string& expr);
    std::vector<std::string> search_many(
        std::string& page_content,
        std::string& expr
    );
    Product new_product(
        std::string& link,
        double& product_download
    );

    std::vector<std::string> buffer;
    std::mutex buffer_lock;
    std::mutex stop_lock;
    std::mutex consumed_lock;
    int stop = 0;
    int n_producers;
    int consumed = 0;

    std::string re_last_page;
    std::string re_product_name;
    std::string re_product_description;
    std::string re_product_img_url;
    std::string re_product_price;
    std::string re_product_installment_qty;
    std::string re_product_price_in_installment;
    std::string re_product_category;
    std::string re_product_link;
};

#endif/*__KRAWLERS_H__*/
