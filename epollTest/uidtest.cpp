#include<stdio.h>
#include<unistd.h>
int main(){
    uid_t uid = getuid();
    uid_t euid = geteuid();
    printf("user id is %d, effective id id %d\n", uid, euid);
    return 0;
}