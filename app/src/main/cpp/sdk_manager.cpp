//
// Created by chenqiongyao on 2021/1/28.
//

#include "sdk_manager.h"

sdk_manager::sdk_manager() {

}

sdk_manager::~sdk_manager() {
    if (tcp_client != nullptr) {
        delete tcp_client;
    }
}


void sdk_manager::set_tcp_client(TcpClient *client) {
    tcp_client = client;
}
