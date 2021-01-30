#include "../../../log/log_util.h"
#include "net_base.h"
#include "tcp_server.h"
#include "poll.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>

extern NetBase *_obj;

using namespace std;


void SetObj(NetBase *pObj) {
    _obj = pObj;
}

void SignalHandle() {
    signal(SIGPIPE, SIG_IGN);
}


void
read_cb(NetBase *obj, poll_event_t *poll_event, poll_event_element_t *node, struct epoll_event ev) {
    //char buf[1024]={0};
    char sbuf[1024] = {0};
    char *buf = sbuf;

    int val = read(node->fd, buf, 1024);

    if (val > 0) {
        buf[val] = '\0';
        LOGD(" received data -> %s", buf);

        PDUBase pdu;
        std::unordered_map<int, ConnectBuffer>::iterator it = obj->recv_buffers.find(node->fd);

        if (obj->recv_buffers.end() != it) {
            if (it->second.length >= 0) {
                std::shared_ptr<char> mergebuf(new char[it->second.length + val + 1]);
                memcpy(mergebuf.get(), it->second.body.get(), it->second.length);
                memcpy(mergebuf.get() + it->second.length, buf, val);
                buf = mergebuf.get();
                val += it->second.length;
                it->second.length = val;
                it->second.body = mergebuf;
            }
        } else {
            std::shared_ptr<char> newbuf(new char[val + 1]);
            memcpy(newbuf.get(), buf, val);
            ConnectBuffer connect_buffer;
            connect_buffer.body = newbuf;
            connect_buffer.length = val;
            obj->recv_buffers.insert({node->fd, connect_buffer});
            it = obj->recv_buffers.find(node->fd);
        }

        int result = 0;
        while ((result = obj->OnPduParse(buf, val, pdu)) != 0) {
            if (result < 0) {
                obj->recv_buffers.erase(node->fd);
                break;
            } else if (result > 0) {
                //read some data(a full package, remove those buffers)
                memmove(it->second.body.get(), it->second.body.get() + result,
                        it->second.length - result);

                it->second.length -= result;
                val -= result;
                //call callback.
                obj->OnRecv(node->fd, pdu);
                buf = it->second.body.get();
            }
        }
/*
                printf("DataPackage num :%d\n", vec.size());
                vector<DataPackage>::iterator iter = vec.begin();
                for (; iter != vec.end(); ++iter)
        {
                        obj->OnRecv(node->fd, &(*iter) );
                }
        */
    }
}


void close_cb(NetBase *obj, poll_event_t *poll_event, poll_event_element_t *node,
              struct epoll_event ev) {
    // close the socket, we are done with it
    poll_event_remove(poll_event, node->fd);
    obj->recv_buffers.erase(node->fd);
    //
    close(node->fd);
    LOGD("close_cb socket:%d", node->fd);
    obj->OnDisconn(node->fd);
}

void accept_cb(NetBase *obj, poll_event_t *poll_event, poll_event_element_t *node,
               struct epoll_event ev) {
    // accept the connection
    struct sockaddr_in clt_addr;
    socklen_t clt_len = sizeof(clt_addr);
    int listenfd = accept(node->fd, (struct sockaddr *) &clt_addr, &clt_len);
    fcntl(listenfd, F_SETFL, O_NONBLOCK);
    fprintf(stderr, "got the socket %d\n", listenfd);
    // set flags to check
    uint32_t flags = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
    poll_event_element_t *p;
    // add file descriptor to poll event
    poll_event_add(poll_event, listenfd, flags, &p);
    // set function callbacks
    p->read_callback = read_cb;
    p->close_callback = close_cb;

    short port = ntohs(clt_addr.sin_port);
    char ip[20] = {0};
    inet_ntop(AF_INET, (void *) &(clt_addr.sin_addr.s_addr), ip, 16);

    obj->OnConn(ip, port);
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
        //LOG("time out number %d", *value);
        // printf("tick (%d)\n", *value);
    }
    //¶àÌ¬
    obj->OnTimeOut();
    return 0;
}


NetBase::NetBase() {
    listen_num = 1;
    port = default_port;
    ip = "127.0.0.1";
    //memset(m_sIp, 0, sizeof(m_sIp));
    //memcpy(m_sIp, "127.0.0.1", sizeof(m_sIp));
}

NetBase::~NetBase() {
    m_Sock.Close();
}

int NetBase::ProcessListenSock() {
    SignalHandle();
    int sockid = m_Sock.CreateSocket();
    if (sockid > 0) {
        SetObj(this);
        //ÓÅÑÅœáÊø
        m_Sock.SetLinger(sockid);

        //·Ç×èÈû
        m_Sock.SetNonBlock(sockid);

        //¶Ë¿ÚÖØÓÃ
        m_Sock.SetReuse(sockid);

        m_Sock.BindSocket(sockid, (char *) ip.c_str(), port);

        m_Sock.ListenSocket(sockid, listen_num);
    }

    return sockid;
}

void NetBase::ProcessEpoll(int sock) {
    poll_event_t *pPe = poll_event_new(1000);
    if (!pPe) {
        LOGD("pPe=null");
        return;
    }
    // set timeout callback
    pPe->timeout_callback = timeout_cb;
    poll_event_element_t *p;
    // add sock to poll event
    poll_event_add(pPe, sock, EPOLLIN, &p);
    // set callbacks
    //p->read_callback = read_cb;
    p->accept_callback = accept_cb;
    p->close_callback = close_cb;
    // enable accept callback
    p->cb_flags |= ACCEPT_CB;
    // start the event loop
    LOGD("1111");
    use_the_force(pPe);
}

void NetBase::Init() {}

void NetBase::StartServer(std::string _ip, short _port) {
    ip = _ip;
    port = _port;
    Init();

    LOGD("%s %d", _ip.c_str(), (int) _port);
    int iSock = ProcessListenSock();
    if (iSock < 1) {
        return;
    }

    ProcessEpoll(iSock);
}

void NetBase::OnConn(const char *ip, short port) {

}

void NetBase::OnDisconn(int sockid) {
}

/*
void NetBase::OnRecv(int sockid, PDUBase &_pack)
{

}
*/

bool NetBase::Send(int _fd, PDUBase &_data) {
    printf("\nNetBase::Send\n");
    std::lock_guard<std::mutex> lk1(send_mutex);
    char *buf = NULL;
    //int len =  m_parse.PackageData(_data, buf);
    int len = OnPduPack(_data, buf);
    if (len > 0) {
        int ret = write(_fd, buf, len);
        if (ret == -1) {
            printf("errno=%d\n", errno);
            char *mesg = strerror(errno);
            printf("Mesg:%s\n", mesg);
        }

        free(buf);  //释放内存
    }
}

bool NetBase::Send(int _fd, char *buff, int len) {
    if (len > 0) {
        int ret = write(_fd, buff, len);
        if (ret == -1) {
            printf("errno=%d\n", errno);
            char *mesg = strerror(errno);
            printf("Mesg:%s\n", mesg);
            return false;
        }
        return true;
    }
    return false;
}

void NetBase::OnTimeOut() {

}
