#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define LINESIZE 20
#define MAXINSTANCE 350
#define CLASSCNT 2
#define FEATURECNT 4

int read_data_set(char line[LINESIZE], int mtr[MAXINSTANCE][FEATURECNT]);
void shuffle(int data[MAXINSTANCE][FEATURECNT],int count);
void bayes_classify(int test[MAXINSTANCE][FEATURECNT], int train[MAXINSTANCE][FEATURECNT],int confusion_matrix[CLASSCNT][CLASSCNT],int test_cnt,int train_cnt);
double calculate_mean(int class_label,int k,int train[MAXINSTANCE][FEATURECNT],int train_cnt);
double calculate_std_deviation(int class_label,int k,double mean,int train[MAXINSTANCE][FEATURECNT],int train_cnt);
void k_fold_cross_validation(int confusion_matrix[CLASSCNT][CLASSCNT],int data[MAXINSTANCE][FEATURECNT],int count,int k_val);


int read_data_set(char line[LINESIZE], int mtr[MAXINSTANCE][FEATURECNT]) { 

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

void shuffle(int data[MAXINSTANCE][FEATURECNT],int count) {

	int i,j,r,tmp;

	for (i = 0; i < count; i++) {
		time_t t;
		srand((unsigned) time(&t));
		r = (rand() % (count - 1 - i + 1)) + i;

		for (j = 0; j < FEATURECNT; j++) {

			tmp = data[i][j];
			data[i][j] = data[r][j];
			data[r][j] = tmp;
		}
	}

}


double calculate_mean(int class_label,int k,int train[MAXINSTANCE][FEATURECNT],int train_cnt) {

	int i,sum=0,count=0;
	double mean;

	for (i = 0; i < train_cnt; i++) {

		if (train[i][3] == class_label) {
			sum += train[i][k];
			count++;
		}
	}

	mean = (double)sum/count;
	return mean;

}


double calculate_std_deviation(int class_label,int k,double mean,int train[MAXINSTANCE][FEATURECNT],int train_cnt) {

	int i,count=0;
	double sum = 0.0,std_deviation;

	for (i = 0; i < train_cnt; i++) {

		if (train[i][3] == class_label) {

			sum += pow(train[i][k]-mean,2.0);
			count++;
		}

	}

	std_deviation = sqrt(sum/count-1);
	return std_deviation;

}

void bayes_classify(int test[MAXINSTANCE][FEATURECNT], int train[MAXINSTANCE][FEATURECNT],int confusion_matrix[CLASSCNT][CLASSCNT],int test_cnt,int train_cnt) {

	int i,j,k,class_label,norm_prob,predicted_class;
	double mean,std_deviation,*class_prob;


	class_prob = (double*)malloc(sizeof(double)*CLASSCNT);


        //traversing through test set
	for (i = 0; i < test_cnt; i++) {

                //for each class calculate probability
		for (j = 1; j < CLASSCNT+1; j++) {

			class_label = j;
			double mul = 1.0;

                        //calculate P(X|Class)
			//mul is the P(X|Class)
			for (k = 0; k < FEATURECNT-1; k++) {


				mean = calculate_mean(class_label,k,train,train_cnt);
				std_deviation = calculate_std_deviation(class_label,k,mean,train,train_cnt);

				mul *= ( 1.0/sqrt( 2.0*M_PI*pow(std_deviation,2.0) ) ) * exp( -1.0 * ( pow(test[i][k]-mean,2.0)/(2.0*pow(std_deviation,2.0)) ) );


			}


			//calculate P(Class)
			//mul2 is the P(class)
			int cnt_class_instance = 0;
			double mul2;

			for (k = 0; k < train_cnt; k++) {

				if (train[k][3] == class_label) {
					cnt_class_instance++;
				}
			}

			mul2 = (double)cnt_class_instance/train_cnt;

                        // class prob is the P(Class|X). P(X) is not considered
			class_prob[j-1] = mul*mul2;


		}

		if (class_prob[0] > class_prob[1]) {
			predicted_class = 1;
		}

		else {
			predicted_class = 2;
		}


		//putting the prediction in the confusion_matrix
		confusion_matrix[test[i][FEATURECNT-1]-1][predicted_class-1]++;

	}


	free(class_prob);

}

void k_fold_cross_validation(int confusion_matrix[CLASSCNT][CLASSCNT],int data[MAXINSTANCE][FEATURECNT],int count,int k_val) {

	int fold_mem_cnt,test_cnt,train_cnt,i,j,k;
	int test[MAXINSTANCE][FEATURECNT];
	int train[MAXINSTANCE][FEATURECNT];
        //used for test set range
	int left=0,right;

	fold_mem_cnt = count/k_val;
	right = fold_mem_cnt-1;

	for (i = 0; i < k_val; i++) {

		test_cnt = 0;
		train_cnt = 0;

		if (i == k_val-1) {
			right = count-1;

		}

	        //preparing test and train set
		for (j = 0; j < count ; j++) {

			if(j>=left && j<=right) {

				for (k = 0; k < FEATURECNT; k++) {
					test[test_cnt][k] = data[j][k];
				}

				test_cnt++;

			}

			else {

				for (k = 0; k < FEATURECNT; k++) {
					train[train_cnt][k] = data[j][k];
				}

				train_cnt++;

			}

		}

		bayes_classify(test,train,confusion_matrix,test_cnt,train_cnt);

		left += fold_mem_cnt;
		right += fold_mem_cnt;
	}

}



/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {

	char line[LINESIZE];
	int data[MAXINSTANCE][FEATURECNT];
	int confusion_matrix[CLASSCNT][CLASSCNT] = {0};
	int i,j,count,k_val;
	int sum_diagonal=0,sum_all=0;
	double accuracy;

        printf("Please enter the k value for k fold cross validation: ");
	scanf("%d",&k_val);


	count = read_data_set(line,data);
	shuffle(data,count);
	k_fold_cross_validation(confusion_matrix,data,count,k_val);

	for (i = 0; i < 2; i++) {

		for (j = 0; j < 2; j++) {
			sum_all += confusion_matrix[i][j];
		}
	}

	for (i = 0; i < 2; i++) {

		sum_diagonal += confusion_matrix[i][i];

	}

	accuracy = (double)sum_diagonal/sum_all;
	accuracy *= 100.0;

	printf("Accuracy: %f\n",accuracy );
	printf("Total number of instances: %d\n",sum_all );
        printf("\n");

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {

			printf("%d ",confusion_matrix[i][j] );
		}
		printf("\n");
	}






	return 0;
}
