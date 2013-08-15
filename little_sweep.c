#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pq.h"
#include "nway.h"

struct int_list_list
{
    int *list;
    struct int_list_list *next;
    int size;
};

//{{{int count_nway( int num_sets,
int count_nway( int num_sets,
                 int s_i,
                 int start_pos,
                 struct pair *ordering)
{
    int num_nways = 1;
    int i;
    for (i = 0; i < num_sets; i++) {
        if (i != s_i)
            num_nways *= ordering[i].end - ordering[i].start + 1;
    }

    return num_nways;
}
//}}}

//{{{ void print_nway( int num_sets,
void print_nway( int num_sets,
                 int s_i,
                 int start_pos,
                 struct pair *ordering)
{
    // Create the first nway list
    struct int_list_list *head = (struct int_list_list *)
            malloc(sizeof(struct int_list_list));
    struct int_list_list *tail = head;

    head->size = num_sets;
    head->list = (int *) malloc(head->size * sizeof(int));
    head->next = NULL;

    // Create nways from the orders before s_i
    int i;
    for (i = 0; i < num_sets; i++) {

        if (i == s_i) {
            // Add the new item to the list
            struct int_list_list *curr = head;
            while (curr != NULL) {
                curr->list[s_i] = ordering[s_i].end + 1;
                curr = curr->next;
            }
        } else { 


            // set the first item in the ordering
            struct int_list_list *curr = head;
            while (curr != NULL) {
                //printf("0\t%d %d\n", i, ordering[i].start);
                curr->list[i] = ordering[i].start;
                curr = curr->next;
            }

            // if there is more than one element in the ordering, then we
            // need to make copies of the nway lists up to this point
            // and set the values
            struct int_list_list *copy_head, *copy_tail;
            copy_head = NULL;

            int j;
            // make copies
            for (j = ordering[i].start + 1; j <= ordering[i].end; ++j) {
                struct int_list_list *curr = head;
                while (curr != NULL) {

                    struct int_list_list *nway_copy = 
                            (struct int_list_list *)
                            malloc(sizeof(struct int_list_list));
                    nway_copy->size = num_sets;
                    nway_copy->list = (int *) 
                            malloc(head->size * sizeof(int));
                    nway_copy->next = NULL;

                    //copy elements from the current nway over
                    memcpy(nway_copy->list,
                           curr->list,
                           num_sets*sizeof(int));

                    //printf("1\t%d %d\n", i, j);
                    nway_copy->list[i] = j;


                    if (copy_head == NULL) {
                        copy_head = nway_copy;
                        copy_tail = nway_copy;
                    } else {
                        copy_tail->next = nway_copy;
                        copy_tail = nway_copy;
                    }

                    curr = curr->next;
                }

            }

            // atach copies to the list
            if (copy_head != NULL) {
                tail->next = copy_head;
                tail = copy_tail;
            }
        }
    }

    struct int_list_list *curr = head;
    while (curr != NULL) {
        printf("%d\t", start_pos);
        int j;
        for (j = 0; j < num_sets; ++j) {
            if (j != 0)
                printf(" ");
            printf("%d", curr->list[j]);
        }
        printf("\n");
        curr->list[s_i] = ordering[s_i].end + 1;
        curr = curr->next;
    }
}
//}}}

//{{{ void print_nway( int num_sets,
void print_nway_o( int num_sets,
                 int s_i,
                 int start_pos,
                 struct pair *ordering)
{
    // Create the first nway list
    struct int_list_list *head = (struct int_list_list *)
            malloc(sizeof(struct int_list_list));
    struct int_list_list *tail = head;

    head->size = num_sets;
    head->list = (int *) malloc(head->size * sizeof(int));
    head->next = NULL;

    // Create nways from the orders before s_i
    int i;
    for (i = 0; i < s_i; i++) {

        // set the first item in the ordering
        struct int_list_list *curr = head;
        while (curr != NULL) {
            //printf("0\t%d %d\n", i, ordering[i].start);
            curr->list[i] = ordering[i].start;
            curr = curr->next;
        }

        // if there is more than one element in the ordering, then we
        // need to make copies of the nway lists up to this point
        // and set the values
        struct int_list_list *copy_head, *copy_tail;
        copy_head = NULL;

        int j;
        // make copies
        for (j = ordering[i].start + 1; j <= ordering[i].end; ++j) {
            struct int_list_list *curr = head;
            while (curr != NULL) {

                struct int_list_list *nway_copy = 
                        (struct int_list_list *)
                        malloc(sizeof(struct int_list_list));
                nway_copy->size = num_sets;
                nway_copy->list = (int *) 
                        malloc(head->size * sizeof(int));
                nway_copy->next = NULL;

                //copy elements from the current nway over
                memcpy(nway_copy->list,
                       curr->list,
                       num_sets*sizeof(int));

                //printf("1\t%d %d\n", i, j);
                nway_copy->list[i] = j;


                if (copy_head == NULL) {
                    copy_head = nway_copy;
                    copy_tail = nway_copy;
                } else {
                    copy_tail->next = nway_copy;
                    copy_tail = nway_copy;
                }

                curr = curr->next;
            }

        }

        // atach copies to the list
        if (copy_head != NULL) {
            tail->next = copy_head;
            tail = copy_tail;
        }
    }

    // Add the new item to the list
    struct int_list_list *curr = head;
    while (curr != NULL) {
        curr->list[s_i] = ordering[s_i].end + 1;
        curr = curr->next;
    }


    // Create nways from the orders before s_i
    for (i = s_i + 1; i < num_sets; i++) {
        // set the first item in the ordering
        struct int_list_list *curr = head;
        while (curr != NULL) {
            curr->list[i] = ordering[i].start;
            curr = curr->next;
        }

        // if there is more than one element in the ordering, then we
        // need to make copies of the nway lists up to this point
        // and set the values
        struct int_list_list *copy_head, *copy_tail;
        copy_head = NULL;

        int j;
        // make copies
        for (j = ordering[i].start + 1; j <= ordering[i].end; ++j) {

            struct int_list_list *curr = head;
            while (curr != NULL) {

                struct int_list_list *nway_copy = 
                        (struct int_list_list *)
                        malloc(sizeof(struct int_list_list));
                nway_copy->size = num_sets;
                nway_copy->list = (int *) 
                        malloc(head->size * sizeof(int));
                nway_copy->next = NULL;

                //copy elements from the current nway over
                memcpy(nway_copy->list,
                       curr->list,
                       num_sets*sizeof(int));

                nway_copy->list[i] = j;

                if (copy_head == NULL) {
                    copy_head = nway_copy;
                    copy_tail = nway_copy;
                } else {
                    copy_tail->next = nway_copy;
                    copy_tail = nway_copy;
                }

                curr = curr->next;
            }

        }

        // atach copies to the list
        if (copy_head != NULL) {
            tail->next = copy_head;
            tail = copy_tail;
        }

    }

    curr = head;
    while (curr != NULL) {
        printf("%d\t", start_pos);
        int j;
        for (j = 0; j < num_sets; ++j) {
            if (j != 0)
                printf(" ");
            printf("%d", curr->list[j]);
        }
        printf("\n");
        curr->list[s_i] = ordering[s_i].end + 1;
        curr = curr->next;
    }
}
//}}}

int main()
{

    int num_sets = 10;
    int num_elements = 5;
    int len = 10;

    struct interval *S[num_sets];
    struct pair ordering[num_sets];

    // Created the intervals in S
    int i,j;
    for (i = 0; i < num_sets; i++) {
        S[i] = (struct interval *)
               malloc(sizeof(struct interval) * num_elements);
        int last_start = 0;
        for (j = 0; j < num_elements; j++) {
            int space = rand() % 20;
            S[i][j].start = last_start + space;
            last_start = last_start + space;
            S[i][j].end = last_start + len;
        }
    }

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
        priq_push(q, set_ids[i], S[i][next[i]].start);
        next[i] += 1;
    }

    int scan = 1;
    while (scan == 1) {
        int start_pos;

        // get the next element to place into an ordering
        // s_i is the index of the set in S that will constribute the element
        // start_pos is the starting position of that element
        int s_i = priq_pop(q, &start_pos);

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
            priq_push(q, set_ids[s_i], S[s_i][next[s_i]].start);
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

            //print_nway(num_sets, s_i, start_pos, ordering);
            printf("%d\t%d\n",
                    start_pos,
                    count_nway(num_sets, s_i, start_pos, ordering));
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
