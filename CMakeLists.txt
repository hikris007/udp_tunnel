cmake_minimum_required(VERSION 3.22)
project(udp_tunnel)

set(CMAKE_CXX_STANDARD 11)

find_package(libhv CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(Boost CONFIG COMPONENTS statechart)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(udp_tunnel
        module/client/Client.cpp
        module/clientForwarder/ClientForwarder.cpp
        module/clientPairManager/ClientPairManager.cpp
        module/logger/Logger.cpp
        module/pair/Pair.cpp
        module/server/Server.cpp
        module/serverPairManager/ServerPairManager.cpp
        module/tunnel/TunnelFactory.cpp
        main.cpp
        )
add_executable(
        test
        test.cpp
        module/pair/Pair.cpp
        module/logger/Logger.cpp
        module/tunnel/LibhvWsClientTunnel/LibhvWsClientTunnel.cpp
)

target_link_libraries(udp_tunnel PRIVATE hv_static)
target_link_libraries(udp_tunnel PRIVATE spdlog::spdlog)
target_link_libraries(udp_tunnel PRIVATE CLI11::CLI11)
target_link_libraries(udp_tunnel ${Boost_LIBRARIES})

target_link_libraries(test PRIVATE hv_static)
target_link_libraries(test PRIVATE spdlog::spdlog)
target_link_libraries(test PRIVATE CLI11::CLI11)
target_link_libraries(test ${Boost_LIBRARIES})
