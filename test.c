#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
int main(void){
	int input;
	while(scanf("%d", &input) != EOF){
		if(input > 0 && input < 6)
			syscall(input+325);
	}
}
