#include "algorithm.h"


char first_time_run = 1;						//（或扩容）首次运行标志
int expland_time = 0;							//动态扩容次数
int normalSequence[SEQ_SIZE*MAX_EXP_TIME] = {0};//正常序列
int normalLength = 0;							//正常序列长度
int realTimeLength = 0;							//实时序列长度
int * realTimeSequence = NULL;					//实时序列
int * diff_before = NULL;						//加窗过滤前的差异度向量
int * abnormal = NULL;							//异常序列标记向量
double * diff_after = NULL;						//加窗过滤后的差异度向量
double * vcom = NULL;							//相对差异度向量
double * vden = NULL;							//差异密度向量





//将字符串转换为数字
int ch_to_int(const char * ch, int * arr)
{
	int i = 0, length = 0, num = -1;
	
	for(;i < SEQ_SIZE * MAX_EXP_TIME;++i)
	{
		if(ch[i] == ' ' || ch[i] == '\0' || ch[i] == '\n')
		{
			arr[length] = num != -1? num: arr[length];
			length += num != -1? 1: 0;
			if(ch[i] != ' ')
				break;
			num = -1;
		}
		else
		{
			if(num == -1)
				num = ch[i] - '0';
			else
				num = num * 10 + ch[i] - '0';
		}
	}
	return length;
}


/*
 *	使用两个从文件中读取的序列初始化字符串normalSequence,realTimeSequence。
 */
void init_sequence(void)
{
	int i;
	int fd;
	int rt_val;
	char normalSequenceChar[SEQ_SIZE * MAX_EXP_TIME] = {0};		//正常序列
	//char realTimeSequenceChar[SEQ_SIZE] = {0};		//实时序列

	fd = open(NORMALSEQ_FILE, O_RDONLY, 0000);
	if(fd == -1){
		perror("open failed");
		exit(EXIT_FAILURE);
	}

	rt_val = read(fd, normalSequenceChar, SEQ_SIZE * MAX_EXP_TIME);
	if(rt_val == -1){
		perror("read failed");
		exit(EXIT_FAILURE);
	}
	close(fd);
	/*
	fd = open(REALTMSEQ_FILE, O_RDONLY, 0000);
	if(fd == -1){
		perror("open failed");
		exit(EXIT_FAILURE);
	}
	
	rt_val = read(fd, realTimeSequenceChar, SEQ_SIZE);
	if(rt_val == -1){
		perror("read failed");
		exit(EXIT_FAILURE);
	}
	close(fd);
	*/
	normalLength = ch_to_int(normalSequenceChar, normalSequence);
	//realTimeLength = ch_to_int(realTimeSequenceChar, realTimeSequence);
	
	for(i = 0;i < normalLength;i++)
		printf("%d ", normalSequence[i]);
	printf("\n");
	/*
	for(i = 0;i < realTimeLength;i++)
		printf("%d ", realTimeSequence[i]);
	printf("\n");
	*/
}


/*
 *
 * 	求实时序列中某一短序列与正常序列的差异度。
 *	return: 差异度。
 */
int i_to_B (const int * a, const int * b, const int n)
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
void A_to_B(void)
{
	int i;
	int * temp;

	if(first_time_run)	//首次运行或扩容后首次运行
	{
		temp = (int *)malloc( sizeof(int) * (SEQ_SIZE * expland_time - SLIDE_WIND + 1) );
	
		if(diff_before == NULL)	//首次运行程序
		{
			diff_before = temp;
			for(i = 0;i < realTimeLength-SLIDE_WIND+1;++i)
			{
				diff_before[i] = i_to_B(realTimeSequence+i, normalSequence,
					normalLength-SLIDE_WIND+1);
				printf("diff_before[%d]:\t%d\n", i, diff_before[i]);
			}
			return ;	//计算出已存在序列的结果
		}
		else					//扩容后首次运行程序
		{
			//将之前的计算结果复制到新空间
			for(i = 0;i < realTimeLength-SLIDE_WIND;++i)
				temp[i] = diff_before[i];
			
			free(diff_before);
			diff_before = temp;
		}
	}

	//非首次运行程序或扩容后首次运行程序，在原来计算的结果上添加新的结果
	diff_before[realTimeLength-SLIDE_WIND] = i_to_B(realTimeSequence+realTimeLength-SLIDE_WIND, normalSequence, normalLength-SLIDE_WIND+1);
	printf("diff_before[%d]:\t%d\n", realTimeLength-SLIDE_WIND, diff_before[realTimeLength-SLIDE_WIND]);
	
}


/*
 *	对得到的差异度向量进行加窗过滤噪声，将结果放入diff_after。
 */
void filter_noise(const int * diff_before)
{
	int i, j;
	double * temp;

	if(first_time_run)	//首次运行或扩容后首次运行
	{
		temp = (double *)calloc((SEQ_SIZE * expland_time - SLIDE_WIND + 1),
			sizeof(double) );
		
		if(diff_after == NULL)	//首次运行程序
		{
			diff_after = temp;
			for(i = 0;i < FILTER_WIND-1;i++)
				printf("diff_after[%d]:\t%f\n", i, diff_after[i]);		
			
			for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND+1;i++)
			{
				for(j = i-FILTER_WIND+1;j <= i;j++)
					diff_after[i] += diff_before[j];
				
				diff_after[i] /= FILTER_WIND;
			}
	
			for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND+1;i++)
				printf("diff_after[%d]:\t%f\n", i, diff_after[i]);		
			return;	//计算出已存在序列的结果
		}
		else					//扩容后首次运行程序
		{
			//将之前的计算结果复制到新空间
			for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND;i++)
				temp[i] = diff_after[i];

			free(diff_after);
			diff_after = temp;
		}
	}

	//非首次运行程序或扩容后首次运行程序，在原来计算的结果上添加新的结果
	for(j = realTimeLength-SLIDE_WIND-FILTER_WIND+1;j <= realTimeLength-SLIDE_WIND;j++)
		diff_after[realTimeLength-SLIDE_WIND] += diff_before[j];
	diff_after[realTimeLength-SLIDE_WIND] /= FILTER_WIND;
	printf("diff_after[%d]:\t%f\n", realTimeLength-SLIDE_WIND, diff_after[realTimeLength-SLIDE_WIND]);
	
}


/*
 *	将过滤后的差异度向量元素逐个与差异度阀值比较，结果用来断定异常序列。
 */
void judge_abnormal(const double * diff_after)
{
	int i;
	int * temp;

	if(first_time_run)	//首次运行或扩容后首次运行
	{
		temp = (int *)calloc( (SEQ_SIZE * expland_time - SLIDE_WIND + 1),
			sizeof(int));
		
		if(abnormal == NULL)	//首次运行程序
		{
			abnormal = temp;
			for(i = 0;i < FILTER_WIND-1;i++)
				printf("abnormal[%d]:\t%d\n", i, abnormal[i]);
	
			for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND+1;i++)
			{
				abnormal[i] = diff_after[i] < DIFF_THRESHOLD ? 0 : 1;
				printf("abnormal[%d]:\t%d\n", i, abnormal[i]);
			}
			return;	//计算出已存在序列的结果
		}
		else					//扩容后首次运行程序
		{
			//将之前的计算结果复制到新空间
			for(i = 0;i < realTimeLength-SLIDE_WIND;i++)
				temp[i] = abnormal[i];

			free(abnormal);
			abnormal = temp;
		}
	}

	//非首次运行程序或扩容后首次运行程序，在原来计算结果上添加新的结果
	abnormal[realTimeLength-SLIDE_WIND] = diff_after[realTimeLength-SLIDE_WIND] < DIFF_THRESHOLD ? 0: 1;
	printf("abnormal[%d]:\t%d\n", realTimeLength-SLIDE_WIND, abnormal[realTimeLength-SLIDE_WIND]);

}


/*
 *	实际求解相对差异度的算法，vcom为相对差异度向量。
 */
void __compared_diff(const int * abnormal)
{
	int i, j;
	double * temp;

	if(first_time_run)	//首次运行或扩容后首次运行
	{
		temp = (double *)calloc( (SEQ_SIZE * expland_time - SLIDE_WIND + 1),
			sizeof(double) );
		
		if(vcom == NULL)	//首次运行程序
		{
			vcom = temp;
			for(i = 0;i < FILTER_WIND-1;i++)
				printf("vcom[%d]:\t%f\n", i, vcom[i]);
				
			for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND+1;i++)
			{
				for(j = i-SLIDE_WIND+1;j <= i; j++)
					vcom[i] += abnormal[j];
				vcom[i] /= i+1-SLIDE_WIND+1;
				printf("vcom[%d]:\t%f\n", i, vcom[i]);
			}
			return;	//计算出已存在序列的结果
		}
		else				//扩容后首次运行程序
		{
			//将之前的计算结果复制到新空间
			for(i = 0;i < realTimeLength-SLIDE_WIND;i++)
				temp[i] = vcom[i];

			free(vcom);
			vcom = temp;
		}
	}

	//非首次运行程序或扩容后首次运行程序，在原来极端结果上添加新的结果
	for(j = realTimeLength-SLIDE_WIND-SLIDE_WIND+1;j <= realTimeLength-SLIDE_WIND;j++)
		vcom[realTimeLength-SLIDE_WIND] += abnormal[j];
	vcom[realTimeLength-SLIDE_WIND] /= realTimeLength-SLIDE_WIND*2+2;
	printf("vcom[%d]:\t%f\n", realTimeLength-SLIDE_WIND, vcom[realTimeLength-SLIDE_WIND]);

}

/*
 *	求解相对差异度。步骤写于函数注释中。
 */
void compared_diff(void)
{
	
/*
 *	得到实时序列各个子序列差异度
 */
	
	A_to_B();	//diff_before申请空间
/*
 *	加窗过滤噪声,diff_after为过滤后的差异度
 */
	filter_noise(diff_before);	//diff_after申请空间

/*
 *	对比预设差异度门限值，确定是否为异常序列。是为1,否则为0.
 */

	judge_abnormal(diff_after);	//abnormal申请空间

/*
 *	求得相对差异度.
 */
	//free(diff_after);
	__compared_diff(abnormal);	//vcom申请空间

	return;
}


/*
 *	求差异密度。将结果放于vden，即差异密度向量。
 */
void diff_density(const int * abnormal)
{
	int i;
	double * temp;

	if(first_time_run)	//首次运行或扩容后首次运行
	{
		temp = (double *)calloc( (SEQ_SIZE * expland_time - SLIDE_WIND + 1),
			sizeof(double) );

		if(vden == NULL)	//首次运行程序
		{
			vden = temp;
			for(i = 0;i < FILTER_WIND-1;i++)
				printf("vden[%d]:\t%f\n", i, vden[i]);

			for(i = FILTER_WIND-1;i < realTimeLength-SLIDE_WIND+1;i++)
			{
				if(abnormal[i] == 1)
					vden[i] = 1.0 + vden[i-1];
				printf("vden[%d]:\t%f\n", i, vden[i]);
			}
			return;	//计算出已存在序列的结果
		}
		else				//扩容后首次运行程序
		{
			//将之前的计算结果复制到新空间
			for(i = 0;i < realTimeLength-SLIDE_WIND;i++)
				temp[i] = vden[i];
			
			free(vden);
			vden = temp;
		}
	}

	//非首次运行程序或扩容后首次运行程序，在原来计算结果上添加新的结果
	if(abnormal[realTimeLength-SLIDE_WIND] == 1)
		vden[realTimeLength-SLIDE_WIND] = 1.0 + vden[realTimeLength-SLIDE_WIND-1];
	printf("vden[%d]:\t%f\n", realTimeLength-SLIDE_WIND, vden[realTimeLength-SLIDE_WIND]);
	

}

/*
 *	实际判断异常程序算法。
 *	将两个向量vcom,vden同下标号元素作为一组二元组分别与其阀值TCOM,TDEN比较.
 *	若任一组二元组都高于阀值，则认定为异常程序。
 *	若任一组二元组一个成员高于阀值，一个成员低于阀值，则计算比较它们的偏离
 *	程度，若高于阀值的成员偏离程度大于低于阀值的成员，则认定为异常程序。
 *	否则认定为正常程序。
 */
int __judge_process_each_time(void)
{
	int i;
	double vec, ved;
	static char flag = 1;

	if(flag)
	{
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
	flag = 0;
	}
	else
	{
		i = realTimeLength-SLIDE_WIND;
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
int judge_process_each_time()
{
/*
 *	求得相对差异度向量，并返回异常序列标记向量。
 */
	compared_diff();
/*
 *	求得差异密度，并返回差异密度向量。
 */
	diff_density(abnormal);	//vden申请空间

	//free(abnormal);		//异常序列标记向量已经无用.

	return __judge_process_each_time();	//返回判断结果
}

void judge_process(const int input)
{
	int i;
	int * temp;

	if(realTimeLength + 1 > SEQ_SIZE * expland_time){
		++expland_time;
		first_time_run = 1;
		temp = (int *)malloc(sizeof(int) * SEQ_SIZE * expland_time);
		if(realTimeSequence != NULL){
			for(i = 0;i < realTimeLength;i++)
				temp[i] = realTimeSequence[i];
			free(realTimeSequence);
		}
		realTimeSequence = temp;
	}
	else
		first_time_run = 0;

	realTimeSequence[realTimeLength++] = input;

	if(normalLength < SLIDE_WIND+FILTER_WIND-1 || realTimeLength < SLIDE_WIND+FILTER_WIND-1){
		printf("normalLength or realTimeLength is not long enough\n");
		return ;
	}
	else if(vcom == NULL)
		first_time_run = 1;

	
	if(judge_process_each_time() != ABNORMAL)
		printf("\tCongratulations!\tThis is one normal syscall sequence!\n");
	else{
		printf("WARNING:This is one abnormal syscall sequence!\n");
		exit(EXIT_SUCCESS);
	}
}

void free_all()
{
	free(realTimeSequence);
	free(vcom);
	free(vden);
	free(diff_before);
	free(abnormal);
	free(diff_after);
}
