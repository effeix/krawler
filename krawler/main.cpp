#include "curlhandler.h"
#include "product.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>


typedef struct envvars_t {
    std::string URL;
//     int N_CONS_THREADS;
//     int N_PROD_THREADS;
} envvars;

void get_env(envvars* env) {
//     const char * ENV_cons = std::getenv("N_CONS_THREADS");
//     const char * ENV_prod = std::getenv("N_PROD_THREADS");
    const char * ENV_url = std::getenv("URL");

//     env->N_CONS_THREADS = ENV_cons == NULL ? 2 : std::stoi(ENV_cons);
//     env->N_PROD_THREADS = ENV_prod == NULL ? 2 : std::stoi(ENV_prod);
    
    if(ENV_url == NULL) {
        throw std::invalid_argument("URL undefined. Exiting...");
    }
    else {
        env->URL = ENV_url;
    }
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

boost::property_tree::ptree get_product(boost::property_tree::ptree all_products) {
    
}

std::string get_product_description(std::string content) {
    boost::regex expr(
        "(?<=<p class=\"description__text\"></p>)(.*?)(?=<p class=\"description__text\"></p>)");
    boost::smatch matches;
    
    if(boost::regex_search(content, matches, expr)) {
        return matches[1];
    }
    
    return "";
}

std::string get_product_category(std::string content) {
    std::vector<std::string> strs;
    boost::split(strs, content, boost::is_any_of("/"));
    
    return strs[3];
}

int main(int argc, char **argv) {
    envvars * env = new envvars;

    try {
        get_env(env);
    }
    catch(const std::invalid_argument & e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::string first_page = http_get(env->URL);
    
    std::string page_products;
    page_products = http_get(env->URL);

    std::string total = total_pages(page_products);
    std::string pagination = "?page=";
    std::vector<Product> product_list;
    boost::property_tree::ptree products = get_products(page_products);
    for(const boost::property_tree::ptree::value_type& child : 
                  products.get_child("products")) {
        
        std::string listPrice = child.second.get<std::string>("listPrice");
        
        if(listPrice == "null") {
            continue;
        }
        
        std::string name = child.second.get<std::string>("title");
        std::string pic_url = child.second.get<std::string>("imageUrl");
        std::string priceInstallment = child.second.get<std::string>("installment.totalValue");
        std::string priceInstallmentQty = child.second.get<std::string>("installment.quantity");
        std::string price;
        if(child.second.get<std::string>("bestPrice") == "null") {
            price = priceInstallment;
        }
        else {
            price = child.second.get<std::string>("bestPrice.value");
        }

        std::string prod_url = child.second.get<std::string>("url");

        std::string page_product = http_get(unescape(prod_url));
        std::string description = get_product_description(page_product);
        std::string category = get_product_category(env->URL);
        
        product_list.push_back(
            Product(name,
                    description,
                    pic_url,
                    price,
                    priceInstallment,
                    priceInstallmentQty,
                    category,
                    prod_url)
        );
    }
    
    for(Product prod: product_list) {
        std::cout << "{" << std::endl;
        std::cout << "    " << "\"nome\":" << " \"" + prod.name + "\"," << std::endl;
        std::cout << "    " << "\"descricao\":" << " \"" + prod.description + "\"," << std::endl;
        std::cout << "    " << "\"foto\":" << " \"" + prod.pic_url + "\"," << std::endl;
        std::cout << "    " << "\"preco\":" << " " + prod.price + "," << std::endl;
        std::cout << "    " << "\"preco_parcelado\":" << " " + prod.priceInstallment + "," << std::endl;
        std::cout << "    " << "\"preco_num_parcelas\":" << " " + prod.priceInstallmentQty + "," << std::endl;
        std::cout << "    " << "\"categoria\":" << " \"" + prod.category + "\"," << std::endl;
        std::cout << "    " << "\"url\":" << " \"" + prod.prod_url + "\"," << std::endl;
        std::cout << "}";
        std::cout << std::endl << std::endl;
    }


    return EXIT_SUCCESS;
}
