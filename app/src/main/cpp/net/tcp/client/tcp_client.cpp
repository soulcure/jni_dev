
#include "../../../log_util.h"
#include "tcp_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <errno.h>

#define BUFF_MAX 1024*1000
#define BUFF_LENGTH 1024*5


TcpClient::TcpClient() {

}

TcpClient::~TcpClient() {
    if (socketFd != -1) {
        close(socketFd);
    }
}

void TcpClient::OnConnect() {
    if (callback != nullptr) {
        callback(0, "tcp client connect success");
    }
}

void TcpClient::OnDisconnect() {
    if (callback != nullptr) {
        callback(-1, "tcp client connect fail");
    }
}


int TcpClient::Connect(const char *ip, int port, ConnectResult listener) {
    this->m_ip = ip;
    this->m_port = port;
    this->callback = listener;

    struct sockaddr_in sad{};

    LOGT("TCPClient connect\n");
    memset(&sad, 0, sizeof(sockaddr));
    sad.sin_family = AF_INET;  //使用IPV4地址
    sad.sin_port = htons(port); //端口 host to network short
    inet_aton(ip, &sad.sin_addr);

    socketFd = socket(PF_INET, SOCK_STREAM, 0);

#ifdef DEBUG
    printf("Connecting to [%s]:[%d]...\n\n", ip, port);
#endif
    struct timeval time{};
    socklen_t len = 0;
    getsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, &time, &len);

    int result = connect(socketFd, (struct sockaddr *) &sad, sizeof(sad));

    LOGT("TCPClient Running %d\n", result);

    if (result != 0) {
        close(socketFd);
        socketFd = -1;
        OnDisconnect();
        return result;
    }

    std::thread run(&TcpClient::Run, this);// c11 create a thread to run reading.
    run.detach();  //子线程和main thread 完全分离
    OnConnect(); //call callback while connected.
    return 0;

}

int TcpClient::SendProto(const std::vector<char> &msg, int commandId, int seqId) {
    PDUBase pdu_base;
    std::shared_ptr<char> body(new char[msg.size()]);
    pdu_base.body = body;
    pdu_base.length = msg.size();
    pdu_base.command_id = commandId;
    pdu_base.seq_id = seqId;
    Send(pdu_base);
}


int TcpClient::Send(PDUBase &base) {
    if (socketFd < 0)
        return -1;
    //build package.
    char *buf = nullptr;
    int len = OnPduPack(base, buf);
    if (len <= 0)
        return -3;

    int totalLen = 0;
    int resendNum = 0;              //发送失败当前重发次数

    //若缓冲区满引起发送不完全时，需要循环发送直至数据完整
    while (totalLen < len) {
        int write_len = write(socketFd, buf + totalLen, len - totalLen);
        if (write_len <= 0) {
            //...重发
            resendNum++;
            if (resendNum >= ResendNumLimit) {
                //超过重发次数限制后，返回错误
                free(buf);
                return -2;
            }

            usleep(2000);
            continue;
        }

        totalLen += write_len;
    }

    LOGT("TCP Send Data Out.");

    free(buf);
    return totalLen;

}

void TcpClient::OnReceive(PDUBase &base) {

}


void TcpClient::Run() {
    int total_length = 0;
    char *total_buffer = new char[BUFF_MAX];
    char *buf = new char[BUFF_LENGTH];
    PDUBase base;

    while (true) {
        int len = 0;
        memset(buf, 0, BUFF_LENGTH);
        len = read(socketFd, buf, BUFF_LENGTH);

        if (len > 0) {  //读取到数据
            if (total_length + len > BUFF_MAX) {
                total_length = 0;
                continue;
            }
            memcpy(total_buffer + total_length, buf, len);
            total_length += len;

            int read_size;
            while ((read_size = OnPduParse(total_buffer, total_length, base)) > 0) {
                //remove read data.
                memmove(total_buffer, total_buffer + read_size, total_length - read_size);
                total_length -= read_size;
                OnReceive(base);
            }

            printf("Received message!!: %s\n", buf);
            usleep(1);
        } else {
            if (len < 0) {   //len<0 网络错误断开
                char *msg = strerror(errno);
                printf("NetClient Msg:%s\n", msg);
            } else {
                LOGD("NetClient disconnect.\n "); //len=0 对方主动断开
            }
            /*
             * closed
             * finish reading block..
             */
            //后续应考虑重连
            close(socketFd);// need reconnect
            socketFd = -1;
            delete[]total_buffer;
            delete[]buf;
            break;
        }

    }
}






