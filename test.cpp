#include <iostream>
#include "hv/UdpServer.h"
#include "module/pair/Pair.h"

int main() {
    omg::PairPtr pair = std::make_shared<omg::Pair>(10);
    pair->sendHandler = [](const omg::PairPtr& pair,const omg::Byte* payload, omg::SizeT len){
        std::cout << "写入：" << payload << std::endl;
        return 0;
    };

    hv::UdpServer udpServer;
    udpServer.onMessage = [](const hv::SocketChannelPtr& channel, hv::Buffer* buffer){
        channel->startConnect(1,"");
    };

    char data[] = {'H','e','y', ' ', 'K','r','i','s'};

    pair->send(reinterpret_cast<const omg::Byte *>(data), 8);
    return 0;
}

//#include <iostream>
//#include <list>
//#include "module/callbackManager/CallBackManager.h"
//
//int main() {
//    omg::CallBackManager<int> callBackManager;
//
//    callBackManager.add([](int i){
//        std::cout << "N 1" << std::endl;
//    });
//
//    HANDLER_ID id = callBackManager.add([](int i){
//        std::cout << "N 2" << std::endl;
//    });
//
//    callBackManager.add([](int i){
//        std::cout << "N 3" << std::endl;
//    });
//
//    callBackManager.remove(id);
//
//    callBackManager.trigger(1);
//    return 0;
//}