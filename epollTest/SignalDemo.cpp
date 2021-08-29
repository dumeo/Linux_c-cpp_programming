#include<iostream>
using std::cout;
using std::endl;
#include<unistd.h>
#include<signal.h>
void sighandler(int signal){
	cout << "handling signal... " << endl;
	cout << "caught sigal " << signal << endl;
	exit(signal);
}


int main(){
	signal(SIGINT, sighandler);
	for(int i = 1; i; i++) cout << i << endl;
	return 0;

}
