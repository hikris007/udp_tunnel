#include <iostream>
#include <string>
#include <regex>

enum ErrorCode {
    SUCCESS = 0,
    INVALID_FORMAT,
    INVALID_IPV4_OCTET,
    INVALID_PORT,
    INVALID_IPV6
};

int parseIPAddress(const std::string& address, std::string& ip, int& port) {
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

int main() {
    std::string ip;
    int port;

    // 测试用例
    int errorCode1 = parseIPAddress("392.168.1.1:8080", ip, port);
    if (errorCode1 == SUCCESS) {
        std::cout << "IP: " << ip << " Port: " << port << std::endl;
    } else {
        std::cerr << "Error code: " << errorCode1 << std::endl;
    }

    int errorCode2 = parseIPAddress("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:80", ip, port);
    if (errorCode2 == SUCCESS) {
        std::cout << "IP: " << ip << " Port: " << port << std::endl;
    } else {
        std::cerr << "Error code: " << errorCode2 << std::endl;
    }

    return 0;
}