#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pq.h"
#include "nway.h"


int main(int argc, char **argv)
{

    if (argc != 3) {
        printf("usage:\t%s <num sets> <num elements>\n", argv[0]);
        return 1;
    }

    int num_sets = atoi(argv[1]);
    int num_elements = atoi(argv[2]);
    int len = 10;

    struct interval *S[num_sets];
    struct pair ordering[num_sets];

    int set_sizes[num_sets];

    int i,j;
    for (i = 0; i < num_sets; i++)
        set_sizes[i] = num_elements;

    for (i = 0; i < num_sets; i++) {
        S[i] = (struct interval *)
               malloc(sizeof(struct interval) * set_sizes[i]);
        int last_start = 0;
        for (j = 0; j < set_sizes[i]; j++) {
            int space = rand() % 20;
            S[i][j].start = last_start + space;
            last_start = last_start + space;
            S[i][j].end = last_start + len;
        }
    }

    struct int_list_list *R;
    R = NULL;
    split(S, set_sizes, num_sets, &R);

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
