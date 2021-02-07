#ifndef NET_BASE_H
#define NET_BASE_H

#include "../../../log/log_util.h"
#include "../../pdu_base.h"
#include "../../pdu_util.h"
#include "socket_base.h"

#include <string>
#include <unordered_map>
#include <memory>   //shared_ptr
#include <mutex>

#define default_port 8080

#define TCP_MAX_BUFF 51200

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
    * @允许个性化的初始化处理
    *
    */
    virtual void Init();

    /***********************************************************
    * @接收数据回调处理函数
    *
    */
    virtual void OnRecv(int sockId, PDUBase &_pack) = 0;

    /***********************************************************
    * @接收请求时回调处理函数
    *
    */
    virtual void OnConn(const char *ip, short port);

    /***********************************************************
    * @接收请求时回调处理函数
    *
    */
    virtual void OnDisconn(int sockId);

    /***********************************************************
    * @Epoll超时回调处理函数
    *
    */
    virtual void OnTimeOut();


    /***********************************************************
    * @ 监听套接字初始化处理
    *
    * @param
    */
    int ProcessListenSock();

    /***********************************************************
    * @ 调用Epoll接口函数
    *
    * @param
    */
    void ProcessEpoll(int sockId);

    /***********************************************************
    * @ 服务器启动
    *
    * @param
    */
    void StartServer(const std::string& ip, int port = default_port);

    /*
     *发送
     *封装了封单包的方法。
     */
    bool Send(int fd, PDUBase &data);

    bool Send(int fd, char *buff, int len);


    /**********************************************************
     * this is app level buffer for tcp.
     * if a connect named A , send buffer 100Byte to server.
     * server recv 50Byte, other 50Byte did not recv due to network question
     * mean while, another connection named B send 100Byte to server.
     * server recv 100Byte.
     */
    std::unordered_map<int, ConnectBuffer> recv_buffers;

protected:
    CSocketBase m_Sock;

    int listen_num;        //监听的SOCKET数量
    std::string m_ip;
    int m_port;            //服务端监听端口

    std::mutex send_mutex;
};

#endif  //NET_BASE_H
