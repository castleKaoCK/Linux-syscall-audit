#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
int main(void){
	int input;
	int fd;
	char buf[100];

	printf("my pid:%d\n", getpid());
	sleep(10);
	printf("ready\n");
	fd = open("temp/normal_sequence.txt", O_RDONLY, 0000);
	read(fd, buf, 100);
	close(fd);
	while(scanf("%d", &input) != EOF){
		if(input > 0 && input < 6)
			syscall(input+325);
	}
}
