#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "pq.h"
#include "nway.h"


int main(int argc, char **argv)
{
    if (argc != 6) {
        fprintf(stderr,"usage:\t%s "
                "<num sets> "
                "<num intervals> "
                "<max interval size> "
                "<max gap size> "
                "<to print>\n",argv[0]);
        return 1;
    }
    unsigned int seed = 1;
    srand(seed);

    int num_sets = atoi(argv[1]),
        num_intervals = atoi(argv[2]),
        max_interval_size = atoi(argv[3]),
        max_gap_size = atoi(argv[4]),
        to_print = atoi(argv[5]);

    int *set_sizes;


    struct interval **S = rand_flat_sets(num_sets,
                                         num_intervals,
                                         max_interval_size,
                                         max_gap_size,
                                         &set_sizes);

    if (to_print == 1)
        printf("::nway_split::\n");
    int num_nway;
    start();
    nway_split(num_sets, set_sizes, S, &num_nway, to_print);
    stop();
    unsigned long split = report();
    fprintf(stderr,"%d\t%lu\n", num_nway, split);
}
