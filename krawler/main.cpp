#include "curlhandler.h"
#include <iostream>
#include <stdexcept>

typedef struct envvars_t {
    std::string URL;
    int N_CONS_THREADS;
    int N_PROD_THREADS;
} envvars;

void get_env(envvars* env) {
    const char * ENV_cons = std::getenv("N_CONS_THREADS");
    const char * ENV_prod = std::getenv("N_PROD_THREADS");
    const char * ENV_url = std::getenv("URL");

    env->N_CONS_THREADS = ENV_cons == NULL ? 2 : std::stoi(ENV_cons);
    env->N_PROD_THREADS = ENV_prod == NULL ? 2 : std::stoi(ENV_prod);

    if(ENV_url == NULL) {
        throw std::invalid_argument("URL undefined. Exiting...");
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
    catch(const std::invalid_argument & e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    std::string response;
    response = http_get(env->URL);
    std::cout << response << std::endl;

    return 0;
}
