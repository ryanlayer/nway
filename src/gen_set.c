#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "pq.h"
#include "nway.h"
#include "utils.h"


int main(int argc, char **argv)
{
    struct interval **S;
    int *set_sizes;
    int to_print;
    int num_sets;
    int num_threads;
    int step_size;
    int num_splits;


    int r = parse_args(argc,
                       argv,
                       &S,
                       &set_sizes,
                       &num_sets,
                       &to_print,
                       &num_threads,
                       &step_size,
                       &num_splits);


    print_interval_sets(S, num_sets, set_sizes);
}
