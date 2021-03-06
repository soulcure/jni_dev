#include "net_base.h"
#include "socket_base.h"
#include "poll.h"
#include "log_util.h"

extern NetBase *ptrNetBase;

using namespace std;


void SetObj(NetBase *pObj) {
    ptrNetBase = pObj;
}

void SignalHandle() {
    signal(SIGPIPE, SIG_IGN);
}


void read_cb(NetBase *obj, poll_event_t *poll_event, poll_event_element_t *node,
             struct epoll_event ev) {
    char subBuf[1024] = {0};
    char *buf = subBuf;

    int val = read(node->fd, buf, 1024);

    if (val > 0) {
        buf[val] = '\0';
        LOGD(" received data -> %s", buf);

        PDUBase pdu;
        auto it = obj->receiveMapBuffers.find(node->fd);

        if (obj->receiveMapBuffers.end() != it) {
            if (it->second.length >= 0) {
                std::shared_ptr<char> mergeBuf(new char[it->second.length + val + 1]);
                memcpy(mergeBuf.get(), it->second.body.get(), it->second.length);
                memcpy(mergeBuf.get() + it->second.length, buf, val);
                buf = mergeBuf.get();
                val += it->second.length;
                it->second.length = val;
                it->second.body = mergeBuf;
            }
        } else {
            std::shared_ptr<char> newBuf(new char[val + 1]);
            memcpy(newBuf.get(), buf, val);
            ConnectBuffer connect_buffer;
            connect_buffer.body = newBuf;
            connect_buffer.length = val;
            obj->receiveMapBuffers.insert({node->fd, connect_buffer});
            it = obj->receiveMapBuffers.find(node->fd);
        }

        int result = 0;
        while ((result = obj->OnPduParse(buf, val, pdu)) != 0) {
            if (result < 0) {
                obj->receiveMapBuffers.erase(node->fd);
                break;
            } else if (result > 0) {
                //read some data(a full package, remove those buffers)
                memmove(it->second.body.get(), it->second.body.get() + result,
                        it->second.length - result);

                it->second.length -= result;
                val -= result;
                //call callback.
                obj->OnReceive(node->fd, pdu);
                buf = it->second.body.get();
            }
        }
    }
}


void close_cb(NetBase *obj, poll_event_t *poll_event, poll_event_element_t *node,
              struct epoll_event ev) {
    // close the socket, we are done with it
    poll_event_remove(poll_event, node->fd);
    obj->receiveMapBuffers.erase(node->fd);
    //
    close(node->fd);
    LOGD("close_cb socket:%d", node->fd);
    obj->OnDisconnect(node->fd);
}

void accept_cb(NetBase *obj, poll_event_t *poll_event, poll_event_element_t *node,
               struct epoll_event ev) {
    // accept the connection
    struct sockaddr_in socket_address{};
    socklen_t clt_len = sizeof(socket_address);
    int listenFd = accept(node->fd, (struct sockaddr *) &socket_address, &clt_len);
    fcntl(listenFd, F_SETFL, O_NONBLOCK);
    LOGD("got the socket: [%d]", listenFd);
    // set flags to check
    uint32_t flags = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
    poll_event_element_t *p;
    // add file descriptor to poll event
    poll_event_add(poll_event, listenFd, flags, &p);
    // set function callbacks
    p->read_callback = read_cb;
    p->close_callback = close_cb;

    short port = ntohs(socket_address.sin_port);
    char ip[20] = {0}; // 存放点分十进制IP地址
    inet_ntop(AF_INET, (void *) &(socket_address.sin_addr.s_addr), ip, 16);

    obj->OnConnect(ip, port);
}

//time out function
int timeout_cb(NetBase *obj, poll_event_t *poll_event) {
    // just keep a count
    if (!poll_event->data) {
        // no count initialised, then initialize it
        poll_event->data = calloc(1, sizeof(int));
    } else {
        // increment and print the count
        int *value = (int *) poll_event->data;
        *value += 1;
        LOGD("time out number [%d]", *value);
    }
    //创建epoll超时
    obj->OnTimeOut();
    return 0;
}


NetBase::NetBase() {
    m_ip = "127.0.0.1";
    m_port = default_port;
}

NetBase::~NetBase() {
    m_Sock.Close();
}

int NetBase::createListenSocket() {
    SignalHandle();
    int sockFd = CSocketBase::CreateSocket();
    if (sockFd > 0) {
        SetObj(this);
        //SOCKET在CLOSE时候是否等待缓冲区发送完成
        CSocketBase::SetLinger(sockFd);

        //非阻塞模式下调用accept()函数立即返回
        CSocketBase::SetNonBlock(sockFd);

        //SO_REUSEADDR是让端口释放后立即就可以被再次使用
        CSocketBase::SetReuse(sockFd);

        CSocketBase::BindSocket(sockFd, m_ip.c_str(), m_port);

        //10 ,backlog 为请求队列的最大长度。
        CSocketBase::ListenSocket(sockFd, 10);
    }

    return sockFd;
}

void NetBase::StartServer(int port) {
    m_port = port;

    LOGD("StartServer ip:[%s], port:[%d]", m_ip.c_str(), port);
    int sockFd = createListenSocket();
    if (sockFd < 1) {
        return;
    }
    LOGD("addToEpoll sockFd:[%d]",sockFd);
    addToEpoll(sockFd);
}


void NetBase::addToEpoll(int sockFd) {
    // create a poll event object, with time out of 1 sec
    poll_event_t *pe = poll_event_new(1000);
    if (!pe) {
        LOGE("create a poll event object is null");
        return;
    }
    // set timeout callback
    pe->timeout_callback = timeout_cb;

    poll_event_element_t *p;
    // add sock to poll event
    poll_event_add(pe, sockFd, EPOLLIN, &p);
    // set callbacks
    //p->read_callback = read_cb;
    p->accept_callback = accept_cb;
    p->close_callback = close_cb;
    // enable accept callback
    p->cb_flags |= ACCEPT_CB;
    // start the event loop
    poll_event_loop(pe);
}


void NetBase::OnConnect(const char *ip, short port) {
    //unsigned short 对应%hu（十进制） %ho（八进制） %hx（十六进制）
    LOGD("tcpServer OnConnect client :[%s]:[%hu]", ip, port);
}

void NetBase::OnDisconnect(int sockFd) {
    LOGD("tcpServer OnDisconnect client sockFd:[%d]", sockFd);
}

void NetBase::OnReceive(int sockFd, PDUBase &pack) {
    int length = pack.length;
    char *body = pack.body.get();
    body[length] = 0;

    LOGD("tcpServer OnReceive client sockFd:[%d] and body:\n%s", sockFd, body);
}

bool NetBase::Send(int sockFd, PDUBase &pdu) {
    LOGD("NetBase::Send...");
    std::lock_guard<std::mutex> lockGuard(send_mutex);
    char *buf = nullptr;
    int len = OnPduPack(pdu, buf);
    if (len > 0) {
        int ret = write(sockFd, buf, len);
        if (ret == -1) {
            LOGD("errno:[%d]", errno);
            char *msg = strerror(errno);
            LOGD("Message:[%s]", msg);
        }

        free(buf);  //释放内存
        return true;
    }
    return false;
}

bool NetBase::Send(int sockFd, char *buff, int len) {
    if (len > 0) {
        int ret = write(sockFd, buff, len);
        if (ret == -1) {
            LOGD("errno:[%d]", errno);
            char *msg = strerror(errno);
            LOGD("Message:[%s]", msg);
            return false;
        }
        return true;
    }
    return false;
}

void NetBase::OnTimeOut() {

}
