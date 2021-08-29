#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<assert.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<stdlib.h>
#include<pthread.h>
struct fds{
    int epollfd, sockfd;
};
int setnonblock(int fd){
    int oldOption = fcntl(fd, F_GETFL);
    int newOption = oldOption | O_NONBLOCK;
    fcntl(fd, F_SETFL, newOption);
    return oldOption;
}

void addfd(int epollfd, int fd, bool oneshot){
    setnonblock(fd);
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if(oneshot) event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}


void reset_oneshot(int epollfd, int fd){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void* worker(void* arg){
    int sockfd = ((fds*)arg)->sockfd;
    int epollfd = ((fds*)arg)->epollfd;
    printf("start new thread to receive data in fd %d\n", sockfd);
    char buf[1024];
    while(1){
        bzero(buf, sizeof buf);
        int ret = read(sockfd, buf, 1024, 0);
        if(ret == 0){
            printf("remote client closed connection\n");
            close(sockfd);
            break;
        }
        else if(ret < 0 ){
            if(errno == EAGAIN){
                reset_oneshot(epollfd, sockfd);
                printf("read later\n");
                break;
            }
        }
        else {
            printf("ret = %d\nget content from client:%s\n", ret, buf);
            sleep(5);
        }
        
        
    }
    printf("end receicving msg thead\n");
}

int main(int argc, char* argv[]){
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    struct sockaddr_in sv;
    int ret = 0;
    bzero(&sv, sizeof(sv));
    sv.sin_family = AF_INET;
    sv.sin_port = htons(port);
    sv.sin_addr.s_addr = inet_addr(ip);
    
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd != -1);
    
    ret = bind(listenfd, (struct sockaddr*)&sv, sizeof(sv));
    assert(ret != -1);
    
    ret = listen(listenfd, 5);
    assert(ret != -1);
    
    epoll_event events[1024];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, listenfd, false);
    while(1){
        int ret = epoll_wait(epollfd, events, 1024, -1);
        if(ret < 0){
            printf("epoll failed\n");
            break;
        }
        for(int i = 0;i < ret; i++){
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd){
                struct sockaddr_in cl;
                bzero(&cl, sizeof(cl));
                socklen_t len = sizeof(cl);
                int connfd = accept(listenfd, (struct sockaddr*)&cl, &len);
                if(connfd != -1) printf("accepted sucessfully...\n");
                write(connfd, "Hello from server", strlen("Hello from server"));
                addfd(connfd, epollfd, true);
            }
            else if(events[i].events & EPOLLIN){
                printf("epoll in\n");
                pthread_t thread;
                fds newfd;
                newfd.sockfd = sockfd;
                newfd.epollfd = epollfd;
                
                pthread_create(&thread, NULL, worker, (void*)&newfd);
            }
            else printf("something else happened\n");
        }
        
    }
    close(listenfd);
    return 0;
}




































