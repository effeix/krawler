#ifndef __KRAWLER_H__
#define __KRAWLER_H__

#include <chrono>
#include <string>
#include <vector>
#include <mutex>

#include <boost/regex.hpp>
#include "product.hpp"
#include "semaphore.hpp"


typedef std::chrono::high_resolution_clock Time;

class Krawler {
public:
    Krawler();

    /* Method executed by the producer thread */
    void producer(
        Semaphore& filled_slots,
        Semaphore& empty_slots,
        std::vector<std::string> urls,
        int first_url,
        int last_url
    );

    /* Method executed by the consumer thread */
    void consumer(
        Semaphore& empty_slots,
        Semaphore& filled_slots
    );

    /* Thread-safe buffer interaction */
    void buffer_put(std::string url);
    std::string buffer_get();

    /* Crawling method used in parallel mode */
    void crawl_par(
        std::vector<std::string> urls,
        int n_prod,
        int n_cons
    );

    /* Crawling method used in sequential mode */
    std::vector<std::string> crawl(
        std::vector<std::string> urls,
        double& total_idle_time
    );

    /* Retrieve pages to be crawled */
    std::vector<std::string> get_pages(std::string url);

    /* Retrieve product category based on URL */
    std::string product_category(std::string product_url);

    /* Search for single regex match */
    std::string search(
        std::string& page_content,
        std::string& expr
    );

    /* Search for multiple regex matches */
    std::vector<std::string> search_many(
        std::string& page_content,
        std::string& expr
    );

    /* Access product URL and create new Product */
    Product new_product(
        std::string& link,
        double& product_download
    );

    /* Used by producer-consumer implementation */
    std::vector<std::string> buffer;
    std::mutex buffer_lock;
    int stop = 0;
    int n_producers;

    /* Timing */
    double total_idle_time;
    double total_execution_time;
    double prod_count;
    std::mutex lock_idle_time;
    std::mutex lock_prod_count;

    /* Regexes for product info */
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

#endif/*__KRAWLER_H__*/
