//
// Created by chenqiongyao on 2021/1/28.
//

#ifndef SOCKET_SINGLETON_H
#define SOCKET_SINGLETON_H

#include <memory>

template<class T>
class singleton {
public:
    static std::shared_ptr<T> Instance() {
        if (!m_pInstance) {
            m_pInstance = std::make_shared<T>();
        }
        return m_pInstance;
    }

protected:
    singleton() {}

    ~singleton() {}

private:
    singleton(singleton const &);

    singleton &operator=(singleton const &);

    static std::shared_ptr<T> m_pInstance;
};

template<class T>
std::shared_ptr<T> singleton<T>::m_pInstance = nullptr;
#endif //SOCKET_SINGLETON_H
