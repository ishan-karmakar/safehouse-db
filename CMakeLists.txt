cmake_minimum_required(VERSION 3.10)

project(SafehouseDB CXX)
add_executable(safehouse-db src/main.cpp)
target_include_directories(safehouse-db PRIVATE include)