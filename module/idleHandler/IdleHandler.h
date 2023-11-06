//
// Created by Kris on 2023/11/6.
//

#ifndef UDP_TUNNEL_ONIDLE_H
#define UDP_TUNNEL_ONIDLE_H

#include "hv/EventLoop.h"
#include "../callbackManager/CallBackManager.h"

namespace omg {
    using IdleCallback = std::function<void(void*)>;

    class IdleHandler {
    public:
        static IdleHandler& getInstance();

        // 删除复制和赋值操作
        IdleHandler(const IdleHandler&) = delete;
        IdleHandler& operator=(const IdleHandler&) = delete;

        HANDLER_ID addHandler(IdleCallback callback);
        void removeHandler(HANDLER_ID handlerID);

        static void libhvOnIdleTrigger(hidle_t* idle);
    private:
        // 私有构造函数，确保外部无法创建新实例
        IdleHandler();

    private:
        CallBackManager<void*> _callBackManager;
    };
}

#endif //UDP_TUNNEL_ONIDLE_H
