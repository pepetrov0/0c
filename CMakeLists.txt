cmake_minimum_required(VERSION 3.29)
project(0c)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# find pkg-config
find_package(PkgConfig)

# find dependencies
pkg_search_module(spdlog REQUIRED spdlog)
pkg_search_module(cppzmq REQUIRED cppzmq)

# add common library
add_library(0c lib.cpp)
target_include_directories(0c PUBLIC ${spdlog_INCLUDE_DIRS} ${cppzmq_INCLUDE_DIRS})
target_link_libraries(0c PUBLIC ${spdlog_LIBRARIES} ${cppzmq_LIBRARIES})

# add push-pull executable
add_executable(0c-push-pull push_pull.cpp)
target_link_libraries(0c-push-pull PRIVATE 0c)

# add pub-sub executable
add_executable(0c-pub-sub pub_sub.cpp)
target_link_libraries(0c-pub-sub PRIVATE 0c)

