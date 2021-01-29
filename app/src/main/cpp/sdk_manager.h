//
// Created by chenqiongyao on 2021/1/28.
//

#ifndef SOCKET_SDK_MANAGER_H
#define SOCKET_SDK_MANAGER_H

#include "net/tcp/client/tcp_client.h"


class sdk_manager {
public:
    sdk_manager(const sdk_manager &) = delete;

    void set_tcp_client(TcpClient *client);

    static sdk_manager &Get() {
        static sdk_manager instance;
        return instance;
    }


private:
    sdk_manager();

    ~sdk_manager();

    TcpClient *tcp_client;

};


#endif //SOCKET_SDK_MANAGER_H
