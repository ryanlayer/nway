#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pq.h"
#include "nway.h"


int main(int argc, char **argv)
{

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


    //print_interval_sets(S, num_sets, set_sizes);

    // Initialize the ordering for S
    for (i = 0; i < num_sets; i++) {
        ordering[i].start = 0;
        ordering[i].end = -1;
    }

    int next[num_sets];
    for (i = 0; i < num_sets; i++)
        next[i] = 0;

    int set_ids[num_sets];
    for (i = 0; i < num_sets; i++)
        set_ids[i] = i;

    pri_queue q = priq_new(0);
    for (i = 0; i < num_sets; ++i) {
        priq_push(q, &set_ids[i], S[i][next[i]].start);
        next[i] += 1;
    }

    int scan = 1;
    while (scan == 1) {
        int start_pos;

        // get the next element to place into an ordering
        // s_i is the index of the set in S that will constribute the element
        // start_pos is the starting position of that element
        int *s_i_p = priq_pop(q, &start_pos);
        int s_i = *s_i_p;

        // remove anything from the orderings that ends before start_pos
        for (i = 0; i < num_sets; i++) {
            for (j = ordering[i].start; j <= ordering[i].end; ++j) {
                if (S[i][j].end < start_pos)  {
                    ordering[i].start += 1;
                }
            }
        }


        // Make sure that there is somethin left to push before pushing
        // the next elemetn from S[s_i]
        if (next[s_i] < num_elements) {
            priq_push(q, &set_ids[s_i], S[s_i][next[s_i]].start);
            next[s_i] += 1;
        }

        // Check to see if adding this element causes an n-way intersection
        int is_nway = 0;
        // Check the orders before s_i
        for (i = 0; i < s_i; i++)
            if (ordering[i].end >= ordering[i].start)
                is_nway += 1;
        // Check the orders after s_i
        for (i = s_i + 1; i < num_sets; i++)
            if (ordering[i].end >= ordering[i].start)
                is_nway += 1;

        // If all other orderings contain elements, than we have an n-way
        // intersection, print it
        if (is_nway == num_sets - 1) {

            /*
            printf("%d\t%d\n",
                    start_pos,
                    count_nway(num_sets, s_i, ordering));
            */
            print_nway(num_sets, s_i, start_pos, ordering);
        }
 

        // Add the element to the ordering for that set
        ordering[s_i].end+=1;

        // check to see if we can stop scanning To stop scanning the last
        // element in a set must have been moved out of context
        for (i = 0; i < num_sets; i++) {
            if (ordering[i].start >= num_elements)
                scan = 0;
        }
    }
}
