#ifndef __KRAWLERP_H__
#define __KRAWLERP_H__

#include <string>

class KrawlerP {
public:
    KrawlerP(int n_prod, int n_cons);
    void crawl(std::string url);

    int n_prod, n_cons;
};

#endif/*__KRAWLERP_H__*/
