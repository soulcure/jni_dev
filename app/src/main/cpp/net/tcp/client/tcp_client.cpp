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


TcpClient::TcpClient(const char *ip, int port, OnConnectState state, OnReceive receive)
        : m_ip(ip), m_port(port), m_connect_state(state), m_receive(receive) {

}

TcpClient::~TcpClient() {
    if (socketFd != -1) {
        close(socketFd);
    }
}

void TcpClient::Open() {
    m_exit = false;
    std::thread run(&TcpClient::Connect, this);// c11 create a thread to run reading.
    run.detach();  //子线程和main thread 完全分离
}

void TcpClient::Close() {
    m_exit = true;
    interrupt.notify_one();

    close(socketFd);
    socketFd = -1;
}


void TcpClient::SendProto(const std::vector<char> &msg, int commandId, int seqId) {
    PDUBase pdu_base;
    std::shared_ptr<char> body(new char[msg.size()]);
    pdu_base.body = body;
    pdu_base.length = msg.size();
    pdu_base.command_id = commandId;
    pdu_base.seq_id = seqId;
    Send(pdu_base);
}


void TcpClient::Send(PDUBase &base) {
    //std::lock_guard<std::mutex> guard(mtx);
    std::unique_lock<std::mutex> lock(mtx);
    m_queue.push(base);
    interrupt.notify_one();
}


void TcpClient::Connect() {
    struct sockaddr_in sad{};

    LOGT("TCPClient connect\n");
    memset(&sad, 0, sizeof(sockaddr));
    sad.sin_family = AF_INET;  //使用IPV4地址
    sad.sin_port = htons(m_port); //端口 host to network short
    inet_aton(m_ip, &sad.sin_addr);

    socketFd = socket(PF_INET, SOCK_STREAM, 0);

#ifdef DEBUG
    printf("Connecting to [%s]:[%d]...\n\n", m_ip, m_port);
#endif
    struct timeval time{};
    socklen_t len = 0;
    getsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, &time, &len);

    int result = connect(socketFd, (struct sockaddr *) &sad, sizeof(sad));

    LOGT("TCPClient Running %d\n", result);

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
    while (m_exit) {
        while (socketFd >= 0) {
            std::unique_lock<std::mutex> lock(mtx);
            interrupt.wait(lock, [this] { return !m_queue.empty(); });

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
                if (write_len <= 0) {
                    OnDisconnect();
                    return;
                }
                totalLen += write_len;
            }
            LOGT("TCP Send Data Out.\n");
            free(buf);
        }

        LOGT("TCP Send Thread Wait.\n");
        std::unique_lock<std::mutex> lck(mtx);
        interrupt.wait(lck);
    }


}


void TcpClient::OnConnect() {
    if (m_connect_state != nullptr) {
        m_connect_state(0);
    }
}

void TcpClient::OnDisconnect() {
    if (m_connect_state != nullptr) {
        m_connect_state(-1);
    }
}

void TcpClient::OnReceiver(PDUBase &base) {
    if (m_receive != nullptr) {
        m_receive(base);
    }
}






