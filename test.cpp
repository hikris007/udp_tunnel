#include <string>
#include "hv/WebSocketServer.h"
#include "hv/hfile.h"
#include "json/json.h"

int main(){
    std::string str = "wss://10.0.0.0:1";
    std::string str2 = "";
    std::cout << str.empty() << str2.empty() << std::endl;
    HFile file;
    file.open("./hikris.txt", "rb");
    std::string res;
//    file.readall(res);
    file.close();

    Json::Reader reader;
    Json::Value root;
    if(!reader.parse(res, root)){
        std::cout << "ERRJSON" << std::endl;
        return -1;
    }

    std::cout << res << root["name"].asString() << root["age"].asInt() << root["aa"].asInt() << std::endl;
    return 0;
}