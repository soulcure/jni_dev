#ifndef __SocketBase_H__
#define __SocketBase_H__

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/time.h>


class CSocketBase {
public:
    CSocketBase();

    virtual ~CSocketBase();

    void Close();

    static int CreateSocket(int type = SOCK_STREAM);

    static bool BindSocket(int sock, const char *ip, int port);

    static bool ConnectSocket(int sock, const char *ip, int port);

    static bool ListenSocket(int sock, int conn_num);

    static int AcceptSocket(int sock, sockaddr_in &remote_address);

    static int SendMsg(int sock, char *buf, int buf_len);

    static int SendToMsg(int sock, char *ip, int port, char *buf, int buf_len);

    static bool SetBlock(int sockFd);

    static bool SetNonBlock(int sockFd);

    static bool SetNoDelay(int sockFd);

    static void SetReuse(int sockFd);

    static void SetLinger(int sockFd);

private:
    int m_fd;


};


#endif


