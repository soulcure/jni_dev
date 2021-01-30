#include "tcp_server.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>


TcpServer::TcpServer() {
    m_fd = -1;
}

TcpServer::~TcpServer() {
    Close();
}

bool TcpServer::SetBlock(int fd) {
    int flag = fcntl(fd, F_GETFL);
    flag = flag & (~O_NONBLOCK);
    return fcntl(fd, F_SETFL, flag) >= 0;
}

bool TcpServer::SetNonBlock(int fd) {
    int flag = fcntl(fd, F_GETFL);
    flag = flag | O_NONBLOCK;
    return fcntl(fd, F_SETFL, flag) >= 0;
}

bool TcpServer::SetNoDelay(int fd) {
    long bNodelay = 1;
    int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &bNodelay, sizeof(bNodelay));
    return ret >= 0;

}

void TcpServer::SetReuse(int sock) {
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt));
}

void TcpServer::SetLinger(int sock) {
    struct linger ling;
    ling.l_onoff = 1;
    ling.l_linger = 0;
    setsockopt(sock, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
}

int TcpServer::CreateSocket(int type) {
    int nSocket = socket(AF_INET, type, 0);
    if (nSocket < 0) {
        m_nSocketErr = SOCKET_INIT_ERROR;
        return -1;
    }
    return nSocket;
}

bool TcpServer::BindSocket(int sock, char *pAddr, int port) {
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;

    //  sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_addr.sin_addr.s_addr = inet_addr(pAddr);

    sock_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
        m_nSocketErr = SOCKET_BIND_ERROR;
        return false;
    }

    return true;
}

bool TcpServer::ConnectSocket(int sock, const char *pAddr, int port) {
    struct sockaddr_in sock_addr;

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(pAddr);
    sock_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
        m_nSocketErr = SOCKET_CONNECT_ERROR;
        printf("connect failed[%d],[%s]\n", errno, strerror(errno));
        return false;
    }
    return true;
}

bool TcpServer::ListenSocket(int sock, int conn_num) {
    if (listen(sock, conn_num) < 0) {
        m_nSocketErr = SOCKET_LISTEN_ERROR;
        return false;
    }
    return true;
}

int TcpServer::AcceptSocket(int sock, sockaddr_in &remote_addr) {
#ifdef _IBM_
    unsigned int addr_size = (int)sizeof(remote_addr);
#elif HP_UX
    int addr_size = sizeof(remote_addr);
#else
    socklen_t addr_size = (socklen_t) sizeof(remote_addr);
#endif
    int nConnSocket = accept(sock, (struct sockaddr *) &remote_addr, &addr_size);
    if (nConnSocket < 0) {
        m_nSocketErr = SOCKET_ACCEPT_ERROR;
    }

    return nConnSocket;
}

int TcpServer::SendMsg(int sock, char *buf, int buf_len) {
    int byte_send = send(sock, buf, buf_len, 0);
    if (byte_send < 0) {
        m_nSocketErr = SOCKET_TRANSMIT_ERROR;
    }

    return byte_send;
}

int TcpServer::SendToMsg(int sock, char *pAddr, int port, char *buf, int buf_len) {
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(pAddr);
    sock_addr.sin_port = htons(port);

    int byte_send = sendto(sock, buf, buf_len, 0, (struct sockaddr *) &sock_addr,
                           sizeof(sock_addr));
    if (byte_send < 0) {
        m_nSocketErr = SOCKET_TRANSMIT_ERROR;
    }
    return byte_send;
}

void TcpServer::Close() {
    if (m_fd > 0) {
        close(m_fd);
        m_fd = -1;
    }

}


