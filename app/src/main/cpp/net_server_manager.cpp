//
// Created by chenqiongyao on 2021/1/28.
//

#include <thread>
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
    LOGD("TcpServer to:[%d]", port);
    std::thread t(std::mem_fn(&net_server_manager::Open), this, port);//使用类的成员函数作为线程参数
    t.detach();   //与当前线程分离
}

void net_server_manager::Open(int port) {
    LOGD("TcpServer Open :[%d]", port);
    tcp_server->StartServer(port);
}


