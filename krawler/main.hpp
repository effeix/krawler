#ifndef __MAIN_H__
#define __MAIN_H__

#include <string>

typedef struct envvars_t {
    std::string URL;
    std::string MODE;
    int N_CONS_THREADS;
    int N_PROD_THREADS;
} envvars;

#endif/*__MAIN_H__*/
