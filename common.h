#ifndef	_COMMON_H
#define	_COMMON_H

#define	COMM_SIZE	32
#define	AUDIT_BUF_SIZE	10
#define	WRNORM		1
#define	TEST		2

struct syscall_buf{
	u32		serial;
	u32		syscall;
	u32		status;
	pid_t	pid;
	uid_t	uid;
	u8		comm[COMM_SIZE];
};


#endif	//_COMMON_H
