cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
project(SafehouseDB CXX)

include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.x
)
  
FetchContent_MakeAvailable(spdlog)

add_executable(safehouse-db src/main.cpp src/db.cpp src/table.cpp)
target_link_libraries(safehouse-db PRIVATE spdlog::spdlog)
target_include_directories(safehouse-db PRIVATE include)