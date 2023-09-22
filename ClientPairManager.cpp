//
// Created by Kris on 2023/9/11.
//

#include "header/ClientPairManager.h"

Int ClientPairManager::createTunnel() {
    // 创建隧道
    Tunnel *tunnel = TunnelFactory::createTunnel();

    // 设置上下文
    return 0;
}

Int ClientPairManager::createPair() {
    if(this->availableTunnelIDs.empty()){
        // TODO: 创建Tunnel
        TunnelPtr tunnel;
        TunnelID tunnelID = tunnel->id();

        // 添加到 Tunnels 列表
        this->tunnels.insert(std::pair<TunnelID, TunnelPtr>(tunnelID, tunnel));

        // 添加到计数列表
        // 这里的值为1是因为创建这个隧道就是为了这个Pair
        auto insertResult = this->tunnelPairCounter.insert(std::pair<TunnelID, int>(tunnelID, 1));
        if(!insertResult.second){
            // TODO:添加失败
        }
        std::unordered_map<TunnelID ,int>::iterator targetCounter = insertResult.first;

        // 如果有剩余空间就添加到可用的隧道列表
        if(targetCounter->second < MAX_PAIRS_PER_TUNNEL){
            this->availableTunnelIDs.push(tunnelID);
        }
    }else{
        // 获取第一个空闲的 TunnelID
        TunnelID tunnelID = this->availableTunnelIDs.front();

        // 在计数器中查找
        std::unordered_map<TunnelID ,int>::iterator targetCounter = this->tunnelPairCounter.find(tunnelID);
        if(targetCounter == this->tunnelPairCounter.end()){
            // TODO: 如果计数器不存在的逻辑
            return 1;
        }

        // 因为要使用这个底层传输 所以计数+1
        targetCounter->second++;

        // 如果隧道的空位满了就从可用列表中移除
        if(targetCounter->second >= MAX_PAIRS_PER_TUNNEL){
            this->availableTunnelIDs.pop();
        }
    }
    return 0;
}