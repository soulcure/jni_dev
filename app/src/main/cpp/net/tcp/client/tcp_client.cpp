#include "../../../log/log_util.h"
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

#define BUFF_MAX 1024*512   //512KB
#define BUFF_LENGTH 1024*5  //5KB


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
    LOGD("TcpClient Open [%s]:[%d]", ip, port);
    //std::thread run(&TcpClient::connectTcp, this);// c11 create a thread to run reading.
    //run.detach();  //子线程和main thread 完全分离

    std::thread t(std::mem_fn(&TcpClient::connectTcp), this);//使用类的成员函数作为线程参数
    t.detach();   //与当前线程分离
}

void TcpClient::SetConnectStateListener(OnConnectState state) {
    m_connect_state = state;
}

void TcpClient::SetReceiveListener(OnReceive state) {
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
    LOGD("PDUBase send body length:[%d]", size);

    //std::shared_ptr<char> body(new char[size]);
    std::shared_ptr<char> body = std::make_shared<char>(size);

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


void TcpClient::connectTcp() {
    LOGD("TCPClient Connecting to [%s]:[%d]", m_ip, m_port);

    //参数 AF_INET 表示使用 IPv4 地址,SOCK_STREAM 表示使用面向连接的套接字，IPPROTO_TCP 表示使用 TCP 协议
    socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //ipv4,TCP数据连接
    if (socketFd < 0) {
        LOGE("create socketFd error...");
        onDisconnect();
        return;
    }

    struct sockaddr_in sockAddress{};  //T object {} since C++11  Value initialization
    memset(&sockAddress, 0, sizeof(sockAddress));  //T object {} no need zero set
    sockAddress.sin_family = AF_INET;  //使用IPV4地址
    sockAddress.sin_port = htons(m_port); //端口 host to network short
    if (inet_pton(AF_INET, m_ip, &sockAddress.sin_addr) <= 0) { //字符串IP地址 转化为int 32网络序列IP地址
        LOGE("address ip error for [%s]", m_ip);
        onDisconnect();
        return;
    }

    struct timeval time{};
    socklen_t len = 0;
    getsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, &time, &len);  //设置socket option

    int result = connect(socketFd, (struct sockaddr *) &sockAddress, sizeof(sockAddress));
    LOGD("TCPClient Connect result=%d", result);
    if (result != 0) {
        Close();
        onDisconnect();
        return;
    }

    onConnect(); //call callback while connected.

    //std::thread run(&TcpClient::sendThread, this);// c11 create a thread to run reading. 此写法不支持传参
    //run.detach();  //与当前线程分离

    std::thread t(std::mem_fn(&TcpClient::sendThread), this);//使用类的成员函数作为线程参数
    t.detach();   //与当前线程分离

    receiveThread();
}


void TcpClient::receiveThread() {
    int total_length = 0;
    char *total_buffer = new char[BUFF_MAX];
    char *buf = new char[BUFF_LENGTH];
    PDUBase base;

    while (true) {
        int len;
        memset(buf, 0, BUFF_LENGTH);
        LOGD("TCPClient ReceiveThread start...");

        len = read(socketFd, buf, BUFF_LENGTH);

        LOGD("TCPClient Receive size=[%d]", len);
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
                onReceiver(base);
            }

            usleep(1);
        } else {
            if (len < 0) {   //len<0 网络错误断开
                char *msg = strerror(errno);
                LOGE("TCPClient Receive Msg:%s", msg);
            } else {
                LOGD("TCPClient Receive disconnect"); //len=0 对方主动断开
            }
            Close();
            delete[]total_buffer;
            delete[]buf;
            break;
        }
    }
}


void TcpClient::sendThread() {
    LOGD("TCPClient SendThread Start...$exit:[%d]", m_exit);

    while (!m_exit && socketFd >= 0) {
        LOGD("TCPClient SendThread start socketFd:[%d]", socketFd);

        std::unique_lock<std::mutex> lock(mtx);

        LOGD("TCPClient SendThread wait...");
        interrupt.wait(lock, [this] { return !m_queue.empty(); });

        LOGD("TCPClient SendThread wake up and m_queue pop...");
        PDUBase base = m_queue.front();
        m_queue.pop();

        char *buf = nullptr;
        int len = OnPduPack(base, buf);

        LOGD("TCPClient SendThread send pdu buffer length:[%d]", len);
        if (len <= 0) {
            onDisconnect();
            return;
        }

        int totalLen = 0;
        //若缓冲区满引起发送不完全时，需要循环发送直至数据完整
        while (totalLen < len) {
            int write_len = write(socketFd, buf + totalLen, len - totalLen);
            LOGD("TCPClient SendThread write_len:[%d]", write_len);
            if (write_len <= 0) {
                free(buf);
                onDisconnect();
                return;
            }
            totalLen += write_len;
        }
        free(buf);
        LOGD("TCP Send Data Out");
    }

    LOGE("TCPClient SendThread exit...");

}


void TcpClient::onConnect() {
    if (m_connect_state != nullptr) {
        std::string message = "tcp connect success";
        m_connect_state(0, message);
    }
}

void TcpClient::onDisconnect() {
    if (m_connect_state != nullptr) {
        std::string message = "tcp connect fail";
        m_connect_state(-1, message);
    }
}

void TcpClient::onReceiver(PDUBase &base) {
    char *buffer = new char[base.length + 1];
    memcpy(buffer, base.body.get(), base.length);
    buffer[base.length] = 0;
    LOGD("TCPClient Receive message=%s", buffer);
    delete[] buffer;

    if (m_receive != nullptr) {
        m_receive(base);
    }
}






