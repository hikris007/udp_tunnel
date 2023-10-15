//
// Created by Kris Allen on 2023/8/15.
//

#ifndef TUNNEL_PAIR_H
#define TUNNEL_PAIR_H

#include <functional>
#include <mutex>
#include <memory>
#include "cstring"
#include "../../header/typedef.h"
#include "../callbackManager/CallBackManager.h"



namespace omg {
    class Pair;
    typedef uint8 PairID;
    typedef std::shared_ptr<Pair> PairPtr;
    typedef std::function<void(const PairPtr& pair)> onCloseCallback;
    typedef std::function<size_t (const PairPtr& pair,const Byte* payload, size_t length)> SendHandler;
    typedef std::function<void(const PairPtr& pair,const Byte* payload, size_t length)> onReceiveCallback;

    static const PairID INVALID_PAIR_ID = 0;

    /**
     * 需要实现以下方法：
     * 构造 ok
     * 析构 ok
     * 设置、获取、删除 上下文智能指针 ok
     * id ok
     * send ok
     * close ok
     */

    class Pair : public std::enable_shared_from_this<Pair>{
    public:
        // 删除复制和赋值操作
        Pair(const Pair&) = delete;
        Pair& operator=(const Pair&) = delete;

        /*!
         * 构造函数
         * @param pairID
         * 会在创建的时候初始化本地暂存区+拷贝PairID到暂存区
         */
        explicit Pair(PairID pairID);

        /*!
         * 析构
         * 会删除暂存区
         */
        ~Pair();

        /*!
         * 获取上下文智能指针
         * @tparam T 你的上下文的类型
         * @return 返回上下文
         */
        template<class T> std::shared_ptr<T> getContextPtr(){
            return std::static_pointer_cast<T>(this->_ctxPtr);
        }

        /*!
         * 删除上下文智能指针
         */
        void deleteContextPtr(){
            this->_ctxPtr.reset();
        }

        /*!
         * 设置上下文智能指针
         * @param ctx 上下文
         */
        void setContextPtr(const std::shared_ptr<void> &ctx){
            this->_ctxPtr = ctx;
        }

        // 获取映射唯一ID
        PairID id() const;

        // 发送的数据怎么处理
        SendHandler sendHandler = nullptr;

        /*!
         * 发送函数（线程安全）
         * @param payload
         * @param len
         * @return 成功发送的字节数（不包括 PairID 头大小）
         */
        size_t send(const Byte* payload, size_t len);

        /*!
         * 接收的回调
         * 传入 当前隧道和发送完成的消息
         */
        onReceiveCallback onReceive = nullptr;

        /*!
         * 关闭 Pair 线程安全
         * @return 错误码
         * 调用所有的关闭回调 & 设置状态为 closed
         * 0为成功
         */
        int close();

        // 关闭回调
        HANDLER_ID addOnCloseHandler(onCloseCallback callback);
        void removeOnCloseHandler(HANDLER_ID handlerID);

    protected:
    private:
        // PairID
        PairID _pairID;

        // 是否关闭
        bool _isClosed;

        // 暂存数据 把两段数据拼起来(包含 PairID 头)
        Byte* _data = nullptr;

        // 上下文智能指针
        std::shared_ptr<void> _ctxPtr;

        // 锁
        std::mutex _closeMutex;
        std::mutex _sendMutex;

        // 关闭回调管理器 线程安全
        CallBackManager<const std::shared_ptr<Pair>> _onCloseCallbacks;
    };
}


#endif //TUNNEL_PAIR_H
