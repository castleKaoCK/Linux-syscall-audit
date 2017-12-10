#include<unistd.h>
#include<string.h>
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

static int exit_flag = 0;

void handle_int(int signo);	//信号处理函数
void write_normal(const int fd, unsigned int syscall);	//录入正常序列库

int main(int argc, char *argv[])
{
	struct syscall_buf audit_buf[AUDIT_BUF_SIZE];
	unsigned char reset = 1;
	int num = 0;
	int i, fd;

	if(argc < 2){
		perror("at least 2 parameter");
		exit(EXIT_FAILURE);
	}
		

	signal(SIGINT, handle_int);
	if(strcmp(argv[1], "normal") == 0)	//此次运行为录入正常序列库
	{
		fd = open("temp/normal_sequence.txt", O_WRONLY, 0000);
		if(fd == -1){
			perror("open failed");
			exit(EXIT_FAILURE);
		}
	}

	while(1)
	{
		num = syscall(325, 0, (unsigned char *)audit_buf, (u16)(sizeof(struct syscall_buf)*AUDIT_BUF_SIZE), reset);
		printf("num:%d\n", num);
		for(i = 0;i < num;i++)
		{

			printf("No[%d], serial:%d\t", i, audit_buf[i].serial);
			printf("syscall:$%d\n", audit_buf[i].syscall);
			printf("status:%x\n", audit_buf[i].status);
			printf("pid:%d\n", audit_buf[i].pid);
			printf("uid:%d\n", audit_buf[i].uid);
			printf("comm:%s\n", audit_buf[i].comm);
	
			if(strcmp(argv[1], "normal") == 0)
				write_normal(fd, audit_buf[i].syscall);
		}

		if(exit_flag)
		{
			if(strcmp(argv[1], "normal") == 0){	//此次运行为录入正常序列库
				write(fd, '\0', 1);
				close(fd);
			}
			exit(EXIT_SUCCESS);
		}
	}
	return 0;
}


void handle_int(int signo)
{
	exit_flag = 1;
}

void write_normal(const int fd, unsigned int syscall)
{
	char ch[4] = {0};
	int length = 2;

	ch[3] = ' ';
		
	if(syscall == 0){
		ch[2] = (u32)'0';
		write(fd, ch+2, 2);
		return ;
	}

	for(;syscall > 0;length--, syscall /= 10)
		ch[length] = syscall % 10 + (u32)'0';
	write(fd, ch+length+1, 2-length+1);
	
}
