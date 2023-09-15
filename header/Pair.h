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

class Pair : public std::enable_shared_from_this<Pair>{
public:

    void *context();// 获取上下文指针

    template<class T> T *getContext();// 获取上下文指针(带类型)
    template<class T> void deleteContext();// 删除上下文(带类型)
    void setContext(void *ctx);// 设置上下文指针

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
    onRecvCallback onRecv = nullptr;

    // 关闭
    Int close();

    // 关闭回调
    HANDLER_ID addOnCloseHandler(onCloseCallback callback);
    void removeOnCloseHandler(HANDLER_ID handlerID);

protected:
private:
    PairID _pairID;
    void *_ctx = nullptr;
    std::shared_ptr<void> _ctxPtr;
    std::mutex _locker;
    CallBackManager<const std::shared_ptr<Pair>> onCloseCallbacks;
};

typedef std::shared_ptr<Pair> PairPtr;

#endif //TUNNEL_PAIR_H
