#include "algorithm.h"



char normalSequence[SEQ_SIZE] = {0};		//正常序列
char realTimeSequence[SEQ_SIZE] = {0};		//实时序列
int normalLength = 0;						//正常序列长度
int realTimeLength = 0;						//实时序列长度
double * vcom = NULL;						//相对差异度向量
double * vden = NULL;						//差异密度向量




//去除读取到的字符串中的空格和回车
int remove_blank(char * ch)
{
	int i = 0, j = 0, length = 0;
	
	for(;i < SEQ_SIZE && ch[i] != '\0' && ch[i] != '\n';++i){
		if(ch[i] != ' '){
			ch[j++] = ch[i];
			++length;
		}
	}
	ch[j] = '\0';

	return length;
}


/*
 *	使用两个从文件中读取的序列初始化字符串normalSequence,realTimeSequence。
 */
void init_sequence(void)
{
	int fd;
	int rt_val;

	fd = open(NORMALSEQ_FILE, O_RDONLY, 0000);
	if(fd == -1){
		perror("open failed");
		exit(EXIT_FAILURE);
	}

	rt_val = read(fd, normalSequence, SEQ_SIZE);
	if(rt_val == -1){
		perror("read failed");
		exit(EXIT_FAILURE);
	}
	close(fd);
	
	fd = open(REALTMSEQ_FILE, O_RDONLY, 0000);
	if(fd == -1){
		perror("open failed");
		exit(EXIT_FAILURE);
	}
	
	rt_val = read(fd, realTimeSequence, SEQ_SIZE);
	if(rt_val == -1){
		perror("read failed");
		exit(EXIT_FAILURE);
	}
	close(fd);

	normalLength = remove_blank(normalSequence);
	realTimeLength = remove_blank(realTimeSequence);
}


/*
 *
 * 	求实时序列中某一短序列与正常序列的差异度。
 *	return: 差异度。
 */
int i_to_B (const char * a, const char * b, const int n)
{
	int K = 1, DIFF = 0, u = 0, j = 0, min;
	for(;j < n;++j, K = 1, DIFF = 0)
	{
		for(u = 0;u < SLIDE_WIND;++u)
		{
			if(a[u] != b[u+j])
				{	DIFF += K; ++K; }
			else
				K = 1;
		}
		if(DIFF == 0)
			return 0;
		else if( (j == 0) || (min > DIFF) )
			min = DIFF;
	}
	return min;
}


/*
 *	求实时序列的所有短序列与正常序列的差异度。
 *	将结果放入diff_before,即加窗过滤前的差异度向量.
 */
int * A_to_B(void)
{
	int i;
	int * diff_before = (int *)malloc( sizeof(int) * (realTimeLength-SLIDE_WIND+1) );
	for(i = 0;i < realTimeLength-SLIDE_WIND+1;++i){
		diff_before[i] = i_to_B(realTimeSequence+i, normalSequence,
			normalLength-SLIDE_WIND+1);
		printf("%d\n", diff_before[i]);
	}
	return diff_before;
}


/*
 *	对得到的差异度向量进行加窗过滤噪声，将结果放入diff_after。
 */
double * filter_noise(const int * diff_before)
{
	int i, j;
	double * diff_after;
	diff_after = (double *)calloc( (realTimeLength-SLIDE_WIND+1), 
		sizeof(double) );
	for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND+1;i++){
		for(j = i-FILTER_WIND+1;j <= i;j++){
			diff_after[i] += diff_before[j];
		}
		diff_after[i] /= FILTER_WIND;
	}
	
	for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND+1;i++){
		printf("%f\n", diff_after[i]);
	}
	
	return diff_after;
}


/*
 *	将过滤后的差异度向量元素逐个与差异度阀值比较，结果用来断定异常序列。
 */
void judge_abnormal(int * abnormal, const double * diff_after)
{
	int i;
	for(i = 0;i < FILTER_WIND-1;i++){
		abnormal[i] = 0;
		printf("%d\n", abnormal[i]);
	}
	for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND+1;i++){
		abnormal[i] = diff_after[i] < DIFF_THRESHOLD ? 0 : 1;
		printf("%d\n", abnormal[i]);
	}
}


/*
 *	实际求解相对差异度的算法，vcom为相对差异度向量。
 */
double * __compared_diff(const int * abnormal)
{
	int i, j;
	double * vcom;

	vcom = (double *)calloc( (realTimeLength-SLIDE_WIND+1),
		sizeof(double) );
	
	for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND+1;i++){
		for(j = i-SLIDE_WIND+1;j <= i; j++)
			vcom[i] += abnormal[j];
		vcom[i] /= i+1-SLIDE_WIND+1;
		printf("%f\n", vcom[i]);
	}

	return vcom;
}

/*
 *	求解相对差异度。步骤写于函数注释中。
 */
int * compared_diff(void)
{
	int * diff_before, * abnormal;
	double * diff_after;
	
/*
 *	得到实时序列各个子序列差异度
 */
	
	diff_before = A_to_B();
/*
 *	加窗过滤噪声,diff_after为过滤后的差异度
 */
	diff_after = filter_noise(diff_before);

/*
 *	对比预设差异度门限值，确定是否为异常序列。是为1,否则为0.
 */

	abnormal = diff_before;
	judge_abnormal(abnormal, diff_after);

/*
 *	求得相对差异度.
 */
	free(diff_after);
	vcom = __compared_diff(abnormal);

	return abnormal;
}


/*
 *	求差异密度。将结果放于vden，即差异密度向量。
 */
double * diff_density(const int * abnormal)
{
	int i;
	double * vden;

	vden = (double *)calloc( (realTimeLength-SLIDE_WIND+1),
		sizeof(double) );
	printf("\n%f\n", vden[0]);
	for(i = 1;i < realTimeLength-SLIDE_WIND+1;i++){
		if(abnormal[i] == 1)
			vden[i] = 1.0 + vden[i-1];
		printf("%f\n", vden[i]);
	}

	return vden;
}

/*
 *	实际判断异常程序算法。
 *	将两个向量vcom,vden同下标号元素作为一组二元组分别与其阀值TCOM,TDEN比较.
 *	若任一组二元组都高于阀值，则认定为异常程序。
 *	若任一组二元组一个成员高于阀值，一个成员低于阀值，则计算比较它们的偏离
 *	程度，若高于阀值的成员偏离程度大于低于阀值的成员，则认定为异常程序。
 *	否则认定为正常程序。
 */
int __judge_process(void)
{
	int i;
	double vec, ved;
	
	for(i = 0;i < realTimeLength-SLIDE_WIND+1;i++){
		if(vcom[i] > TCOM && vden[i] > TDEN){
			printf("vcom[%d]:%f\tvden[%d]:%f\n", i, vcom[i], i, vden[i]);
			return ABNORMAL;
		}
		else if(vcom[i] < TCOM && vden[i] > TDEN){
			vec = (TCOM - vcom[i]) / TCOM;
			ved = (vden[i] - TDEN) / TDEN;
			if(vec <= ved){
				printf("vcom[%d]:%f\tvden[%d]:%f\n", i, vcom[i], i, vden[i]);
				return ABNORMAL;
			}
		}
		else if(vcom[i] > TCOM && vden[i] < TDEN){
			vec = (vcom[i] - TCOM) / TCOM;
			ved = (TDEN - vden[i]) / TDEN;
			if(vec >= ved){
				printf("vcom[%d]:%f\tvden[%d]:%f\n", i, vcom[i], i, vden[i]);
				return ABNORMAL;
			}
		}
	}

	return NORMAL;
}


/*
 *	断定异常程序。执行步骤写于函数注释中。
 */
int judge_process()
{
	int * abnormal;
/*
 *	求得相对差异度向量，并返回异常序列标记向量。
 */
	abnormal = compared_diff();
/*
 *	求得差异密度，并返回差异密度向量。
 */
	vden = diff_density(abnormal);

	free(abnormal);		//异常序列标记向量已经无用.

	return __judge_process();	//返回判断结果
}
