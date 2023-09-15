#include <iostream>
#include <unordered_map>
#include "header/CallBackManager.h"
int main() {
//    CallBackManager<int> callBackManager;
//
//    auto callback1 = [](int i){
//
//    };
//    int fn1 = callBackManager.add([](int a){
//        std::cout << "Fn1" << std::endl;
//    });
//
//    int fn2 = callBackManager.add([](int a){
//        std::cout << "Fn2" << std::endl;
//    });
//
//    int fn3 = callBackManager.add([](int a){
//        std::cout << "Fn3" << std::endl;
//    });
//
//    callBackManager.remove(fn2);
//
//    callBackManager.trigger(1);
//    std::cout << "Hello, World!" << std::endl;

    std::unordered_map<std::string, int> myMap;
    myMap["apple"] = 5;
    myMap["banana"] = 7;

// 使用 find 方法查找键
    auto it = myMap.find("apple");

// 如果键在 unordered_map 中
    if (it != myMap.end()) {
        // 使用 ++ 语法增加值
//        ++(it->second);
        it->second++;
    }

    std::cout << myMap["apple"] << std::endl;
    return 0;
}
