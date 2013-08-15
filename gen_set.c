#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "pq.h"
#include "nway.h"


int main(int argc, char **argv)
{
    if (argc != 7) {
        fprintf(stderr,"usage:\t%s "
                "<num sets> "
                "<num intervals> "
                "<interval size> "
                "<domain size> "
                "<min nway> "
                "<to print>\n",argv[0]);
        return 1;
    }
    unsigned int seed = 1;
    srand(seed);

    int num_sets = atoi(argv[1]),
        num_intervals = atoi(argv[2]),
        interval_size = atoi(argv[3]),
        domain_size = atoi(argv[4]),
        num_nways = atoi(argv[5]),
        to_print = atoi(argv[6]);

    int *set_sizes;


    struct interval **S = rand_set_flat_sets(num_sets,
                          num_intervals,
                          interval_size,
                          domain_size,
                          num_nways,
                          &set_sizes);

    int i,j;
    for (i = 0; i < num_sets; ++i) {
        for (j = 0; j < num_intervals; ++j)
            printf("%d,%d ", S[i][j].start, S[i][j].end);
        printf("\n");
    }
}
