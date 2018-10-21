#include "main.hpp"
#include "krawlerp.hpp"
#include "krawlers.hpp"
#include <iostream>
#include <stdexcept>


void get_env(envvars* env) {
    const char * ENV_cons = std::getenv("N_CONS_THREADS");
    const char * ENV_prod = std::getenv("N_PROD_THREADS");
    const char * ENV_url = std::getenv("URL");
    const char * ENV_mode = std::getenv("MODE");

    env->N_CONS_THREADS = ENV_cons == NULL ? 2 : std::stoi(ENV_cons);
    env->N_PROD_THREADS = ENV_prod == NULL ? 2 : std::stoi(ENV_prod);
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


    if(env->MODE == "seq") {
        KrawlerS ks;
        ks.crawl(env->URL);
    }
    else {
        KrawlerP kp(env->N_PROD_THREADS, env->N_CONS_THREADS);
        kp.crawl(env->URL);
    }

    return EXIT_SUCCESS;
}
