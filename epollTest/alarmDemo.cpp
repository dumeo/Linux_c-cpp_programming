#include<iostream>
#include<unistd.h>
#include<signal.h>
using std::cout;
using std::endl;
void caculator(int a, int b){
	cout << "the res is " <<  a / b << endl;
}


void handler(int sig){
	cout << "handling signal..." << endl;
	cout << "catch signal " << sig << endl;
	cout << "The errno is " << errno << endl;
	cout << "Input the number:";
	int a, b;
	scanf("%d%d", &a, &b);
	caculator(a, b);
	exit(sig);
}

int main(){
	signal(SIGALRM, handler);
	alarm(3);//alarm函数设置SIGALRM信号并传送给当前进程
	for(int i = 1; i ; i++) cout << i << endl;
	return 0;
}






















