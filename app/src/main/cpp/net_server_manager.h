//
// Created by chenqiongyao on 2021/1/28.
//

#ifndef NET_SERVER_MANAGER_H
#define NET_SERVER_MANAGER_H

#include "tcp_client.h"
#include "log_util.h"
#include "pdu_base.h"
#include "net_base.h"

class net_server_manager {
public:


    void create_tcp_server(int port);

    void Open(int port);

    static net_server_manager &Get();


private:
    net_server_manager(const net_server_manager &) = delete;

    net_server_manager operator=(const net_server_manager &) = delete;

    net_server_manager operator=(const net_server_manager &&) = delete;

    net_server_manager();

    ~net_server_manager();

    NetBase *tcp_server;


};


#endif //NET_SERVER_MANAGER_H
