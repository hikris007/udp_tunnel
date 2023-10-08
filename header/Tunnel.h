//
// Created by Kris Allen on 2023/8/15.
//

#ifndef TUNNEL_TUNNEL_H

#include <functional>
#include "Payload.h"

#define TUNNEL_TUNNEL_H

class Tunnel;
typedef std::shared_ptr<Tunnel> TunnelPtr;


typedef Uint32 TunnelID;

const TunnelID INVALID_TUNNEL_ID = 0;

class Tunnel {
public:
    enum State {
        // INITIAL -> CONNECTING
        //               -> ERROR
        //               -> CONNECTED
        //                     -> RECONNECTING
        //                           -> ERROR
        //                           -> CONNECTED -> DESTROYING -> DESTROYED
        //                     -> DESTROYING -> DESTROYED
        INITIAL,

        CONNECTING,
        CONNECTED,
        RECONNECTING,

        ERROR,

        DESTROYING,
        DESTROYED
    };

    enum Type {
        RELIABLE,
        UNRELIABLE,
        UNCERTAIN
    };

    virtual ~Tunnel() = default;

    // 获取上下文智能指针
    template<class T> std::shared_ptr<T> getContextPtr(){
        return std::static_pointer_cast<T>(this->_ctxPtr);
    }

    // 设置上下文智能指针
    void setContextPtr(const std::shared_ptr<void>& ctx){
        std::lock_guard<std::mutex> locker(this->_locker);
        this->_ctxPtr = ctx;
    }

    // 删除上下文智能指针
    void deleteContextPtr(){
        this->_ctxPtr.reset();
    }

    // 销毁隧道
    virtual Int destroy() = 0;

    // 当隧道被销毁的回调
    // 传入当前隧道的指针
    std::function<void(const TunnelPtr& tunnel)> onDestroy = nullptr;

    // 传入消息
    // 返回写入的字节数
    virtual SizeT send(const Byte* payload, SizeT len) = 0;

    // 当有消息的回调
    // 传入当前隧道和发送完成的消息
    std::function<void(const TunnelPtr& tunnel, const Byte* payload, SizeT length)> onReceive = nullptr;

    // 当隧道状态变更的回调
    // 传入当前隧道
    // 之前的的状态
    // 当前状态
    std::function<void(const TunnelPtr tunnel, State from, State state)> onStateChange = nullptr;

    // 获取当前状态
    virtual State state() = 0;

    // 获取隧道类型
    virtual Type type() = 0;

    // 获取隧道的唯一 ID
    virtual TunnelID id() = 0;

protected:
    std::mutex _locker;
    std::shared_ptr<void> _ctxPtr;
};


#endif //TUNNEL_TUNNEL_H
