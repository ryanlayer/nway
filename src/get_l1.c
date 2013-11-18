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

    struct split_search_node_list *leaf_head, *leaf_tail;
    struct split_search_node_list *to_clear_head, *to_clear_tail;

    l1_split_sets_o(S,
                    set_sizes,
                    &to_clear_head,
                    &to_clear_tail,
                    &leaf_head,
                    &leaf_tail,
                    num_sets);

    struct split_search_node_list *curr = leaf_head;

    struct int_list_list *R_head=NULL, *R_tail=NULL;
    while (curr != NULL) {
        struct split_search_node *slice = curr->node;

        int i,j;
        //for(i = 0; i < slice->S_dim.end - slice->S_dim.start + 1; ++i) {
        for(i = 0; i < num_sets; ++i) {
            if (i!=0)
                printf("\t");
            for(j = slice->s_dim[i].start; j <= slice->s_dim[i].end; ++j) {
                if (j != slice->s_dim[i].start)
                    printf(";");
                printf("%d,%d", slice->S[i][j].start, slice->S[i][j].end);
            }
        }
        printf("\n");

        curr = curr->next;
    }
}
