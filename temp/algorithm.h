#ifndef	_ALGORITHM_H
#define	_ALGORITHM_H

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

#define	SEQ_SIZE	20				//序列块长度
#define	MAX_EXP_TIME	500			//最多扩容次数
#define	SLIDE_WIND	6				//滑动窗口大小
#define	FILTER_WIND	SLIDE_WIND		//过滤窗口大小，不能小于滑动窗口大小
#define	DIFF_THRESHOLD	1.00000	//差异度门限值
#define	TCOM		0.08			//相对差异度阀值
#define	TDEN		8				//差异密度阀值
#define	ABNORMAL	1
#define	NORMAL		0
#define	NORMALSEQ_FILE	"normal_sequence.txt"
//#define	REALTMSEQ_FILE	"real_time_sequence.txt"
#define	REALTMSEQ_FILE	"test_sequence.txt"

extern char first_time_run;					//（或扩容）首次运行标志
extern int expland_time;					//动态扩容次数
extern int normalSequence[SEQ_SIZE*500];	//正常序列
extern int normalLength;					//正常序列长度
extern int realTimeLength;					//实时序列长度
extern int * realTimeSequence;				//实时序列
extern int * diff_before;					//加窗过滤前的差异度向量
extern int * abnormal;						//异常序列标记向量
extern double * diff_after;					//加窗过滤后的差异度向量
extern double * vcom;						//相对差异度向量
extern double * vden;						//差异密度向量

void init_sequence(void);										//初始化两个序列
void judge_process(const int );									//断定异常程序
void free_all(void);											//释放所有动态申请的空间

#endif	//_ALGORITHM_H
