#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/time.h>

#define MAX_CLIENT_CON   64    //最大客户端连接数

typedef enum {     // IvrSocket exception codes
    SOCKET_NO_ERROR = 0,         // No socket errors reported
    SOCKET_INVALID_ERROR_CODE,   // Invalid socket error code

    SOCKET_ACCEPT_ERROR,         // Error accepting remote socket
    SOCKET_BIND_ERROR,           // Could not bind socket
    SOCKET_BUFOVER_ERROR,        // Buffer overflow
    SOCKET_CONNECT_ERROR,        // Could not connect socket
    SOCKET_FILESYSTEM_ERROR,     // A file system error occurred
    SOCKET_GETOPTION_ERROR,      // Error getting socket option
    SOCKET_HOSTNAME_ERROR,       // Could not resolve hostname
    SOCKET_INIT_ERROR,           // Initialization error
    SOCKET_LISTEN_ERROR,         // Listen error
    SOCKET_PEERNAME_ERROR,       // Get peer name error
    SOCKET_PROTOCOL_ERROR,       // Unknown protocol requested
    SOCKET_RECEIVE_ERROR,        // Receive error
    SOCKET_REQUEST_TIMEOUT,      // Request timed out
    SOCKET_SERVICE_ERROR,        // Unknown service requested
    SOCKET_SETOPTION_ERROR,      // Error setting socket option
    SOCKET_SOCKNAME_ERROR,       // Get socket name error
    SOCKET_SOCKETTYPE_ERROR,     // Unknown socket type requested
    SOCKET_TRANSMIT_ERROR,       // Transmit error
} ESocketError;

typedef struct {
    int fd;
    char name[20];
    struct sockaddr_in addr; /* client's address information */
} CLIENT;

class TcpServer {
public:
    TcpServer();

    virtual ~TcpServer();

    bool SetBlock(int fd);

    bool SetNonBlock(int fd);

    bool SetNoDelay(int fd);

    void SetReuse(int sock);

    void SetLinger(int sock);

    int CreateSocket(int type = SOCK_STREAM);

    bool BindSocket(int sock, char *pAddr, int port);

    bool ConnectSocket(int sock, const char *pAddr, int port);

    bool ListenSocket(int sock, int conn_num);

    int AcceptSocket(int sock, sockaddr_in &remote_addr);

    int SendMsg(int sock, char *buf, int buf_len);

    int SendToMsg(int sock, char *pAddr, int port, char *buf, int buf_len);

    void Close();

private:
    int m_nSocketErr;
    int m_fd;
};


#endif  //TCP_SERVER_H


