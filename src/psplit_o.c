#include <stdio.h>
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
    psplit_o(S, set_sizes, num_sets, &R,num_threads);
#ifndef IN_TIME_SPLIT
    stop();
    printf("%lu\n", report());
#endif

    int i;
    if (to_print != 0) {
        struct int_list_list *curr = R;
        while (curr != NULL) {
            int j;
            for (j = 0; j < curr->size; ++j) {
                if (j != 0)
                    printf("\t");
                printf("%d", curr->list[j]);
            }
            printf("\n");
            curr = curr->next;
        }
    }

    free_int_list_list(R);

    for (i = 0; i < num_sets; i++) 
        free(S[i]);
}
