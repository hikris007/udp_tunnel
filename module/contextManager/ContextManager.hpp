//
// Created by Kris Allen on 2023/10/17.
//

#ifndef UDP_TUNNEL_CONTEXTMANAGER_H
#define UDP_TUNNEL_CONTEXTMANAGER_H

#include <memory>

namespace omg {
    class ContextManager {
    public:
        /*!
         * 获取上下文智能指针
         * @tparam T 你的上下文的类型
         * @return 返回上下文
         */
        template<class T> std::shared_ptr<T> getContextPtr(){
            return std::static_pointer_cast<T>(this->_contextPtr);
        }

        /*!
         * 删除上下文智能指针
         */
        void deleteContextPtr(){
            this->_contextPtr.reset();
        }

        /*!
         * 设置上下文智能指针
         * @param ctx 上下文
         */
        void setContextPtr(const std::shared_ptr<void> &ctx){
            this->_contextPtr = ctx;
        }

    private:
        std::shared_ptr<void> _contextPtr;
    };
}
#endif //UDP_TUNNEL_CONTEXTMANAGER_H
