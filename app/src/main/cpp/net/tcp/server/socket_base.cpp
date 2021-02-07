#include "socket_base.h"
#include <unistd.h>
#include <cerrno>
#include <string>


CSocketBase::CSocketBase() {
    m_fd = -1;
}

CSocketBase::~CSocketBase() {
    Close();
}

bool CSocketBase::SetBlock(int sockFd) {
    int flag = fcntl(sockFd, F_GETFL);
    flag = flag & (~O_NONBLOCK);
    return fcntl(sockFd, F_SETFL, flag) >= 0;
}

bool CSocketBase::SetNonBlock(int sockFd) {
    int flag = fcntl(sockFd, F_GETFL);
    flag = flag | O_NONBLOCK;
    return fcntl(sockFd, F_SETFL, flag) >= 0;
}

bool CSocketBase::SetNoDelay(int sockFd) {
    long noDelay = 1;
    int ret = setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, (char *) &noDelay, sizeof(noDelay));
    return ret >= 0;

}

void CSocketBase::SetReuse(int sockFd) {
    int opt = 1;
    setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt));
}

void CSocketBase::SetLinger(int sockFd) {
    struct linger ling{};
    ling.l_onoff = 1;
    ling.l_linger = 0;
    setsockopt(sockFd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
}

int CSocketBase::CreateSocket(int type) {
    int nSocket = socket(AF_INET, type, 0);
    if (nSocket < 0) {
        m_nSocketErr = SOCKET_INIT_ERROR;
        return -1;
    }
    return nSocket;
}

bool CSocketBase::BindSocket(int sock, const char *ip, int port) {
    struct sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    //socket_address.sin_addr.s_addr = inet_addr(pAddr);  //将一个点间隔地址 转换整数形式的IP地址
    inet_pton(AF_INET, ip, &socket_address.sin_addr);//将一个点间隔地址 转换整数形式的IP地址

    socket_address.sin_port = htons(port);

    if (::bind(sock, (struct sockaddr *) &socket_address, sizeof(socket_address)) < 0) {
        m_nSocketErr = SOCKET_BIND_ERROR;
        return false;
    }

    return true;
}

bool CSocketBase::ConnectSocket(int sock, const char *ip, int port) {
    struct sockaddr_in socket_address{};

    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = inet_addr(ip);
    socket_address.sin_port = htons(port);

    if (::connect(sock, (struct sockaddr *) &socket_address, sizeof(socket_address)) < 0) {
        m_nSocketErr = SOCKET_CONNECT_ERROR;
        printf("connect failed[%d],[%s]\n", errno, strerror(errno));
        return false;
    }
    return true;
}

bool CSocketBase::ListenSocket(int sock, int conn_num) {
    if (::listen(sock, conn_num) < 0) {
        m_nSocketErr = SOCKET_LISTEN_ERROR;
        return false;
    }
    return true;
}

int CSocketBase::AcceptSocket(int sock, sockaddr_in &remote_address) {
    auto size = (socklen_t) sizeof(remote_address);
    int nConnSocket = ::accept(sock, (struct sockaddr *) &remote_address, &size);
    if (nConnSocket < 0) {
        m_nSocketErr = SOCKET_ACCEPT_ERROR;
    }

    return nConnSocket;
}

int CSocketBase::SendMsg(int sockId, char *buf, int buf_len) {
    int byte_send = ::send(sockId, buf, buf_len, 0);
    if (byte_send < 0) {
        m_nSocketErr = SOCKET_TRANSMIT_ERROR;
    }

    return byte_send;
}

int CSocketBase::SendToMsg(int sock, char *ip, int port, char *buf, int buf_len) {
    struct sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = inet_addr(ip);
    socket_address.sin_port = htons(port);

    int byte_send = ::sendto(sock, buf, buf_len, 0, (struct sockaddr *) &socket_address,
                             sizeof(socket_address));
    if (byte_send < 0) {
        m_nSocketErr = SOCKET_TRANSMIT_ERROR;
    }
    return byte_send;
}

void CSocketBase::Close() {
    if (m_fd > 0) {
        ::close(m_fd);
        m_fd = -1;
    }

}


