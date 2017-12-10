#include "algorithm.h"

int main(void){
	int input;

	init_sequence();

	while(scanf("%d", &input) != EOF)
	{
		judge_process(input);
	}
	printf("Process exit successfully\n");
	free_all();

	return 0;
}
