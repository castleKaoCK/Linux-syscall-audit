#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<signal.h>
#include<fcntl.h>
#include<sys/resource.h>
#include<sys/syscall.h>
#include<sys/types.h>

typedef unsigned char 	u8;
typedef unsigned short	u16;
typedef unsigned int 	u32;


#include "common.h"

int exit_flag = 0;

void handle_int(int signo){
	exit_flag = 1;
}

int main(int argc, char *argv[])
{
	struct syscall_buf audit_buf[AUDIT_BUF_SIZE];
	unsigned char reset = 1;
	int num = 0;
	int i, fd;
	char ch[2] = {0};

	signal(SIGINT, handle_int);

	fd = open("temp/test_sequence.txt", O_WRONLY, 0000);
	if(fd == -1){
		perror("open failed");
		exit(EXIT_FAILURE);
	}

	while(1){
		num = syscall(325, 0, (unsigned char *)audit_buf, (u16)(sizeof(struct syscall_buf)*AUDIT_BUF_SIZE), reset);
		printf("num:%d\n", num);
		for(i = 0;i < num;i++){
			printf("No[%d], serial:%d\t", i, audit_buf[i].serial);
			printf("syscall:$%d\n", audit_buf[i].syscall);
			printf("status:%x\n", audit_buf[i].status);
			printf("pid:%d\n", audit_buf[i].pid);
			printf("uid:%d\n", audit_buf[i].uid);
			printf("comm:%s\n", audit_buf[i].comm);
		
			ch[0] = (char)(audit_buf[i].syscall - 325 + (u32)'0');
			ch[1] = ' ';
			write(fd, ch, 2);
		}
		if(exit_flag){
			write(fd, '\0', 1);
			close(fd);
			exit(EXIT_SUCCESS);
		}
	}
	return 0;
}
