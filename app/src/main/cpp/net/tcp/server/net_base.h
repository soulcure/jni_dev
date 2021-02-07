#ifndef NET_BASE_H
#define NET_BASE_H

#include <string>
#include <unordered_map>
#include <memory>   //shared_ptr
#include <mutex>

#include "../../../log/log_util.h"
#include "../../pdu_base.h"
#include "../../pdu_util.h"
#include "socket_base.h"


#define default_port 24000


class ConnectBuffer {
public:
    std::shared_ptr<char> body;
    int length;

    ConnectBuffer() {
        length = 0;
    }
};


class NetBase : public PduUtil {
public:
    /***********************************************************
    * @ 构造函数.
    *
    * @param bEt 默认是ET模式，当状态发生变化的时候才获得通知
    */
    NetBase();

    /***********************************************************
    * @析构函数
    *
    */
    virtual ~NetBase();

    /***********************************************************
    * @接收数据回调处理函数
    *
    */
    virtual void OnReceive(int sockFd, PDUBase &pack) = 0;

    /***********************************************************
    * @接收请求时回调处理函数
    *
    */
    virtual void OnConnect(const char *ip, short port);

    /***********************************************************
    * @接收请求时回调处理函数
    *
    */
    virtual void OnDisconnect(int sockFd);

    /***********************************************************
    * @Epoll超时回调处理函数
    *
    */
    virtual void OnTimeOut();

    /***********************************************************
    * @ 服务器启动
    *
    * @param
    */
    void StartServer(const std::string &ip, int port = default_port);

    /***********************************************************
    * @ 监听套接字初始化处理
    *
    * @param
    */
    int createListenSocket();

    /***********************************************************
    * @ 调用Epoll接口函数
    *
    * @param
    */
    void addToEpoll(int sockFd);

    /*
     *发送
     *封装了封单包的方法。
     */
    bool Send(int sockFd, PDUBase &data);

    static bool Send(int sockFd, char *buff, int len);


    /**********************************************************
     * this is app level buffer for tcp.
     * if a connect named A , send buffer 100Byte to server.
     * server receive 50Byte, other 50Byte did not receive due to network question
     * mean while, another connection named B send 100Byte to server.
     * server receive 100Byte.
     */
    std::unordered_map<int, ConnectBuffer> receiveMapBuffers;

protected:
    CSocketBase m_Sock;

    int listen_num;        //监听的SOCKET数量
    std::string m_ip;
    int m_port;            //服务端监听端口

    std::mutex send_mutex;
};

#endif  //NET_BASE_H
