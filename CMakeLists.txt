cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
project(SafehouseDB CXX)

include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.x
)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG develop
)

FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum
  GIT_TAG master
)
  
FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(magic_enum)

add_executable(safehouse-db src/main.cpp src/db.cpp src/table.cpp)
target_link_libraries(safehouse-db PRIVATE spdlog::spdlog)
target_link_libraries(safehouse-db PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(safehouse-db PRIVATE magic_enum::magic_enum)
target_include_directories(safehouse-db PRIVATE include)