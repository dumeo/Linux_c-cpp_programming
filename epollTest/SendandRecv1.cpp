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
using std::cout;
using std::endl;

void setnonblock(int fd){
    int oldfd = fcntl(fd, F_GETFL);
    int newfd = oldfd | O_NONBLOCK;
    fcntl(fd, F_SETFL, newfd);
}


void* handle_recv(void* fdd){
    int fd = *(int*) fdd;
    struct epoll_event events[5];
    int epollfd = epoll_create(5);
    struct epoll_event ev;
    bzero(&ev, sizeof ev);
    ev.events = EPOLLIN | POLLRDHUP;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    char buf[1024];
    memset(buf, 0, sizeof buf);


    while(1){
        int n = epoll_wait(epollfd, events, 5, -1);
        if(n < 0) {
            cout << "epoll wait erro" << endl;
            pthread_exit(NULL);
        }
        for(int i = 0;i < n; i++){
            int sockfd = events[i].data.fd;
            if(events[i].events & EPOLLIN){
                read(fd, buf, sizeof buf);
                printf("recv from other size:%s\n", buf);
            }
        }

    }
}

void* handle_send(void* fdd){
    int fd = *(int*) fdd;
    char buf[1024];
    struct epoll_event events[5];
    int epollfd = epoll_create(5);
    struct epoll_event ev;
    bzero(&ev, sizeof ev);
    ev.events = EPOLLOUT | POLLRDHUP;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    char buf[1024];
    memset(buf, 0, sizeof buf);


    while(1){
        int n = epoll_wait(epollfd, events, 5, -1);
        if(n < 0) {
            cout << "epoll wait erro" << endl;
            pthread_exit(NULL);
        }
        for(int i = 0;i < n; i++){
            int sockfd = events[i].data.fd;
            if(events[i].events & EPOLLOUT){
                write(fd, buf, sizeof buf);
                printf("sent sucessfully\n", buf);
            }
        }
}















































