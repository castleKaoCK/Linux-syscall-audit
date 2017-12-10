#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
int main(void){
	int input;

	printf("my pid:%d\n", getpid());
	sleep(10);
	printf("ready\n");
	while(scanf("%d", &input) != EOF){
		if(input > 325 && input < 331)
			syscall(input);
	}
}
