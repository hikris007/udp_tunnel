//#include <iostream>
//#include "module/pair/Pair.h"
//
//int main() {
//    omg::PairPtr pair = std::make_shared<omg::Pair>(10);
//    pair->sendHandler = [](const omg::PairPtr& pair,const omg::Byte* payload, omg::SizeT len){
//        std::cout << "写入：" << payload << std::endl;
//        return 0;
//    };
//
//    char data[] = {'H','e','y', ' ', 'K','r','i','s'};
//
//    pair->send(reinterpret_cast<const omg::Byte *>(data), 8);
//    return 0;
//}

#include <iostream>
#include <list>

int main() {
    std::list<int> numbers = {1, 2, 3, 4, 5};

    for(auto it = numbers.rbegin(); it != numbers.rend(); ++it) {
        std::cout << *it << " ";
    }

    return 0;
}