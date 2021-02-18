#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "../../../log/log_util.h"

using namespace std;

const int MAX_EPOLL_EVENTS = 1000;
const int MAX_MSG_LEN = 1024;

void setFdNonblock(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void err_exit(const char *s) {
    LOGE("error: %s\n", s);
    exit(0);
}

int create_socket(const char *ip, const int port_number) {
    struct sockaddr_in server_addr = {0};
    /* 设置ipv4模式 */
    server_addr.sin_family = AF_INET;           /* ipv4 */
    /* 设置端口号 */
    server_addr.sin_port = htons(port_number);
    /* 设置主机地址 */
    /*if(inet_pton(server_addr.sin_family, ip, &server_addr.sin_addr) == -1){
        err_exit("inet_pton");
    }*/
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //OK

    /* 建立socket */
    //int sockfd =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        err_exit("socket");
    }
    /* 设置复用模式 */
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        err_exit("setsockopt");
    }
    /* 绑定端口 */
    if (::bind(sockfd, (sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        err_exit("bind");
    }
    /* 设置被动打开 */
    if (::listen(sockfd, 5) == -1) {
        err_exit("listen");
    }
    return sockfd;
}

int start(const char *ip, const int port) {
    /* 创建套接字 */
    int sockfd = create_socket(ip, port);
    LOGE("success create sockfd %d\n", sockfd);
    setFdNonblock(sockfd);
    /* 创建epoll */
    int epollfd = epoll_create1(0);
    if (epollfd == -1) err_exit("epoll_create1");
    /* 添加sockfd到epollfd兴趣列表 */
    struct epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
        err_exit("epoll_ctl1");
    }
    /* 创建一个列表用于存放wait所返回的events */
    struct epoll_event events[MAX_EPOLL_EVENTS] = {0};
    /* 开始等待所有在epoll上挂上去的事件 */

    while (1) {
        /* 等待事件 */
        LOGE("begin wait\n");
        int number = epoll_wait(epollfd, events, MAX_EPOLL_EVENTS, -1);
        LOGE("end wait\n");
        sleep(1);
        if (number > 0) {
            /* 遍历所有事件 */
            for (int i = 0; i < number; i++) {
                int eventfd = events[i].data.fd;
                /* 如果触发事件的fd是sockfd，则说明有人连接上来了，我们需要accept他 */
                if (eventfd == sockfd) {
                    LOGE("accept new client...\n");
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int connfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
                    setFdNonblock(connfd);
                    /* accept之后，需要将文件描述符加入到监听列表中 */
                    struct epoll_event ev;
                    ev.data.fd = connfd;
                    ev.events = EPOLLIN;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
                        err_exit("epoll_ctl2");
                    }
                    LOGE("accept new client end.\n");
                }
                    /* 如果触发的fd不是sockfd，那就是新加的connfd */
                else {
                    /* 读出内容，直到遇到回车。然后显示该内容。 */
                    LOGE("read start...\n");
                    while (1) {
                        char buff[4097] = {0};
                        int ret = ::read(eventfd, &buff, 4096);
                        buff[ret] = '\0';
                        if (ret > 0) {
                            LOGE("recv data:%s", buff);
                        }
                        if (ret == 0) {
                            LOGE("client close.\n");
                            close(eventfd);
                            epoll_ctl(epollfd, EPOLL_CTL_DEL, eventfd, NULL);
                            break;
                        } else if (ret < 0) {
                            LOGE("read error.\n");
                            break;
                        }


                    }
                    LOGE("read end.\n");
                }
            }
        }
    }
}
