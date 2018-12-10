#include "main.hpp"
#include "krawlerp.hpp"
#include "krawlers.hpp"
#include <iostream>
#include <stdexcept>


void get_env(envvars* env) {
    const char * ENV_cons = std::getenv("N_CONS");
    const char * ENV_prod = std::getenv("N_PROD");
    const char * ENV_url = std::getenv("URL");
    const char * ENV_mode = std::getenv("MODE");

    env->N_CONS = ENV_cons == NULL ? 2 : std::stoi(ENV_cons);
    env->N_PROD = ENV_prod == NULL ? 2 : std::stoi(ENV_prod);
    env->MODE = ENV_mode == NULL ? "seq" : ENV_mode;

    if(!(env->MODE == "seq" or env->MODE == "par")) {
        throw std::invalid_argument("Environment variable MODE must be 'seq' or 'par'. Exiting...");
    }

    if(ENV_url == NULL) {
        throw std::invalid_argument("Environment variable URL must be set. Exiting...");
    }
    else {
        env->URL = ENV_url;
    }
}

int main(int argc, char **argv) {
    envvars * env = new envvars;

    try {
        get_env(env);
    }
    catch(const std::invalid_argument &e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    double process_idle_time = 0;
    std::vector<std::string> all_products;

    KrawlerS K;
    std::vector<std::string> pages = K.get_pages(env->URL);

    if(env->MODE == "seq")
        all_products = K.crawl(pages, process_idle_time);
    else
        all_products = K.crawl_par(pages, env->N_PROD, env->N_CONS);

    for(auto p : all_products) std::cout << p;

    return EXIT_SUCCESS;
}
