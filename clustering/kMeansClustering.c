#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAXITER 500
#define MAXINSTANCE 350
#define LINESIZE 20
#define FEATURECNT 4

struct Centroid {

        float age;
        float op_year;
        float num_pos_nodes;
        int cluster_no;

};



//Prototypes
int read_data_set(char line[LINESIZE], int mtr[MAXINSTANCE][FEATURECNT]);
void shuffle(int data[MAXINSTANCE][FEATURECNT],int count);
void set_init_centroids(struct Centroid* centroids,int data[MAXINSTANCE][FEATURECNT],int K_value);
int k_means_clustering(struct Centroid* centroids,int data[MAXINSTANCE][FEATURECNT],int K_value,int sample_cnt);
float calculate_euclidean_dist(struct Centroid centroid,int sample[FEATURECNT]);
void find_new_centroids(int data[MAXINSTANCE][FEATURECNT],struct Centroid* centroids,int K_value,int sample_cnt);
int is_same(struct Centroid* centroids,struct Centroid* past_centroids,int K_value);



//Reads Haberman data set from file
int read_data_set(char line[LINESIZE], int mtr[MAXINSTANCE][FEATURECNT]) {

	FILE *myfile = fopen ( "habermanData.txt", "r" );
	int i = 0;
        int k;
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


//Shuffles data set that is read from file
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


//Set first K values from shuffled data as initial cluster centroids
void set_init_centroids(struct Centroid* centroids,int data[MAXINSTANCE][FEATURECNT],int K_value) {

        int i;

        for (i = 0; i < K_value; i++) {
                centroids[i].age = data[i][0];
                centroids[i].op_year = data[i][1];
                centroids[i].num_pos_nodes = data[i][2];
                centroids[i].cluster_no = i;
        }


}


//Does K means clustering
int k_means_clustering(struct Centroid* centroids,int data[MAXINSTANCE][FEATURECNT],int K_value,int sample_cnt) {

        //float delta = 0.0001 * K_value;
        float dist,min_dist;
        int i = 0,flag = 0,k,min,j;


        struct Centroid* past_centroids = (struct Centroid*)malloc(sizeof(struct Centroid)*K_value);

        //Past centroids are init centroids at the beginning
        for (k = 0; k < K_value;k++) {
                past_centroids[k] = centroids[k];
        }


        while (!flag &&  i<MAXITER) {

                //Traversing through data set
                for (k = 0; k < sample_cnt; k++) {

                        min_dist = calculate_euclidean_dist(centroids[0],data[k]);
                        data[k][3] = 0; //Initial cluster_no of the sample

                        //Traversing through centroids
                        for (j = 0; j < K_value; j++) {
                                dist = calculate_euclidean_dist(centroids[j],data[k]);

                                if (dist < min_dist) {
                                        //Setting sample's cluster no
                                        data[k][3] = j;
                                        min_dist = dist;
                                }
                        }

                }


                //Storing initial value in the past_centroids
                for (k = 0; k < K_value;k++) {
                        past_centroids[k] = centroids[k];
                }

                //Getting new cluster centroids by calculating means of samples which belongs to centroids
                find_new_centroids(data,centroids,K_value,sample_cnt);

                //Checks whether centroids are equal to past_centroids or the difference between them is minimum
                flag = is_same(centroids,past_centroids,K_value);

                i++;

        }

        free(past_centroids);
        return i;



}


//Calculates Euclidean distance between a centroid and sample
float calculate_euclidean_dist(struct Centroid centroid,int sample[FEATURECNT]) {

        int i;
        float dist;


        dist = pow(sample[0]-centroid.age,2) + pow(sample[1]-centroid.op_year,2) + pow(sample[2]-centroid.num_pos_nodes,2);
        dist = sqrt(dist);

        return dist;


}



//Finds new centroids by by taking average of samples which belong to K clusters
void find_new_centroids(int data[MAXINSTANCE][FEATURECNT],struct Centroid* centroids,int K_value,int sample_cnt) {

        int i,k,j,cnt,sum;

        for (i = 0; i < K_value; i++) {

                cnt = 0;
                int* sum = (int*)calloc(FEATURECNT-1,sizeof(int));

                for (k = 0; k < sample_cnt; k++) {

                        if (data[k][3] == centroids[i].cluster_no) {

                                cnt++;

                                //Adding initial sample's feature values to sum
                                for (j = 0; j < FEATURECNT-1; j++) {
                                        sum[j] += data[k][j];
                                }

                        }

                }

                centroids[i].age = (float)sum[0]/cnt;
                centroids[i].op_year = (float)sum[1]/cnt;
                centroids[i].num_pos_nodes = (float)sum[2]/cnt;

                free(sum);


        }


}


// Returns 1 if centroids are same or a little different else 0
int is_same(struct Centroid* centroids,struct Centroid* past_centroids,int K_value) {

        int i;
        float delta = 0.0001 * K_value;
        float sum = 0.0;


        for (i = 0; i < K_value; i++) {
                sum += sqrt(pow(centroids[i].age-past_centroids[i].age,2) + pow(centroids[i].op_year-past_centroids[i].op_year,2) + pow(centroids[i].num_pos_nodes-past_centroids[i].num_pos_nodes,2));

        }

        if (sum <= delta) {
                return 1;
        }

        return 0;


}



int main() {

        int K_value;
        int sample_cnt;
        struct Centroid* centroids;
        int data[MAXINSTANCE][FEATURECNT];
        char line[LINESIZE];
        int iteration_cnt,k,i;


        printf("Please enter number of clusters: \n");
        scanf("%d",&K_value);
        printf("\n\n");

        centroids = (struct Centroid*)malloc(sizeof(struct Centroid)*K_value);
        int* total_cnt = (int*)calloc(K_value,sizeof(int));


        sample_cnt=read_data_set(line,data);
        shuffle(data,sample_cnt);
        set_init_centroids(centroids,data,K_value);
        iteration_cnt = k_means_clustering(centroids,data,K_value,sample_cnt);

        printf("K means clustering total number of iterations: \n");
        printf("%d\n\n",iteration_cnt );




        printf("Final cluster centroids: \n\n");
        for (i = 0; i < K_value; i++) {
                printf("Centroid %d: (%f ,%f ,%f)\n",i+1,centroids[i].age,centroids[i].op_year,centroids[i].num_pos_nodes );
        }


        printf("Number of elements in clusters: \n\n");
        for (i = 0; i < sample_cnt; i++) {

                total_cnt[data[i][3]]++;
        }

        for (i = 0; i < K_value; i++) {
                printf("Cluster %d: %d\n",i+1,total_cnt[i] );
        }









        return 0;
}
