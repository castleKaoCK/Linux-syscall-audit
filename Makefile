#Makefile 2.6
#obj-m := hellomod.o				#产生hellomod模块的目标文件
#obj-m := memmod.o					#产生memmod模块的目标文件
#obj-m := intrmod.o					#产生中断统计模块的目标文件
#obj-m := taskletmod.o				#产生小任务模块的目标文件
#obj-m := workqueuemod.o			#产生任务队列的目标文件	
#obj-m := timermod.o				#产生定时器模块的目标文件
#obj-m := super_block.o
#obj-m := mycdev.o					#字符设备驱动
#obj-m := cdev_driver.o				#字符设备驱动
#obj-m := print_vma.o				#打印进程虚存区
#obj-m := get_page.o				#虚拟地址转换为物理地址
#obj-m := hack_syscall.o			#篡改系统调用mkdir
obj-m := my_audit.o					#系统调用监控
#obj-m := procon.o					#生产者消费者
#obj-m := sharelist.o					#内核任务并发
#obj-m := test.o
#test-objs := xen-head.o head_64.o init-mm.o map_driver.o
#CONFIG_MODULE_SIG = n
CURRENT_PATH := $(shell pwd) 		#模块所在的当前路径
LINUX_KERNEL := $(shell uname -r) 	#Linux内核源代码的当前版本
#LINUX_KERNEL_PATH := /usr/src/linux-headers-$(LINUX_KERNEL)	#Linux内核源代码的绝对路径	4.2.0
LINUX_KERNEL_PATH := /usr/src/linux-$(LINUX_KERNEL)	#Linux内核源代码的绝对路径	4.2.1
KBUILD_EXTRA_SYMBOLS += $(LINUX_KERNEL_PATH)/Module.symvers
export KBUILD_EXTRA_SYMBOLS
all:
	make -C $(LINUX_KERNEL_PATH)M=$(CURRENT_PATH)modules	#编译模块
clean:
	make -C $(LINUX_KERNEL_PATH)M=$(CURRENT_PATH)clean		#清理模块
