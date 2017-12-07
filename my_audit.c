#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/errno.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/list.h>
#include<linux/kdev_t.h>
#include<linux/spinlock.h>
#include<linux/cdev.h>
#include<linux/types.h>
#include<linux/time.h>
#include<linux/cred.h>
#include<linux/mm.h>
#include<linux/slab.h>
#include<linux/vmalloc.h>
#include<linux/sched.h>
#include<asm/current.h>
#include<asm/uaccess.h>
#include "common.h"

pid_t	cpid;
module_param(cpid, int, 0644);

DECLARE_WAIT_QUEUE_HEAD(buffer_wait);

static struct syscall_buf	audit_buf[AUDIT_BUF_SIZE];
static int current_pos = 0;
static u32 serial = 0;

void syscall_create_info(int syscall, int return_status){
	struct syscall_buf * temp;

	if(cpid == current->pid && current_pos < AUDIT_BUF_SIZE){
		temp = &audit_buf[current_pos];

		temp->serial = serial++;
		//temp->ts_sec = xtime.tv_sec;
		//temp->ts_micro = xtime.tv_usec;
		temp->syscall = syscall;
		temp->status = return_status;
		temp->pid = current->pid;
		//temp->uid = current->uid;
		temp->uid = current_uid().val;
		
		memcpy(temp->comm, current->comm, COMM_SIZE);

		if(++current_pos >= AUDIT_BUF_SIZE*8/10){
			printk("In this module--audit: it's near full.\n");
			wake_up_interruptible(&buffer_wait);
		}
	}
}

int syscall_collect_info(u8 type, u8 * us_buf, u16 us_buf_size, u8 reset){
	int ret = 0;
	if(!type){
		if(__clear_user(us_buf, us_buf_size)){
			printk("Error: clear_user.\n");
			return 0;
		}
		printk("In this module--system call:starting...\n");
		ret = wait_event_interruptible(buffer_wait, current_pos >= AUDIT_BUF_SIZE*8/10);
		printk("In this module--system call:over, current_pos is %d\n", current_pos);
		if(__copy_to_user(us_buf, audit_buf, (current_pos)*sizeof(struct syscall_buf))){
			printk("Error:copy error.\n");
			return 0;
		}
		ret = current_pos;
		current_pos = 0;
	}
	return ret;
}

extern void (* create_info_audit)(int, int);
extern int (* collect_info_audit)(unsigned char, unsigned char *, unsigned short, unsigned char);

static int __init audit_init(void)
{
	create_info_audit = syscall_create_info;		//创建系统调用信息的函数
	collect_info_audit = syscall_collect_info;		//收集信息的函数
	printk("Starting system call auditing.\n");
	return 0;
}

static void __exit audit_exit(void)
{
	create_info_audit = NULL;
	collect_info_audit = NULL;
	printk("Exiting system call auditing.\n");
	return ;
}

module_init(audit_init);
module_exit(audit_exit);
MODULE_LICENSE("GPL");
