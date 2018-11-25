#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14
#define LINESIZE 20
#define MAXINSTANCE 350

int read_data_set(char line[LINESIZE], int mtr[MAXINSTANCE][4]) {

	FILE *myfile = fopen ( "habermanData.txt", "r" );
	int i = 0;
	char *value;

	while(fgets(line, LINESIZE, myfile)){


		value = strtok(line, ",");
		mtr[i][0] = atoi(value);

		value = strtok(NULL, ",");
		mtr[i][1] = atoi(value);

		value = strtok(NULL, ",");
		mtr[i][2] = atoi(value);

		value = strtok(NULL, "\n");
		mtr[i][3] = atoi(value);

		i++;


	}

	return i;

}



/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {

	char line[LINESIZE];
	int data[MAXINSTANCE][4];
	int i,j,count;


	count = read_data_set(line,data);

	for (i = 0; i < count; i++) {
		for (j = 0; j < 4; j++) {
			printf("%d ",data[i][j] );
		}
		printf("\n");
	}

	printf("%d\n",count );










	return 0;
}
