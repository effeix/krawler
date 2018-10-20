#include "curlhandler.h"
#include <iostream>

int main(int argc, char **argv) {
    std::string response;

    response = http_get("https://www.magazineluiza.com.br/smartphone/celulares-e-smartphones/s/te/tcsp/");

    std::cout << response << std::endl;

    return 0;
}
