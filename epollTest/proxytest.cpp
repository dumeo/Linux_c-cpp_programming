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
#include<iostream>
using std::string;
using std::cout;
using std::endl;
char cldata[2048];
char rpcl[1024];


typedef struct adrrInfo{
    string host;
    int port = -1;
}addrInfo;

char* getDns(const char* domain){
    struct hostent* hostinfo= gethostbyname(domain);
    if(hostinfo){
        struct in_addr netaddr = *(struct in_addr*)*(hostinfo->h_addr_list);
        char* ip = inet_ntoa(netaddr);
        return ip;
    }
    return NULL;
}

void setnonblock(int fd){
    int old = fcntl(fd, F_GETFL);
    int ne = fcntl(fd, F_SETFL, old | O_NONBLOCK); 
    if(ne < 0) {
        printf("Set fcntl failed\n");
        return;
    }
}

int parse(){
    if(cldata[0] == 'G') return 1;
    else if(cldata[0] == 'C') return 2;
    else return -1;
}


void getaddrInfo(addrInfo* res){
    //get
    if(parse() == 1){
        int i = 0;
        string tmp;
        while(cldata[i] != '/') i ++;
        i += 2; 
        while(cldata[i] != '/') tmp += cldata[i], i ++;
        //printf("tmp domain = %s\n", tmp.c_str());
        res->host = tmp;
        //printf("res.host = %s\n", res->host);
        res->port = 80;
        
    }
    
    else if(parse() == 2){
        int i = 0;
        while(cldata[i] != ' ') i++;
        i ++;
        string tmp;
        while(cldata[i] != ':') tmp += cldata[i], i ++;
        //printf("tmp domain = %s\n", tmp.c_str());
        res->host = tmp;
        //printf("res.host = %s\n", res->host);
        tmp = "";
        i ++;
        while(cldata[i] != ' ') tmp += cldata[i], i++;
        res->port = atoi(tmp.c_str());
    }
    
}

typedef struct fdt{
    int fd = -1;
    int flag = -1;
    char data[2048];
}fdt;

void* handle(void* fdttt){
    printf("-------Handle thread handling client-------\n");
    fdt* fdtt = (fdt*)fdttt;
    struct sockaddr_in rmaddr;
    addrInfo* info = new addrInfo;
    getaddrInfo(info);
    bzero(&rmaddr, sizeof(rmaddr));
    const char* ip = getDns(info->host.c_str());
    int port = info->port;
    rmaddr.sin_family = AF_INET;
    rmaddr.sin_port = htons(port);
    rmaddr.sin_addr.s_addr = inet_addr(ip);
    int to_rm_fd = socket(PF_INET, SOCK_STREAM, 0);
    
    if(to_rm_fd < 0){
        printf("handle %s failed\n", info->host.c_str());
        pthread_exit(NULL);
    }
    
    int to_cl_fd = fdtt->fd;
    int flag = fdtt->flag;
    
    char buf[18096];
    bzero(buf, sizeof buf);
    if(flag == 1){ //GET
        
        int rm_efd = connect(to_rm_fd, (const struct sockaddr*)&rmaddr, sizeof(rmaddr));
        if(rm_efd < 0) {
            printf("connect %s failed\n", info->host.c_str());
            pthread_exit(NULL);
        }
        
        
        printf("----buf-----\n%s\n", fdtt->data);
        
        
        write(to_rm_fd, fdtt->data, sizeof fdtt->data);
        bzero(buf, sizeof buf);
        while(read(to_rm_fd, buf, sizeof buf) > 0){
            int c = send(to_cl_fd, buf, sizeof buf, 0);
            printf("\nwite %d to client\n", c);
            bzero(buf, sizeof buf);
        }
        close(to_cl_fd);
//        bzero(buf, sizeof buf);
//        read(to_rm_fd, buf, sizeof buf);
//        if(buf[0]) printf("--------from %s Data---------\n%s", info->host.c_str(), buf);
//        int c = send(to_cl_fd, buf, sizeof buf, 0);
//        printf("\nwite %d to client\n", c);
//        close(to_rm_fd);
        pthread_exit(NULL);
    }
    
    else if(flag == 2){ //CONNECT
        int rm_efd = connect(to_rm_fd, (struct sockaddr*)&rmaddr, sizeof(rmaddr));
        if(rm_efd < 0) {
            printf("connect %s failed\n", info->host.c_str());
            pthread_exit(NULL);
        }
        
        printf("Handling Connect request\n");
        
        int c = write(to_cl_fd, "HTTP/1.1 200 Connection Established", strlen("HTTP/1.1 200 Connection Established"));
        printf("###After connection, wrote %d data back to client <<%d>>\n", c, to_cl_fd);
        while(1){
            bzero(buf, sizeof buf);
            read(to_cl_fd, buf, sizeof buf);
            if(buf[0]){
                printf("%s\n^^^^^client <<%d>> sent to server^^^^^\n", buf, to_cl_fd);
                int c = write(to_rm_fd, buf, sizeof(buf));
                printf("clent sent %d data to remote server\n", c);
                bzero(buf, sizeof buf);
            }

            
            read(to_rm_fd, buf, sizeof buf);
            if(buf[0]) printf("--------from %s Data---------\n%s", info->host.c_str(), buf);
            if(buf[0]) write(to_cl_fd, buf, sizeof buf);
        }
    }
    
    
}




int main(int argc, char* argv[]){
    int clPort = atoi(argv[1]);
    struct sockaddr_in laddr;
    bzero(&laddr, sizeof(laddr));
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(clPort);
    laddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(lfd != -1);
    int ret = bind(lfd, (struct sockaddr*)&laddr, sizeof(laddr));
    assert(ret != -1);
    ret = listen(lfd, 5);
    assert(ret != -1);
    struct sockaddr_in caddr;
    bzero(&caddr, sizeof(caddr));
    socklen_t len = sizeof(caddr);
    
    int k = 0, cfd, cnt = 0;
    
    
    char ex[1024];
    while(1){
//        printf("exit?(y/n)\n");
//        scanf("%s", ex);
//        if(strcmp(ex, "y") == 0) break;
    
        cfd = accept(lfd, (struct sockaddr*)&caddr, &len);
        if(cfd != -1){
            printf("Main loop accept client %d sucessfully\n", cfd);
            cnt ++;
            if(cnt >= 30) break;
        }
        else {
            printf("Accept failed\n");
            break;
        }
    
        
        bzero(cldata, sizeof cldata);
        read(cfd, cldata, sizeof cldata);
        printf("<----%d---->\n%s", cfd, cldata);
        printf("-----------------------------------\n");
        if(cnt <= 30){
            fdt clinfo;
            clinfo.fd = cfd;
            clinfo.flag = parse();
            for(int i = 0; cldata[i]; i++) clinfo.data[i] = cldata[i];
            pthread_t thread;
            pthread_create(&thread, NULL, handle, (void*)&clinfo);
            //pthread_join(thread, NULL);
        }
        
        
        if(cldata[0]){
            addrInfo* info = new addrInfo;
            getaddrInfo(info);
            printf("domain = %s\n", info->host.c_str());
            printf("port = %d\n", info->port);
            char* ip = getDns(info->host.c_str());
            printf("ip = %s\n", ip == NULL ? "NULL" : ip);
            delete info;
        }
        
        //close(cfd);
        sleep(60);
    }
    close(lfd);
    return 0;
}