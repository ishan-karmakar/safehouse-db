cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 17)
project(SafehouseDB C)

include(FetchContent)
FetchContent_Declare(
  stumpless
  GIT_REPOSITORY https://github.com/goatshriek/stumpless
  GIT_TAG latest
)

FetchContent_MakeAvailable(stumpless)

add_executable(safehouse-db src/main.c)
target_link_libraries(safehouse-db PRIVATE stumpless)
target_include_directories(safehouse-db PRIVATE ${stumpless_SOURCE_DIR}/include ${stumpless_BINARY_DIR}/include)
target_include_directories(safehouse-db PRIVATE include)