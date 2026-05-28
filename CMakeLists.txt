cmake_minimum_required(VERSION 3.27)
project(make_it_work)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For GMP
include_directories(/usr/local/include/)
link_directories(/usr/local/lib)

find_library(GMP_LIBRARY gmp HINTS /usr/local/lib)
find_library(GMPXX_LIBRARY gmpxx HINTS /usr/local/lib)

add_executable(make_it_work 2Dfiles_no_stock.cpp)

target_link_libraries(make_it_work ${GMP_LIBRARY} ${GMPXX_LIBRARY})
