#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<iostream>
using std::cout;
using std::endl;
using std::string;
int main(int argc, char* argv[]){
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    assert(fd != -1);
    int bd = bind(fd, (struct sockaddr*)& addr, sizeof (addr));
    assert(bd != -1);
    int ls = listen(fd, 10);
    assert(ls != -1);
    struct sockaddr_in cl;
    bzero(&cl, sizeof(cl));
    socklen_t len = sizeof(cl);
    int ac = accept(fd, (struct sockaddr*)&cl, &len);
    if(ac < 0){
        printf("Accept errro\n");
    }
    else{
        
        string IP = inet_ntoa(cl.sin_addr);
        int pt = ntohs(cl.sin_port);
        string port = std::to_string(pt);
        string tmp = "Your IP " + IP + "\nPort " + port + "\n";
        const char* clInfo = tmp.c_str();
        write(ac, clInfo, strlen(clInfo));
        for(int i = 0;i < 10;i ++)
            write(ac, "Hello from server...\n", strlen("Hello from server...\n"));
        
        close(ac);
    }
    close(fd);
    return 0;
}