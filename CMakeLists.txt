cmake_minimum_required(VERSION 3.22)
project(udp_tunnel)

set(CMAKE_CXX_STANDARD 11)

find_package(libhv CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(Boost CONFIG COMPONENTS statechart)
find_package(cityhash CONFIG REQUIRED)


include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(udp_tunnel
        module/client/Client.cpp
        module/server/Server.cpp
        module/listener/ListenerFactory.cpp
        module/listener/libhvWsListener/LibhvWsListener.cpp
        module/tunnel/LibhvWsTunnel/Server.cpp
        module/clientForwarder/ClientForwarder.cpp
        module/clientPairManager/ClientPairManager.cpp
        module/logger/Logger.cpp
        module/pair/Pair.cpp
        module/tunnel/TunnelFactory.cpp
        module/tunnel/LibhvWsTunnel/Client.cpp
        module/serverPairManager/ServerPairManager.cpp
        module/tunnelIDPool/TunnelIDPool.cpp
        module/udpClient/UDPClientFactory.cpp
        module/payload/PayloadWrapper.cpp
        module/payload/Payload.cpp
        main.cpp
        )
add_executable(
        test
        test.cpp
        module/pair/Pair.cpp
        module/logger/Logger.cpp
        module/tunnel/TunnelFactory.cpp
        module/tunnel/LibhvWsTunnel/Client.cpp
        module/tunnel/LibhvWsTunnel/Server.cpp
        module/listener/libhvWsListener/LibhvWsListener.cpp
        module/tunnelIDPool/TunnelIDPool.cpp
        module/serverPairManager/ServerPairManager.cpp
        module/udpClient/UDPClientFactory.cpp
        module/payload/PayloadWrapper.cpp
        module/payload/Payload.cpp
)


target_link_libraries(udp_tunnel PRIVATE hv_static)
target_link_libraries(udp_tunnel PRIVATE spdlog::spdlog)
target_link_libraries(udp_tunnel PRIVATE CLI11::CLI11)
target_link_libraries(udp_tunnel ${Boost_LIBRARIES})
target_link_libraries(udp_tunnel PRIVATE cityhash)


target_link_libraries(test PRIVATE hv_static)
target_link_libraries(test PRIVATE spdlog::spdlog)
target_link_libraries(test PRIVATE CLI11::CLI11)
target_link_libraries(test ${Boost_LIBRARIES})
target_link_libraries(test PRIVATE cityhash)

