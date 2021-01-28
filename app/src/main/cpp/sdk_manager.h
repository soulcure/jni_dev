//
// Created by chenqiongyao on 2021/1/28.
//

#ifndef SOCKET_SDK_MANAGER_H
#define SOCKET_SDK_MANAGER_H

#include "singleton.h"
#include "net/tcp/client/tcp_client.h"


class sdk_manager : public singleton<sdk_manager> {
public:
    sdk_manager();
    ~sdk_manager();
    void set_tcp_client(TcpClient *client);

private:
    TcpClient *tcp_client;

};


#endif //SOCKET_SDK_MANAGER_H
