#include "krawlerp.hpp"
#include <iostream>
#include <thread>

KrawlerP::KrawlerP(int n_prod, int n_cons) : n_prod(n_prod), n_cons(n_cons) {}

void KrawlerP::crawl(std::string url) {
    std::cout << "Parallel Crawl" << std::endl;
}

