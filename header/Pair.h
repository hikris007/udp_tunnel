//
// Created by Kris Allen on 2023/8/15.
//

#ifndef TUNNEL_PAIR_H
#define TUNNEL_PAIR_H

#include <list>
#include <functional>
#include <mutex>
#include "Payload.h"
#include "CallBackManager.h"

class Pair;

typedef Uint32 PairID;
typedef std::function<void(const std::shared_ptr<Pair> pair)> onCloseCallback;
typedef std::function<SizeT(const std::shared_ptr<Pair> pair,const Byte* payload, SizeT len)> sendHandler;
typedef std::function<void(const std::shared_ptr<Pair> pair,const Byte* payload)> onRecvCallback;

static const PairID INVALID_PAIR_ID = 0;

class Pair : public std::enable_shared_from_this<Pair>{
public:
    explicit Pair(PairID pairID);
    ~Pair();

    template<class T> std::shared_ptr<T> getContextPtr();// 获取上下文智能指针
    void deleteContextPtr();// 删除上下文智能指针
    void setContextPtr(const std::shared_ptr<void> &ctx);// 设置上下文智能指针

    // 获取映射唯一ID
    PairID id() const;

    // 返回发送的字节数
    // 传入消息
    sendHandler handleSend = nullptr;
    SizeT send(const Byte* payload, SizeT len);

    // 接收的回调
    // 传入 当前隧道和发送完成的消息
    onRecvCallback onReceive = nullptr;

    // 关闭
    Int close();

    // 关闭回调
    HANDLER_ID addOnCloseHandler(onCloseCallback callback);
    void removeOnCloseHandler(HANDLER_ID handlerID);

protected:
private:
    PairID _pairID;
    Byte* _data = nullptr; // 暂存数据 把两段数据拼起来(包含 PairID 头)
    std::shared_ptr<void> _ctxPtr;
    std::mutex _locker;
    CallBackManager<const std::shared_ptr<Pair>> _onCloseCallbacks;
};

typedef std::shared_ptr<Pair> PairPtr;

#endif //TUNNEL_PAIR_H
