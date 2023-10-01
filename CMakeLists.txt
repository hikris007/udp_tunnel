cmake_minimum_required(VERSION 3.25)
project(udp_tunnel)

set(CMAKE_CXX_STANDARD 11)

find_package(libhv CONFIG REQUIRED)

add_executable(udp_tunnel main.cpp)

target_link_libraries(udp_tunnel PRIVATE hv_static)