//
// Created by Kris Allen on 2023/8/15.
//

#ifndef TUNNEL_TUNNEL_H
#define TUNNEL_TUNNEL_H

#include <functional>
#include "../payload/Payload.h"

namespace omg {

    class Tunnel;
    typedef std::shared_ptr<Tunnel> TunnelPtr;

    typedef uint32 TunnelID;
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

        struct StateResult {
            State previous;
            State current;
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
            this->_ctxPtr = ctx;
        }

        // 删除上下文智能指针
        void deleteContextPtr(){
            this->_ctxPtr.reset();
        }

        // 销毁隧道
        virtual int destroy() = 0;

        // 传入消息
        // 返回写入的字节数
        virtual size_t send(const Byte* payload, size_t len) = 0;

        // 获取当前状态
        virtual const StateResult& state() = 0;

        // 获取隧道类型
        virtual Type type() = 0;

        // 获取隧道的唯一 ID
        virtual TunnelID id() = 0;

        // ------- 回调 -------

        // 当有消息的回调
        // 传入当前隧道和发送完成的消息
        std::function<void(const TunnelPtr& tunnel, const Byte* payload, size_t length)> onReceive = nullptr;

        // 当隧道建立成功后调用
        std::function<void(const TunnelPtr& tunnel)> onReady = nullptr;

        // 隧道建立失败调用
        std::function<void(const TunnelPtr& tunnel, void* data)> onError = nullptr;

        // 当隧道被销毁之前的回调
        // 传入当前隧道的指针
        std::function<void(const TunnelPtr& tunnel)> onDestroy = nullptr;

    protected:
        std::mutex _locker;
        std::shared_ptr<void> _ctxPtr;
    };

}
#endif //TUNNEL_TUNNEL_H
