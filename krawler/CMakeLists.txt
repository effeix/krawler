cmake_minimum_required(VERSION 2.6)
project(krawler)

# Point to our own cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)
find_package(PkgConfig REQUIRED)

find_package(Boost 1.65.1 COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIR})

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)

# add_executable(krawler main.cpp curlhandler.cpp product.cpp krawlers.cpp krawlerp.cpp semaphore.cpp)
# include_directories(${CURL_INCLUDE_DIR})
# target_link_libraries(krawler "-lcurl -lpthread" ${Boost_LIBRARIES} ${CURL_LIBRARIES})

add_executable(krawler main.cpp curlhandler.cpp product.cpp krawlers.cpp semaphore.cpp)
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(krawler "-lcurl -lpthread" ${Boost_LIBRARIES} ${CURL_LIBRARIES})

install(TARGETS krawler RUNTIME DESTINATION bin)
