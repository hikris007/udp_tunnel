cmake_minimum_required(VERSION 3.25)
project(udp_tunnel)

set(CMAKE_CXX_STANDARD 11)

find_package(libhv CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

add_executable(udp_tunnel
        Client.cpp
        ClientForwarder.cpp
        ClientPairManager.cpp
        Logger.cpp
        Pair.cpp
        Server.cpp
        ServerPairManager.cpp
        TunnelFactory.cpp
        TestTunnel.cpp
        main.cpp
)
add_executable(test test.cpp)

target_link_libraries(udp_tunnel PRIVATE hv_static)
target_link_libraries(udp_tunnel PRIVATE spdlog::spdlog)
target_link_libraries(udp_tunnel PRIVATE CLI11::CLI11)

target_link_libraries(test PRIVATE hv_static)
target_link_libraries(test PRIVATE spdlog::spdlog)
target_link_libraries(test PRIVATE CLI11::CLI11)
