#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
int main(int argc, char* argv[]){
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    struct sockaddr_in svaddr;
    bzero(&svaddr, sizeof(svaddr));
    socklen_t len = sizeof(svaddr);
    svaddr.sin_family = AF_INET;
    svaddr.sin_addr.s_addr = inet_addr(ip);
    svaddr.sin_port = htons(port);
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd != -1);
    int confd = connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr));
    if(confd < 0){
        printf("Connect\n");
        return 1;
    }
    char buf[1024];
    while(1){
        printf("Input:");
        scanf("%s", buf);
        if(strcmp(buf, "exit") == 0) break;
        write(sockfd, buf, 1024);
        bzero(buf, sizeof(buf));
    }
    
    close(sockfd);
    return 0;
}