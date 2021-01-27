#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "../../pdu_base.h"
#include "../../pdu_util.h"
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>    // std::condition_variable

class TcpClient : public PduUtil {
public:
    typedef void(*OnConnectState)(const int, std::string &);

    typedef void (*OnReceive)(PDUBase &);

    TcpClient(const char *ip, int port, OnConnectState state, OnReceive receive);

    ~TcpClient();

    void Open();

    void Close();

    void Send(PDUBase &base);

    void SendProto(const std::vector<char> &msg, int commandId, int seqId);


private:
    int m_port;
    const char *m_ip;
    const int ResendNumLimit = 3;   //发送失败重发次数限制
    int socketFd;
    bool m_exit;
    std::queue<PDUBase> m_queue;   // 全局消息队列
    std::mutex mtx;        // 全局互斥锁
    std::condition_variable interrupt;

    OnConnectState m_connect_state;
    OnReceive m_receive;

    void OnConnect();

    void OnDisconnect();

    void OnReceiver(PDUBase &base);

    void Connect();

    void ReceiveThread();

    void SendThread();
};

#endif // TCP_CLIENT_H

