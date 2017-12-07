#ifndef	_ALGORITHM_H
#define	_ALGORITHM_H

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

#define	SEQ_SIZE	100				//序列长度
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

extern int normalSequence[SEQ_SIZE];		//正常序列
extern int realTimeSequence[SEQ_SIZE];		//实时序列
extern int normalLength;					//正常序列长度
extern int realTimeLength;					//实时序列长度
extern double * vcom;						//相对差异度向量
extern double * vden;						//差异密度向量


void init_sequence(void);										//初始化两个序列
int judge_process(void);										//断定异常程序

#endif	//_ALGORITHM_H
