#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <netdb.h>
#include <assert.h>
#include <pthread.h>
#include <string>
using std::string;
bool handling[1e4 + 11];//正在处理的文件描述符

typedef struct clinfo{
    int fd;
    char buf[2048];
}clinfo;

void setnonblock(int fd){
    int oldOP = fcntl(fd, F_GETFL);
    int newOP = oldOP | O_NONBLOCK;
    fcntl(fd, F_SETFL, newOP);
}

void addfd(int epfd, int fd, int event, int op){
    setnonblock(fd);
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = event;
    ev.data.fd = fd;
    int ret = epoll_ctl(epfd, op, fd, &ev);
    if(ret < 0){
        printf("Change fd %d status failed...\n");
        return ;
    }
}

const char* getHost(clinfo* cl){
    for(int i = 0; cl->buf[i]; i++){
        if(cl->buf[i] == ':'){
            int j = i - 4;
            string tmp;
            for(j; j < i; j++) tmp += cl->buf[j];
            if(tmp == "Host"){
                string res;
                i++;
                while(cl->buf[i] && cl->buf[i] != '\r') res += cl->buf[i], i++;
                return res.c_str();
            }
        }
    }
   
}

const char* getIP(const char* domain){
    struct hostent* hostinfo = gethostbyname(domain);
    if(hostinfo){
        struct in_addr netaddr = *(struct sockaddr*)*(hostinfo->h_addr_list)
        const char* ip = inet_ntoa(netaddr);
        return ip;
    }
    
    return NULL;
}


void* handleGet(void* info){
    clinfo* cl = (clinfo*) info;
    struct sockaddr_in rmaddr;
    bzero(&rmaddr, sizeof(rmaddr));
    const char* host = getHost(cl);///////////////*
    int port = 80;
    const char* ip = getIP(host); ////////////////*
    printf("----Handling Get----\n%s", cl->buf);
    printf("host = %s\nport = %d\nIP = %s\n", host, port, ip);
    rmaddr.sin_family = AF_INET;
    rmaddr.sin_port = htons(port);
    rmaddr.sin_addr.s_addr = inet_addr(ip);
    int rmfd = socket(PF_INET, SOCK_STREAM, 0);
    if(rmfd < 0){
        printf("Handle get: socket create failed...\n");
        return NULL;
    }
    
    int ret = connect(rmfd, (struct sockaddr*)&rmaddr, sizeof(rmaddr));
    if(ret < 0){
        printf("Handle get: connect remote host failed...\n");
        return NULL;
    }

    int clfd = cl->fd;
    //创建get epoll事件表
    
    struct epoll_event events[5];
    int epollfdGet =  = epoll_create(5);
    if(epollfdGet < 0) printf("epollGet\n");
    addfd(epollGet, rmfd, EPOLLIN | POLLRDHUP);
    addfd(epollGet, clfd, EPOLLIN | POLLRDHUP);
    while(1){
        int nfd = epoll_wait(epollfdGet, events, 5, -1);
        if(nfd < 0){
            printf("epoll wait get\n");
            pthread_exit(NULL);
        }
        
        for(int i = 0;i < nfd; i ++){
            
        }
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    /*write(rmfd, cl->buf, sizeof(cl->buf));
    bzero(cl->buf, sizeof(cl->buf));
    read(rmfd, cl->buf, sizeof(cl->buf));
    if(buf[0]){
        printf("-----Data from %s-----\n", host);
        ret = write(clfd, cl->buf, sizeof(cl->buf));
        if(ret > 0) printf("write %d data to %d\n", ret, clfd);
    }
    
    printf("-----Finish handle get-----\n");
    close(rmfd);*/
    return NULL;
    
    
}


void* handleCon(void* info){
    clinfo* cl = (clinfo*)info;
    struct sockaddr_in rmaddr;
    bzero(&rmaddr, sizeof(rmaddr));
    const char* host = getHost(cl);///////////////
    int port = getPort(cl);///////////////
    const char* ip = getIP(host); ///////////
    printf("----Handling Connect----\n%s", cl->buf);
    printf("host = %s\nport = %d\nIP = %s", host, port, ip);
    rmaddr.sin_family = AF_INET;
    rmaddr.sin_port = htons(port);
    rmaddr.sin_addr.s_addr = inet_addr(ip);
    int rmfd = socket(PF_INET, SOCK_STREAM, 0);
    if(rmfd < 0){
        printf("Handle connect: socket create failed...\n");
        pthread_exit(NULL);
    }
    
    int ret = connect(rmfd, (struct sockaddr*)&rmaddr, sizeof(rmaddr));
    if(ret < 0){
        printf("Handle get: connect remote host failed...\n");
        pthread_exit(NULL);
    }
    
    int clfd = cl->fd;
    
    write(clfd, "HTTP/1.1 200 Connection Established", 
    strlen("HTTP/1.1 200 Connection Established"));
    int epollfd_con = epoll_create(50);
    addfd(epollfd_con, clfd, EPOLLIN);////////////////
    addfd(epollfd_con, rmfd, EPOLLIN);
    epoll_event events[10];
    char data[2048]; //双方交互的数据缓冲区
    while(1){
        bzero(data, sizeof(data));
        int ret = epoll_wait(epollfd_con, events, 100, -1);
        if(ret < 0){
            printf("Connection: Epoll wait failed...\n");
            break;
        }
        
        for(int i = 0;i < ret; i++){
            int sockfd = events[i].data.fd;
            if(sockfd == clfd && (events[i].events & EPOLLIN)){
                read(sockfd, data, sizeof(data));
                if(data[0]){
                    printf("-----Data from %d-----\n%s", data);
                    int s = write(rmfd, data, sizeof(data));
                    if(s > 0) {
                        printf("Write to %s sucessfully\n", host);
                    }
                    else printf("Write to %s failed\n", host);
                }
            }
            
            else if(sockfd == rmfd && (events[i].events & EPOLLIN)){
                read(sockfd, data, sizeof(data));
                if(data[0]){
                    printf("-----Data from %d-----\n%s", sockfd, data);
                    int s = write(clfd, data, sizeof(data));
                    if(s > 0) {
                        printf("Write %d to client sucessfully\n", s);
                    }
                    else printf("Write to client failed\n");
                }
            }
            
        }
    }
}
