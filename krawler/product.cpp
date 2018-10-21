#include "product.hpp"
#include <iostream>

Product::Product(std::string name,
                 std::string description,
                 std::string pic_url,
                 std::string price,
                 std::string price_in_installment,
                 std::string installment_qty,
                 std::string category,
                 std::string prod_url) :
    name(name),
    description(description),
    pic_url(pic_url),
    price(price),
    price_in_installment(price_in_installment),
    installment_qty(installment_qty),
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
    std::cout << "    " << "\"preco_parcelado\":" << " " + price_in_installment + "," << std::endl;
    std::cout << "    " << "\"preco_num_parcelas\":" << " " + installment_qty + "," << std::endl;
    std::cout << "    " << "\"categoria\":" << " \"" + category + "\"," << std::endl;
    std::cout << "    " << "\"url\":" << " \"" + prod_url + "\"," << std::endl;
    std::cout << "}";
    std::cout << std::endl << std::endl;
}

