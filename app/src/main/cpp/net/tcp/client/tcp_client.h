#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>    // std::condition_variable
#include <functional>   // std::mem_fn

#include "../../pdu_base.h"
#include "../../pdu_util.h"

class TcpClient : public PduUtil {
public:
    typedef void(*OnConnectState)(const int, std::string &);

    typedef void (*OnReceive)(PDUBase &);

    TcpClient(OnConnectState state, OnReceive receive);

    ~TcpClient();

    void Open(const std::string& ip, int port);

    void SetConnectStateListener(OnConnectState state);

    void SetReceiveListener(OnReceive state);

    void Close();

    void Send(PDUBase &base);

    void SendProto(char pdu_type, int length, const char *message);

private:
    int m_port{}; //0  since C++11
    std::string m_ip;
    int socketFd{};
    bool m_exit{};
    std::queue<PDUBase> m_queue;   // 全局消息队列
    std::mutex mtx;        // 全局互斥锁
    std::condition_variable interrupt;

    OnConnectState m_connect_state;
    OnReceive m_receive;

    void onConnect();

    void onDisconnect();

    void onReceiver(PDUBase &base);

    void connectTcp();

    void receiveThread();

    void sendThread();
};

#endif // TCP_CLIENT_H

