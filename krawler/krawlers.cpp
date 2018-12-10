#include "krawlers.hpp"

#include "curlhandler.h"
#include "product.hpp"
#include "semaphore.hpp"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/regex.hpp>

using namespace std::chrono;

KrawlerS::KrawlerS() {
    re_last_page = "(?<=\"lastPage\":)(\\d+)";
    re_product_name = "(?<=\"fullTitle\":\\s)(.*?)(?=\",)";
    re_product_description = "(?<=<p class=\"description__text\"></p>)(.*?)(?=<p class=\"description__text\"></p>)";
    re_product_img_url = "(?<=showcase-product__big-img js-showcase-big-img\"\\ssrc=\")(.*?)(?=\" item)";
    re_product_price = "(?<=\"priceTemplate\":\\s\")(.*?)(?=\",)";
    re_product_installment_qty = "(?<=\"installmentQuantity\":\\s\")(.*?)(?=\",)";
    re_product_price_in_installment = "(?<=\"priceTemplate\":\\s\")(.*?)(?=\",)";
    re_product_category = "";
    re_product_link = "(?<=linkToProduct\"\\shref=\")(.*?)(?=\")";
}

/* Search single occurrence of a regex */
std::string KrawlerS::search(std::string& page_content, std::string& expr) {
    boost::regex expression(expr);
    boost::smatch matches;

    if(boost::regex_search(page_content, matches, expression)) {
        return matches[1];
    }

    return "N/A";
}

/* Search many occurrences of a regex */
std::vector<std::string> KrawlerS::search_many(std::string& page_content,
        std::string& expr) {
    boost::regex expression(expr);

    std::vector<std::string> all_matches;

    boost::sregex_token_iterator iter(
        page_content.begin(),
        page_content.end(),
        expression,
        0
    );
    boost::sregex_token_iterator end;

    for(; iter != end; ++iter)
        all_matches.push_back(*iter);

    return all_matches;
}

/* Discover product category from main URL */
std::string KrawlerS::product_category(std::string product_url) {
    std::vector<std::string> strs;
    boost::split(strs, product_url, boost::is_any_of("/"));

    return strs[3];
}

/* Get all pages that contain products to be crawled */
std::vector<std::string> KrawlerS::get_pages(std::string url) {
    std::string first_page = http_get(url);
    std::string n_pages = search(first_page, re_last_page);
    std::string pagination = "?page=";

    std::vector<std::string> pages;

    for(int i = 1; i <= std::stoi(n_pages); i++) {
        pages.push_back(
            url + pagination + std::to_string(i)
        );
    }

    return pages;
}

/* Access a product URL and get product information */
Product KrawlerS::new_product(std::string& link, double& download_time) {

    Time::time_point t0, t1, t2, t3;
    double elapsed_analysis;

    t0 = Time::now();

        t1 = Time::now();
            std::string product_page = http_get(link);
        t2 = Time::now();

        std::string name = search(product_page, re_product_name);
        std::string description = search(product_page, re_product_description);
        std::string pic_url = search(product_page, re_product_img_url);
        std::string price = search(product_page, re_product_price);
        std::string installment_qty = search(product_page,
            re_product_installment_qty);
        std::string price_in_installment = search(product_page,
            re_product_price_in_installment);
    
    t3 = Time::now();

    download_time = duration_cast<duration<double>>(t2 - t1).count();

    elapsed_analysis = duration_cast<duration<double>>(t3 - t0).count();
    std::cerr << elapsed_analysis << std::endl;

    return Product(
        name,
        description,
        pic_url,
        price,
        price_in_installment,
        installment_qty,
        "",
        link
    );
}

std::vector<std::string> KrawlerS::crawl(
    std::vector<std::string> urls,
    double& process_idle_time) {

    std::vector<std::string> all_products;

    std::string category = product_category(urls[0]);

    Time::time_point t0, t1;
    double download_time;

    for(unsigned int i = 0; i < urls.size(); i++) {
        t0 = Time::now();
            std::string products_page = http_get(urls[i]);
        t1 = Time::now();
        process_idle_time += duration_cast<duration<double>>(t1 - t0).count();

        std::vector<std::string> product_links = search_many(
            products_page,
            re_product_link
        );

        for(std::string& link: product_links) {
            Product p = new_product(link, download_time);
            p.category = category;
            all_products.push_back(p.display());

            process_idle_time += download_time;
        }
    }

    return all_products;
}

std::vector<std::string> KrawlerS::crawl_par(std::vector<std::string> urls,
        int n_prod, int n_cons) {
    
    buffer.reserve(1000);

    std::vector<std::thread> p_thr;
    std::vector<std::thread> c_thr;

    Semaphore filled_slots(0);
    Semaphore empty_slots(1000);

    int first_url = 0;
    int last_url;
    int chunk_size = urls.size() / n_prod;
    stop = 0;
    n_producers = n_prod;

    for(unsigned int p = 1; p <= n_prod; p++) {
        last_url = p == n_prod ? urls.size() : p * chunk_size;

        p_thr.push_back(
            std::thread(
                &KrawlerS::producer,
                this,
                std::ref(filled_slots),
                std::ref(empty_slots),
                std::ref(urls),
                first_url,
                last_url
            )
        );

        first_url = last_url;
    }

    for(unsigned int p = 1; p <= n_prod; p++) {
        p_thr.push_back(
            std::thread(
                &KrawlerS::consumer,
                this,
                std::ref(filled_slots),
                std::ref(empty_slots)
            )
        );
    }

    for(std::thread &pt: p_thr)
        pt.join();

    for(std::thread &ct: c_thr)
        ct.join();

    std::cout << "CONSUMED: " << consumed << std::endl;

    std::cout << "BUFFER (" << buffer.size() << ")" << '\n' << std::flush;
    for(std::string& url : buffer) {
        std::cout << url << std::endl << std::flush;
    }
}

std::string KrawlerS::buffer_get() {
    {
        std::unique_lock<std::mutex> lockk(buffer_lock);
            std::string url = buffer.back();
            buffer.pop_back();
            return url;
    }
}

void KrawlerS::buffer_put(std::string url) {
    {
        std::unique_lock<std::mutex> lock(buffer_lock);
            buffer.push_back(url);
    }
}

void KrawlerS::producer(
        Semaphore& filled_slots,
        Semaphore& empty_slots,
        std::vector<std::string> urls,
        int first_url,
        int last_url) {

    /* Producer thread will receive a list of URLs containing products.
       For each URL, the page is accessed, all product links are collected
       and loaded into the buffer.
     */

    std::string products_page;
    std::thread::id this_id = std::this_thread::get_id();

    for(int u = first_url; u < last_url; u++) {
        products_page = http_get(urls[u]);

        std::vector<std::string> product_urls = search_many(
            products_page,
            re_product_link
        );

        for(std::string& url : product_urls) {
            empty_slots.acquire();
            buffer_put(url);
            filled_slots.release();
        }
    }

    stop++;
}

void KrawlerS::consumer(Semaphore& empty_slots, Semaphore& filled_slots) {
    
    double download_time;

    while(true) {
        if(stop == n_producers && buffer.size() == 0) {
            filled_slots.release();
            break;
        }

        if(!buffer.empty()) {
            filled_slots.acquire();
            std::string url = buffer_get(); 
            empty_slots.release();

            Product p = new_product(url, download_time);

            std::cout << p.display();
        }

    }
}
