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
bool Houtai(){
    pid_t pid = fork();
    if(pid < 0) {
        printf("fork failed\n");
        return false;
    }
    else if(pid > 0) {
        printf("pid > 0\n");
        exit(0);
    }
    umask(0);
    pid_t sid = setsid();
    
    if(sid < 0) {
        printf("create son id failed\n");
        return false;
    }
    if(chdir("/") < 0) {
        printf("change dir failed\n");
        return false;
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDONLY);
    open("dev/null", O_RDWR);
    return true;
    
}

int main(){
    printf("fork begin...\n");
    if(!daemon(0,0)) printf("Houtai successfully\n");
    else printf("fork failed\n");
    return 0;
}