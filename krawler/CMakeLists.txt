cmake_minimum_required(VERSION 3.5)
project(Krawler)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)
find_package(PkgConfig REQUIRED)

find_package(Boost 1.65.1 COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIR})

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(
    krawler
    main.cpp
    curlhandler.cpp
    product.cpp
    krawler.cpp
    semaphore.cpp
)

target_link_libraries(krawler "-lcurl -lpthread" ${Boost_LIBRARIES} ${CURL_LIBRARIES})
