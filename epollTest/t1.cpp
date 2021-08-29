#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
using std::cout;
int main(){
    struct sockaddr_in svaddr;
    memset(&svaddr, 0, sizeof (svaddr));
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    assert(fd >= 0);
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(1235);
    svaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int bd = bind(fd, (struct sockaddr*)&svaddr, sizeof(svaddr));
    assert(bd >= 0);
    int ls = listen(fd, 5);
    assert(ls != -1);
    sleep(20);
    struct sockaddr_in cl;
    memset(&cl, 0, sizeof (cl));
    socklen_t len = sizeof(cl);
    int con = accept(fd, (struct sockaddr*)&cl, &len);
    if(con >= 0){
        char* remote = {""};
        remote = inet_ntoa(cl.sin_addr);
        auto host = ntohs(cl.sin_port);
        printf("Accept from IP %s, port %d\n", remote, host);
        close(con);
    }
    else printf("None\n");

    
}





























