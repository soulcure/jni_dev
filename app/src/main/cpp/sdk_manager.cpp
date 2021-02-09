//
// Created by chenqiongyao on 2021/1/28.
//

#include "sdk_manager.h"
#include "JniHelper.h"

sdk_manager::sdk_manager() {
    LOGD("sdk_manager constructor");
    onConnectStateListener = [](int code, std::string &message) {
        LOGD("tcp client connect result code:%d message:%s", code, message.c_str());
    };
    onReceiveListener = [](PDUBase &pdu) {
        const char *buf = pdu.body.get();
        int len = pdu.length;
        LOGD("ConReceivePdu buffer size:[%d]", len);
        JniHelper::ConReceivePdu(buf, len);
    };
    tcp_client = new TcpClient(onConnectStateListener, onReceiveListener);
}

sdk_manager::~sdk_manager() {
    LOGD("sdk_manager destructor");
    delete tcp_client;
}


void sdk_manager::create_tcp_client(const char *ip, int port) {
    LOGD("TcpClient to [%s]:[%d]", ip, port);
    tcp_client->Open(ip, port);
}

void sdk_manager::send_message(int length, const char *message) {
    LOGD("send_message:[%s]", message);
    tcp_client->SendProto(PDUBase::LOCAL_STRING, length, message);

}


void sdk_manager::send_bytes(int length, const char *message) {
    LOGD("send_bytes:[%s]", message);
    tcp_client->SendProto(PDUBase::LOCAL_BYTES, length, message);
}



