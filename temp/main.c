#include "algorithm.h"

int main(void){
	init_sequence();
	printf("%s\n", normalSequence);
	printf("%s\n", realTimeSequence);
	if(normalLength <= FILTER_WIND || realTimeLength <= FILTER_WIND){
		printf("normalLength or realTimeLength is not long enough\n");
		exit(EXIT_SUCCESS);
	}
	
	if(judge_process() != ABNORMAL)
		printf("This is one normal syscall sequence!\n");
	else
		printf("WARNING:This is one abnormal syscall sequence!\n");
	free(vcom);
	free(vden);
	
	return 0;
}
