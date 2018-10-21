#include "product.hpp"
#include <iostream>

Product::Product(std::string name,
                 std::string description,
                 std::string pic_url,
                 std::string price,
                 std::string priceInstallment,
                 std::string priceInstallmentQty,
                 std::string category,
                 std::string prod_url) :
    name(name),
    description(description),
    pic_url(pic_url),
    price(price),
    priceInstallment(priceInstallment),
    priceInstallmentQty(priceInstallmentQty),
    category(category),
    prod_url(prod_url)
{
}

void Product::display() {
    std::cout << "{" << std::endl;
    std::cout << "    " << "\"nome\":" << " \"" + name + "\"," << std::endl;
    std::cout << "    " << "\"descricao\":" << " \"" + description + "\"," << std::endl;
    std::cout << "    " << "\"foto\":" << " \"" + pic_url + "\"," << std::endl;
    std::cout << "    " << "\"preco\":" << " " + price + "," << std::endl;
    std::cout << "    " << "\"preco_parcelado\":" << " " + priceInstallment + "," << std::endl;
    std::cout << "    " << "\"preco_num_parcelas\":" << " " + priceInstallmentQty + "," << std::endl;
    std::cout << "    " << "\"categoria\":" << " \"" + category + "\"," << std::endl;
    std::cout << "    " << "\"url\":" << " \"" + prod_url + "\"," << std::endl;
    std::cout << "}";
    std::cout << std::endl << std::endl;
}

