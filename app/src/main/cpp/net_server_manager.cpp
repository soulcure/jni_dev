//
// Created by chenqiongyao on 2021/1/28.
//

#include "net_server_manager.h"
#include "platform/android/JniHelper.h"

net_server_manager::net_server_manager() {
    LOGD("net_server_manager constructor");
    tcp_server = new NetBase();
}

net_server_manager::~net_server_manager() {
    LOGD("net_server_manager destructor");
    delete tcp_server;
}


void net_server_manager::create_tcp_server(int port) {
    LOGD("TcpClient to:[%d]", port);
    tcp_server->StartServer(port);
}




