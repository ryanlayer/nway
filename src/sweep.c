#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "pq.h"
#include "nway.h"
#include "timer.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

int main(int argc, char **argv)
{

    struct interval **S;
    int *set_sizes;
    int to_print;
    int num_sets;
    int num_threads;


    int r = parse_args(argc,
                       argv,
                       &S,
                       &set_sizes,
                       &num_sets,
                       &to_print,
                       &num_threads);

    /*
    if (to_print != 0) {
        print_interval_sets(S, num_sets, set_sizes);
        printf("\n");
    }
    */

    struct int_list_list *R;
    R = NULL;
    int num_R;
#ifndef IN_TIME_SPLIT
    start();
#endif
    sweep(S, set_sizes, num_sets, &R, &num_R);
#ifndef IN_TIME_SPLIT
    stop();
    printf("%lu\n", report());
#endif

    if (to_print == 1)
        print_nway_indicies(R, S);
    if (to_print == 2)
        print_nway_intervals(R, S);
    if (to_print == 3)
        print_nway_common_interval(R, S);

    free_int_list_list(R);

    int i;
    for (i = 0; i < num_sets; i++) 
        free(S[i]);
}

