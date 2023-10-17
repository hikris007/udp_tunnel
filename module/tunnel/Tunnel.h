//
// Created by Kris Allen on 2023/8/15.
//

#ifndef TUNNEL_TUNNEL_H
#define TUNNEL_TUNNEL_H

#include <functional>
#include "../payload/Payload.h"
#include "../callbackManager/CallBackManager.h"
#include "../contextManager/ContextManager.hpp"

namespace omg {

    class Tunnel;
    typedef std::shared_ptr<Tunnel> TunnelPtr;

    typedef uint32 TunnelID;
    const TunnelID INVALID_TUNNEL_ID = 0;

    // 回调类型定义
    typedef std::function<void(const TunnelPtr& tunnel)> OnReadyCallback;
    typedef std::function<void(const TunnelPtr& tunnel, void* data)> OnErrorCallback;
    typedef std::function<void(const TunnelPtr& tunnel)> OnDestroyCallback;

    class Tunnel : public ContextManager {
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

        // 销毁隧道
        virtual int destroy() = 0;

        // 传入消息
        // 返回写入的字节数
        virtual size_t send(const Byte* payload, size_t length) = 0;

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
        virtual HANDLER_ID addOnReadyHandler(OnReadyCallback onReadyCallback) = 0;
        virtual void removeOnReadyHandler(HANDLER_ID handlerID) = 0;

        // 隧道建立失败调用
        virtual HANDLER_ID addOnErrorHandler(OnErrorCallback onErrorCallback) = 0;
        virtual void removeOnErrorHandler(HANDLER_ID handlerID) = 0;

        // 当隧道被销毁之前的回调
        virtual HANDLER_ID addOnDestroyHandler(OnDestroyCallback onDestroyCallback) = 0;
        virtual void removeOnDestroyHandler(HANDLER_ID handlerID) = 0;

    protected:
    };

}
#endif //TUNNEL_TUNNEL_H
