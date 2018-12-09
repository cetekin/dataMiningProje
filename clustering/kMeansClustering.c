#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXITER 500


struct Centroid {

        int age;
        int op_year;
        int num_pos_nodes;


};



int main() {

        int K_value;
        float delta = 0.0001;
        struct Cluster* clusters;


        printf("Please enter number of clusters: \n");
        scanf("%d",&K_value);

        clusters = (struct Cluster*)malloc(sizeof(struct Cluster)*K_value);
        


        //nextafterf(norm1,INFINITY) - norm1 --> epsilon
        //4 epsilon -->
        //5 epsilon -->



        //4.1000 3.6000
        //4.1001 3.6001

        printf("%f\n",delta );
        free(clusters);












        return 0;
}
