#include "curlhandler.h"
#include "krawlers.hpp"
#include "product.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/optional/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <sstream>
#include <string>

std::string get_product_description(std::string content) {
    boost::regex expr(
        "(?<=<p class=\"description__text\"></p>)(.*?)(?=<p class=\"description__text\"></p>)");
    boost::smatch matches;

    if(boost::regex_search(content, matches, expr)) {
        return matches[1];
    }

    return "N/A";
}

std::string get_product_category(std::string content) {
    std::vector<std::string> strs;
    boost::split(strs, content, boost::is_any_of("/"));

    return strs[3];
}

void print_json(boost::property_tree::ptree & pt) {
    std::ostringstream output;
    boost::property_tree::write_json(output, pt);
    std::cout << output.str() << std::endl;
}

std::string unescape(std::string s) {
    boost::regex expr("\\\\");
    std::string newtext = "";
    
    return boost::regex_replace(s, expr, newtext);
}

std::string total_pages(std::string content) {
    boost::regex expr("\"lastPage\":(\\d+)");
    boost::smatch matches;
    
    if(boost::regex_search(content, matches, expr)) {
        return matches[1];
    }
    
    return 0;
}

std::string total_products(std::string content) {
    boost::regex expr("\"size\":(\\d+)");
    boost::smatch matches;
    
    if(boost::regex_search(content, matches, expr)) {
        return matches[1];
    }
    
    return 0;
}

boost::property_tree::ptree get_products(std::string content) {
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

Product create_product(const boost::property_tree::ptree::value_type &child, std::string url) {
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

    std::string page_product = http_get(unescape(prod_url));
    std::string description = get_product_description(page_product);
    std::string category = get_product_category(url);

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

KrawlerS::KrawlerS(){}


void KrawlerS::crawl(std::string url) {
    std::string page_first = http_get(url);
    std::string n_pages = total_pages(page_first);
    std::string n_products = total_products(page_first);
    std::string pagination = "?page=";
    std::string page_products;

    for(unsigned int i = 1; i < std::stoi(n_pages) + 1; i++) {
        if(i == 1) {
            page_products = page_first;
        }
        else {
            page_products = http_get(url + pagination + std::to_string(i));
        }

        std::cout << "Page " << i << " --------------------------" << std::endl;

        std::vector<Product> product_list;
        boost::property_tree::ptree products = get_products(page_products);

        boost::optional< boost::property_tree::ptree& > children = products.get_child_optional("products");

        if(!children) {
            std::cout << "Skipping..." << std::endl << std::endl;
        }
        else {
            int count = 0;
            for(const boost::property_tree::ptree::value_type& child : products.get_child("products")) {
                count++;
                std::cout << "\rPage " << i << " - Product " << count << "/" << n_products << std::flush;

                product_list.push_back(create_product(child, url));
            }
            std::cout << std::endl;
        }

        for(Product prod: product_list) {
            prod.display();
        }
    }
}
