//
// Created by Kris Allen on 2023/10/11.
//

#ifndef UDP_TUNNEL_SOCKET_H
#define UDP_TUNNEL_SOCKET_H
#include <string>
#include <regex>
#include "../../header/typedef.h"

namespace omg {
    namespace utils {
        class Socket {
        public:
            enum ParseErrorCode {
                SUCCESS = 0,
                INVALID_FORMAT,
                INVALID_IPV4_OCTET,
                INVALID_PORT,
                INVALID_IPV6
            };

            /*!
             * 把 IP:PORT 转换成 sockaddr_u
             * @return 错误码
             * 0成功，非0失败
             */
            static int parseIPAddress(const std::string& ipAddress, sockaddr_u* sockaddrU){
                std::string ip;
                int port;

                int errCode = splitIPAddress(ipAddress, ip, port);
                if(errCode != 0)
                    return errCode;

                errCode = sockaddr_set_ipport(sockaddrU, ip.c_str(), port);
                if(errCode != 0)
                    return errCode;

                return 0;
            }

            /*!
             * 解析 IP:PORT 格式的地址类型
             * 并且会进行简单的校验会保证是一个字符串合法的地址
             * @param address 要解析的地址
             * @param ip 存放IP结果
             * @param port 存放端口结果
             * @return ParseErrorCode
             */
            static int splitIPAddress(const std::string& address, std::string& ip, int& port) {
                // 正则表达式匹配 IPv4 或 IPv6 地址
                std::regex ipv4Regex(R"((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}):(\d+))");
                std::regex ipv6Regex(R"((\[[a-fA-F0-9:]+\]):(\d+))");

                std::smatch match;

                // 验证和解析 IPv4 地址
                if (std::regex_match(address, match, ipv4Regex)) {
                    ip = match[1].str();
                    std::string portStr = match[2].str();
                    port = std::stoi(portStr);

                    if (port < 0 || port > 65535) {
                        return INVALID_PORT;
                    }

                    std::regex octetRegex(R"(\d{1,3})");
                    auto octetsBegin = std::sregex_iterator(ip.begin(), ip.end(), octetRegex);
                    auto octetsEnd = std::sregex_iterator();

                    for (std::sregex_iterator i = octetsBegin; i != octetsEnd; ++i) {
                        int octet = std::stoi(i->str());
                        if (octet < 0 || octet > 255) {
                            return INVALID_IPV4_OCTET;
                        }
                    }
                } else if (std::regex_match(address, match, ipv6Regex)) {
                    ip = match[1].str().substr(1, match[1].str().size() - 2); // 去除 []
                    std::string portStr = match[2].str();
                    port = std::stoi(portStr);

                    if (port < 0 || port > 65535) {
                        return INVALID_PORT;
                    }
                } else {
                    return INVALID_FORMAT;
                }

                return SUCCESS;
            }
        };
    }
}

#endif //UDP_TUNNEL_SOCKET_H
