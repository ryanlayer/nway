#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "pq.h"
#include "nway.h"
#include "timer.h"


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
#ifndef IN_TIME_SPLIT
    start();
#endif
    split_o(S, set_sizes, num_sets, &R);
#ifndef IN_TIME_SPLIT
    stop();
    printf("%lu\n", report());
#endif

    int i;
    if (to_print != 0) {
        struct int_list_list *curr = R;
        while (curr != NULL) {
            int j;
            int min_v = INT_MAX, max_v =0;
            for (j = 0; j < curr->size; ++j) {
                /*
                if (j!=0)
                    printf("\t");

                printf("%d,%d,%d", 
                                curr->list[j],
                                S[j][curr->list[j]].start,
                                S[j][curr->list[j]].end);
                */
                min_v = min(min_v, S[j][curr->list[j]].end);
                max_v = max(max_v, S[j][curr->list[j]].start);
            }
            //printf("\t%d\t%d\n", max_v, min_v);
            printf("%d\t%d\n", max_v, min_v);
            curr = curr->next;
        }
    }


    free_int_list_list(R);

    for (i = 0; i < num_sets; i++) 
        free(S[i]);
}
