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
int setnonblock(int fd){
        int oldOp = fcntl(fd, F_GETFL);
        int newOp = oldOp | O_NONBLOCK;
        fcntl(fd, F_SETFL, newOp);
        return oldOp;
    }
    
    void addfd(int epollfd, int fd){
        setnonblock(fd);
        struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLOUT;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    }
    
    char buf[1024];
    struct epoll_event events[100];
    bool offline = false;
    
    
void* input(void* argv){
    int sockfd = *(int*) argv;
    while(1){
        printf("Input:");
        scanf("%s", buf);
        if(strcmp(buf, "exit") == 0){
            offline = true;
            break;
        }
        else {
            write(sockfd, buf, sizeof buf);
        }
    }
}



int main(int argc, char* argv[]){
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    struct sockaddr_in sv;
    bzero(&sv, sizeof(sv));
    sv.sin_family = AF_INET;
    sv.sin_port = htons(port);
    sv.sin_addr.s_addr = inet_addr(ip);
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd != -1);
    int connfd = connect(sockfd, (struct sockaddr*)&sv, sizeof(sv));
    assert(connfd != -1);
    while(1){

        printf("Input:");
        scanf("%s", buf);
        if(strcmp(buf, "exit") == 0) break;
        
        else {
            write(sockfd, buf, sizeof buf);
        }
        

        memset(buf, 0, sizeof buf);
        read(sockfd, buf, sizeof buf);
        printf("msg from server:%s\n", buf);
        
        if(offline) break;
        
    }
    
    close(connfd);
    return 0;
}