#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "../../pdu_base.h"
#include "../../pdu_util.h"
#include <string>
#include <vector>

class TcpClient : public PduUtil {
public:
    typedef void(*ConnectResult)(int, std::string);

    TcpClient();

    ~TcpClient();

    int Send(PDUBase &base);

    int SendProto(const std::vector<char> &msg, int commandId, int seqId);

    int Connect(const char *ip, int port, ConnectResult listener);

    virtual void OnReceive(PDUBase &base) = 0;

    virtual void OnConnect();

    virtual void OnDisconnect();

    /***************************************
     * this is a thread exec.
     * similar to java.
     */
    void Run();

private:
    const int ResendNumLimit = 3;   //发送失败重发次数限制
    int socketFd;
    ConnectResult callback;

protected:
    int m_port;
    std::string m_ip;

};

#endif // TCP_CLIENT_H

