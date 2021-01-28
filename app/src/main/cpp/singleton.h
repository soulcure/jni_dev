//
// Created by chenqiongyao on 2021/1/28.
//

#ifndef SOCKET_SINGLETON_H
#define SOCKET_SINGLETON_H

template<class T>
class singleton {
public:
    singleton(){

    }

    ~singleton(){

    }
    static T *Instance() {
        if (!m_pInstance) m_pInstance = new T;
        assert(m_pInstance != nullptr);
        return m_pInstance;
    }

    static void Release() {
        if (m_pInstance) delete m_pInstance;
        m_pInstance = 0;
    }

protected:


private:
    singleton(singleton const &);

    singleton &operator=(singleton const &);

    static T *m_pInstance;
};

template<class T>
T *singleton<T>::m_pInstance = nullptr;
#endif //SOCKET_SINGLETON_H
