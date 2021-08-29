#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<assert.h>
#include<cstring>
#include<arpa/inet.h>
int main(int argc, char* argv[]){
    assert(argc == 2);
    char* host = argv[1];
    struct hostent* hostinfo = gethostbyname(host);
    assert(hostinfo);
    struct servent* servinfo = getservbyname("daytime", "tcp");
    assert(servinfo);
    printf("daytime port is %d\n", ntohs(servinfo->s_port));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = servinfo->s_port;
    addr.sin_addr = *(struct in_addr*)*hostinfo->h_addr_list;
    char* ip = inet_ntoa(addr.sin_addr);//用inet_ntoa函数把网络字节序的ip转换成字符串ip
    if(ip[0]) printf("IP = %s\n", ip);
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd != -1);
    int con = connect(sockfd, (struct sockaddr*)& addr, sizeof(addr));
    assert(con != -1);
    char buf[128];
    memset(buf, 0, sizeof buf);
    int res = read(sockfd, buf, sizeof buf);
    assert(res > 0);
    printf("The daytime: %s\n", buf);
    close(sockfd);
    return 0;
    
}