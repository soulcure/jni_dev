//
// Created by chenqiongyao on 2021/1/28.
//

#ifndef SOCKET_SDK_MANAGER_H
#define SOCKET_SDK_MANAGER_H

#include "tcp_client.h"
#include "log_util.h"
#include "pdu_base.h"

class sdk_manager {
public:

    void create_tcp_client(const char *ip, int port);

    void send_message(int length, const char *message);

    void send_bytes(int length, const char *message);

    static sdk_manager &Get();


private:
    sdk_manager(const sdk_manager &) = delete;

    sdk_manager(const sdk_manager &&) = delete;

    sdk_manager operator=(const sdk_manager &) = delete;

    sdk_manager();

    ~sdk_manager();

    TcpClient *tcp_client;
    TcpClient::OnConnectState onConnectStateListener;
    TcpClient::OnReceive onReceiveListener;


};


#endif //SOCKET_SDK_MANAGER_H
