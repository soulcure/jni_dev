//
// Created by chenqiongyao on 2021/1/28.
//

#ifndef SOCKET_SDK_MANAGER_H
#define SOCKET_SDK_MANAGER_H

#include "net/tcp/client/tcp_client.h"
#include "log/log_util.h"
#include "net/pdu_base.h"
#include "platform/android/cpp_call_java.h"

class sdk_manager {
public:
    sdk_manager(const sdk_manager &) = delete;

    void create_tcp_client(const char *ip, int port);

    void send_message(int length, const char *message);

    void send_bytes(int length, const char *message);

    static sdk_manager &Get() {
        static sdk_manager instance;
        return instance;
    }


private:
    sdk_manager();

    ~sdk_manager();

    TcpClient *tcp_client;
    TcpClient::OnConnectState onConnectStateListener;
    TcpClient::OnReceive onReceiveListener;


};


#endif //SOCKET_SDK_MANAGER_H
