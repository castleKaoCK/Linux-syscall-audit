#include "algorithm.h"

int main(void){
	int i;
	int input;
	int * temp;

	init_sequence();

	while(scanf("%d", &input) != EOF)
	{
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
			//exit(EXIT_SUCCESS);
			continue;
		}
		else if(vcom == NULL)
			first_time_run = 1;

	
		if(judge_process() != ABNORMAL)
			printf("\tCongretulations!\tThis is one normal syscall sequence!\n");
		else{
			printf("WARNING:This is one abnormal syscall sequence!\n");
			exit(EXIT_SUCCESS);
		}
	}
	
	free(realTimeSequence);
	free(vcom);
	free(vden);
	free(diff_before);
	free(abnormal);
	free(diff_after);

	return 0;
}
