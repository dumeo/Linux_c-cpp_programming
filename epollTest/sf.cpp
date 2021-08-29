#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<cstring>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/sendfile.h>
int main(int argc, char* argv[]){
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    const char* filename = argv[3];
    int filefd = open(filename, O_RDONLY);
    assert(filefd > 0);
    struct stat stat_buf;
    fstat(filefd, &stat_buf);
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    int bd = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    assert(bd != -1);
    int ls = listen(sockfd, 5);
    assert(ls != -1);
    sockaddr_in cl;
    bzero(&cl, sizeof(cl));
    int len =  sizeof(cl);
    int con = accept(sockfd, (struct sockaddr*)&cl, (socklen_t*)&len);
    if(con != -1){
        sendfile(con, filefd, NULL, stat_buf.st_size);
        close(con);
    }
    close(sockfd);
    return 0;
}




















