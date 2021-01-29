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


TcpClient::TcpClient(OnConnectState state, OnReceive receive)
        : m_connect_state(state), m_receive(receive) {

}

TcpClient::~TcpClient() {
    if (socketFd != -1) {
        close(socketFd);
    }
}

void TcpClient::Open(const char *ip, int port) {
    m_exit = false;
    m_ip = ip;
    m_port = port;
    LOGD("TcpClient Open [%s]:[%d]...", ip, port);
    std::thread run(&TcpClient::Connect, this);// c11 create a thread to run reading.
    run.detach();  //子线程和main thread 完全分离
}

void TcpClient::setConnectStateListener(OnConnectState state) {
    m_connect_state = state;
}

void TcpClient::setReceiveListener(OnReceive state) {
    m_receive = state;
}


void TcpClient::Close() {
    m_exit = true;
    interrupt.notify_one();

    close(socketFd);
    socketFd = -1;
}


void TcpClient::SendProto(char pdu_type, const char *message) {
    PDUBase pdu_base;

    int size = strlen(message);
    LOGD("SendProto message size:%d", size);

    std::shared_ptr<char> body(new char[size]);
    memcpy(body.get(), message, size);

    pdu_base.pdu_type = pdu_type;
    pdu_base.length = size;
    pdu_base.body = body;

    LOGD("TCPClient SendProto pdu_base pdu_type:[%d] length:[%d]", pdu_type, size);

    Send(pdu_base);
}


void TcpClient::Send(PDUBase &base) {
    //std::lock_guard<std::mutex> guard(mtx);
    std::unique_lock<std::mutex> lock(mtx);
    m_queue.push(base);
    interrupt.notify_one();
    LOGD("TCPClient push PDUBase to send queue");
}


void TcpClient::Connect() {
    LOGD("TCPClient Connecting to [%s]:[%d]", m_ip, m_port);

    socketFd = socket(AF_INET, SOCK_STREAM, 0); //ipv4,TCP数据连接
    if (socketFd < 0) {
        LOGE("create socketFd error...");
        OnDisconnect();
        return;
    }

    struct sockaddr_in server_address{};
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;  //使用IPV4地址
    server_address.sin_port = htons(m_port); //端口 host to network short
    inet_aton(m_ip, &server_address.sin_addr);

    if (inet_pton(AF_INET, m_ip, &server_address.sin_addr) <= 0) { //设置ip地址
        LOGE("address ip error for [%s]", m_ip);
        OnDisconnect();
        return;
    }

    struct timeval time{};
    socklen_t len = 0;
    getsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, &time, &len);  //设置socket option

    int result = connect(socketFd, (struct sockaddr *) &server_address, sizeof(server_address));
    LOGD("TCPClient Connect result=%d", result);
    if (result != 0) {
        Close();
        OnDisconnect();
        return;
    }

    OnConnect(); //call callback while connected.

    std::thread run(&TcpClient::SendThread, this);// c11 create a thread to run reading.
    run.detach();  //子线程和main thread 完全分离

    ReceiveThread();
}


void TcpClient::ReceiveThread() {
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
                OnReceiver(base);
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
            Close();
            delete[]total_buffer;
            delete[]buf;
            break;
        }

    }
}


void TcpClient::SendThread() {
    LOGD("TCPClient SendThread exit:[%d]", m_exit);

    while (!m_exit && socketFd >= 0) {
        LOGD("TCPClient SendThread start socketFd:[%d]", socketFd);

        std::unique_lock<std::mutex> lock(mtx);

        LOGD("TCPClient SendThread wait");
        interrupt.wait(lock, [this] { return !m_queue.empty(); });

        LOGD("TCPClient SendThread m_queue pop");
        PDUBase base = m_queue.front();
        m_queue.pop();

        char *buf = nullptr;
        int len = OnPduPack(base, buf);
        if (len <= 0) {
            OnDisconnect();
            return;
        }
        int totalLen = 0;
        //若缓冲区满引起发送不完全时，需要循环发送直至数据完整
        while (totalLen < len) {
            int write_len = write(socketFd, buf + totalLen, len - totalLen);
            LOGD("TCPClient SendThread write_len:[%d]", write_len);
            if (write_len <= 0) {
                OnDisconnect();
                return;
            }
            totalLen += write_len;
        }
        LOGD("TCP Send Data Out");
        free(buf);
    }
    LOGD("TCPClient SendThread exit...");

}


void TcpClient::OnConnect() {
    if (m_connect_state != nullptr) {
        std::string message = "tcp connect success";
        m_connect_state(0, message);
    }
}

void TcpClient::OnDisconnect() {
    if (m_connect_state != nullptr) {
        std::string message = "tcp connect fail";
        m_connect_state(-1, message);
    }
}

void TcpClient::OnReceiver(PDUBase &base) {
    if (m_receive != nullptr) {
        m_receive(base);
    }
}






