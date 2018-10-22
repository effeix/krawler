#include "krawlerp.hpp"
#include "curlhandler.h"
#include "product.hpp"
#include "semaphore.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/optional/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

typedef std::chrono::high_resolution_clock Time;
using namespace std::chrono;

KrawlerP::KrawlerP(int n_prod, int n_cons) : n_prod(n_prod), n_cons(n_cons) {}

std::string get_product_description_par(std::string content) {
    boost::regex expr(
        "(?<=<p class=\"description__text\"></p>)(.*?)(?=<p class=\"description__text\"></p>)");
    boost::smatch matches;

    if(boost::regex_search(content, matches, expr)) {
        return matches[1];
    }

    return "N/A";
}

std::string get_product_category_par(std::string content) {
    std::vector<std::string> strs;
    boost::split(strs, content, boost::is_any_of("/"));

    return strs[3];
}

std::string unescape_par(std::string s) {
    boost::regex expr("\\\\");
    std::string newtext = "";
    
    return boost::regex_replace(s, expr, newtext);
}

std::string total_pages_par(std::string content) {
    boost::regex expr("\"lastPage\":(\\d+)");
    boost::smatch matches;
    
    if(boost::regex_search(content, matches, expr)) {
        return matches[1];
    }
    
    return 0;
}

std::string total_products_par(std::string content) {
    boost::regex expr("\"size\":(\\d+)");
    boost::smatch matches;
    
    if(boost::regex_search(content, matches, expr)) {
        return matches[1];
    }
    
    return 0;
}

boost::property_tree::ptree get_products_par(std::string content) {
    boost::regex expr("(\"products\":\\[.*?\\])");
    boost::smatch matches;

    boost::property_tree::ptree pt;

    if(boost::regex_search(content, matches, expr)) {
        std::stringstream ss;
        ss << "{" + matches[1] + "}";
        boost::property_tree::read_json(ss, pt);
    }

    return pt;
}

Product create_product_par(const boost::property_tree::ptree::value_type &child, std::string url) {
    std::string name = child.second.get<std::string>("title");
    std::string pic_url = child.second.get<std::string>("imageUrl");

    std::string price_in_installment;
    std::string installment_qty;
    if(child.second.get<std::string>("installment") == "null") {
        price_in_installment = "N/A";
        installment_qty = "N/A";
    }
    else {
        price_in_installment = child.second.get<std::string>("installment.totalValue");
        installment_qty = child.second.get<std::string>("installment.quantity");
    }

    std::string price;
    if(child.second.get<std::string>("bestPrice") == "null") {
        if(price_in_installment == "N/A") {
           price = "N/A"; 
        }
        else {
            price = price_in_installment;
        }
    }
    else {
        price = child.second.get<std::string>("bestPrice.value");
    }

    std::string prod_url = child.second.get<std::string>("url");

    std::string page_product = http_get(unescape_par(prod_url));
    std::string description = get_product_description_par(page_product);
    std::string category = get_product_category_par(url);

    Product p(name,
            description,
            pic_url,
            price,
            price_in_installment,
            installment_qty,
            category,
            prod_url
    );

    return p;
}

void KrawlerP::producer(
        Semaphore &filled_slots,
        Semaphore &empty_slots,
        std::vector<std::string> urls,
        int first_url,
        int last_url) {
    
    std::string payload;
    for(unsigned int i = first_url; i <= last_url; i++) {
        Time::time_point download_time_start = Time::now();
        payload = http_get(urls[i]);
        Time::time_point download_time_end = Time::now();
        time_per_product << duration_cast<duration<double>>(download_time_start - download_time_end).count() << "\n";
        empty_slots.acquire();
        buffer_put(payload);
        filled_slots.release();   
    }
}

void KrawlerP::buffer_put(std::string payload) {
    {
        std::unique_lock<std::mutex> lock(buffer_lock);
            payloads.push_back(payload);
    }
}

void KrawlerP::consumer(Semaphore &filled_slots, Semaphore &empty_slots) {
    while(true) {
        filled_slots.acquire();
        std::string payload = buffer_get();
        empty_slots.release();

        std::vector<Product> product_list;
        boost::property_tree::ptree products = get_products_par(payload);

        boost::optional<boost::property_tree::ptree&> children = products.get_child_optional("products");

        if(!children) {
            std::cout << "Skipping..." << std::endl;
        }
        else {
            int count = 0;
            for(const boost::property_tree::ptree::value_type& child : products.get_child("products")) {
                count++;
                product_list.push_back(create_product_par(child, payload));
            }
            std::cout << std::endl;
        }

        for(Product prod: product_list) {
            prod.display();
        }
    }
}

std::string KrawlerP::buffer_get() {
    {
        std::unique_lock<std::mutex> lockk(buffer_lock);
            std::string payload = payloads.back();
            payloads.pop_back();
            return payload;
    }
}

void KrawlerP::crawl(std::string url) {
    time_per_product.open("time_per_product.txt");

    std::string page_first = http_get(url);
    int n_pages = std::stoi(total_pages_par(page_first));
    std::string n_products = total_products_par(page_first);
    std::string pagination = "?page=";
    std::string page_products;

    for(unsigned int pg = 1; pg <= n_pages; pg++)
        urls.push_back(url + pagination + std::to_string(pg));

    std::vector<std::thread> p_thr;
    std::vector<std::thread> c_thr;

    Semaphore filled_slots(0);
    Semaphore empty_slots(10);

    int first_url = 1;
    int last_url;
    int chunk_size = n_pages / n_prod;

    // Starts at 1 because pagination starts at 1
    for(unsigned int p = 1; p <= n_prod; p++) {
        last_url = p == n_prod ? n_pages : p * chunk_size;

        p_thr.push_back(
            std::thread(
                &KrawlerP::producer,
                this,
                std::ref(filled_slots),
                std::ref(empty_slots),
                std::ref(urls),
                first_url,
                last_url
            )
        );

        first_url = last_url + 1;
    }

    for(unsigned int c = 0; c < n_cons; c++) {
        c_thr.push_back(
            std::thread(
                &KrawlerP::consumer,
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

    time_per_product.close();
}
