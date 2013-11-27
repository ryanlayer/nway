#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timer.h"
#include "pq.h"
#include "nway.h"
#include "timer.h"
#include "threadpool.h"
#include <pthread.h>
#include <sys/types.h>
#include <assert.h>
#include <limits.h>
#include <inttypes.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

//{{{ int b_search_starts(int key,
int64_t b_search_starts(int64_t key,
                    struct interval *S,
                    int64_t lo,
                    int64_t hi)
{
    int64_t mid = -1;
    while (hi - lo > 1) {
        mid = (hi + lo) / 2;

        if (S[mid].start < key)
            lo = mid;
        else
            hi = mid;
    }
    return hi;
#ifdef SPLIT_DEBUG
        fprintf(stderr, "\tb_search_starts\t"
                        "%" PRId64 " %" PRId64 " %" PRId64 " \t",
                        key,
                        hi,
                        S[hi].start);
#endif

}
//}}}

//{{{ int b_search_ends(int key,
int64_t b_search_ends(int64_t key,
                  struct interval *S,
                  int64_t lo,
                  int64_t hi)
{
    int64_t mid = -1;
    while (hi - lo > 1) {
        mid = (hi + lo) / 2;

#ifdef SPLIT_DEBUG
        fprintf(stderr, "k:%" PRId64 ","
                        "l:%" PRId64 ","
                        "h:%" PRId64 ","
                        "m:%" PRId64 ","
                        "S:%" PRId64 "\t",
                        key,
                        lo,
                        hi,
                        mid,
                        S[mid].end);
#endif




        if (S[mid].end < key)
            lo = mid;
        else
            hi = mid;
    }
    return hi;
}
//}}}

//{{{ void get_center_split(struct interval **S,
void get_center_split(struct interval **S,
                      int num_sets,
                      int *set_sizes,
                      struct interval root,
                      struct pair *centers,
                      int *is_empty)
{

    int i;
    *is_empty = 0;
    int center_is_empty = 0;
    for (i = 1; i < num_sets; ++i) {
        center_is_empty = 0;
        struct interval *s = S[i];
        struct pair dim;
        dim.start = 0;
        dim.end = set_sizes[i] - 1;

        get_center(root, s, dim, &centers[i], &center_is_empty);
        *is_empty = *is_empty + center_is_empty;
    }
}
//}}}

//{{{ void split_search_o(struct split_search_node *query,
void split_search_o(struct split_search_node *query,
                  struct split_search_node *left,
                  struct split_search_node *center,
                  struct split_search_node *right)
{

    // s_dim[0] corresponds to S_dim.start
    struct interval *a = query->S[query->S_dim.start];
    struct pair a_dim;
    //a_dim = query->s_dim[0];
    a_dim = query->s_dim[query->S_dim.start];
    int a_mid = (a_dim.end+1 + a_dim.start-1)/2;
    struct interval root = a[a_mid];

    //int num_sets = query->S_dim.end - query->S_dim.start + 1;
    int num_sets = query->S_dim.end + 1;


#ifdef SPLIT_DEBUG
        fprintf(stderr, "pivot interval\t"
                        "S:%" PRId64 "\t"
                        "a_mid:%d\t"
                        "root.start:%" PRId64 "\t"
                        "root.end:%" PRId64 "\t",
                        query->S_dim.start,
                        a_mid,
                        root.start,
                        root.end);
            
#endif

    /* left and right have the same parent as the query and consider the
     * same range in S, but the subsets of each S[i] (values in s_dim)
     * will be different
     */
    left->S = query->S;
    left->S_dim = query->S_dim;
    //left->parent = query->parent;
    left->s_dim = (struct pair *) malloc (num_sets*sizeof(struct pair));
    memcpy(left->s_dim, query->s_dim, num_sets*sizeof(struct pair));
    //left->s_dim[0].start = a_dim.start;
    //left->s_dim[0].end = a_mid - 1;
    left->s_dim[query->S_dim.start].start = a_dim.start;
    left->s_dim[query->S_dim.start].end = a_mid - 1;

    //left->has_empty = left->s_dim[0].start > left->s_dim[0].end;
    left->has_empty = left->s_dim[query->S_dim.start].start > 
                      left->s_dim[query->S_dim.start].end;

    right->S = query->S;
    right->S_dim = query->S_dim;
    right->s_dim = (struct pair *) malloc (num_sets*sizeof(struct pair));
    memcpy(right->s_dim, query->s_dim, num_sets*sizeof(struct pair));
    //right->parent = query->parent;
    //right->s_dim[0].start = a_mid + 1;
    //right->s_dim[0].end = a_dim.end;
    right->s_dim[query->S_dim.start].start = a_mid + 1;
    right->s_dim[query->S_dim.start].end = a_dim.end;

    //right->has_empty = right->s_dim[0].start > right->s_dim[0].end;
    right->has_empty = right->s_dim[query->S_dim.start].start > 
                       right->s_dim[query->S_dim.start].end;

    center->S = query->S;
    center->S_dim.start = query->S_dim.start + 1;
    center->S_dim.end = query->S_dim.end;
    //center->s_dim = (struct pair *) malloc 
            //((num_sets-1)*sizeof(struct pair));
    //center->s_dim = (struct pair *)malloc((num_sets-1)*sizeof(struct pair));
    center->s_dim = (struct pair *)malloc((num_sets)*sizeof(struct pair));
    memcpy(center->s_dim, query->s_dim, num_sets*sizeof(struct pair));
    center->s_dim[query->S_dim.start].start = a_mid;
    center->s_dim[query->S_dim.start].end = a_mid;
    //center->parent = query;

    int i;
    for (i = query->S_dim.start + 1; i <= query->S_dim.end; ++i) {
    //for (i = 1; i < query->S_dim.end - query->S_dim.start + 1; ++i) {
        //struct interval *s = query->S[i + query->S_dim.start];
        struct interval *s = query->S[i];
        struct pair s_dim = query->s_dim[i];

        struct pair s_center;
        int center_is_empty = 0;
        struct pair s_left;
        int left_is_empty = 0;
        struct pair s_right;
        int right_is_empty = 0;

        get_left_center_right(a,
                              a_mid,
                              a_dim, 
                              s, 
                              s_dim, 
                              &s_center, 
                              &center_is_empty, 
                              &s_left, 
                              &left_is_empty, 
                              &s_right, 
                              &right_is_empty);

        left->s_dim[i].start = s_left.start;
        left->s_dim[i].end = s_left.end;
        left->has_empty += left_is_empty;

        right->s_dim[i].start = s_right.start;
        right->s_dim[i].end = s_right.end;
        right->has_empty += right_is_empty;

        center->s_dim[i].start = s_center.start;
        center->s_dim[i].end = s_center.end;
        center->has_empty += center_is_empty;

#ifdef SPLIT_DEBUG
        if (i != query->S_dim.start + 1)
            fprintf(stderr,"\t");
        //fprintf(stderr, "%d%d%d",
                //left_is_empty,center_is_empty,right_is_empty);

        fprintf(stderr, 
                "%"PRId64",%"PRId64" "
                "%"PRId64",%"PRId64" "
                "%"PRId64",%"PRId64,

                    s_left.start,
                    s_left.end,
                    s_center.start,
                    s_center.end,
                    s_right.start,
                    s_right.end);




#endif
    }

#ifdef SPLIT_DEBUG
            fprintf(stderr,"\n");
#endif
}
//}}}

//{{{ void split_search(struct split_search_node *query,
void split_search(struct split_search_node *query,
                  struct split_search_node *left,
                  struct split_search_node *center,
                  struct split_search_node *right)
{

    // s_dim[0] corresponds to S_dim.start
    struct interval *a = query->S[query->S_dim.start];
    struct pair a_dim;
    a_dim = query->s_dim[0];
    int64_t a_mid = (a_dim.end+1 + a_dim.start-1)/2;
    struct interval root = a[a_mid];

    int num_sets = query->S_dim.end - query->S_dim.start + 1;

    /* left and right have the same parent as the query and consider the
     * same range in S, but the subsets of each S[i] (values in s_dim)
     * will be different
     */
    left->S = query->S;
    left->S_dim = query->S_dim;
    left->parent = query->parent;
    left->s_dim = (struct pair *) malloc (num_sets*sizeof(struct pair));
    left->s_dim[0].start = a_dim.start;
    left->s_dim[0].end = a_mid - 1;

    left->has_empty = left->s_dim[0].start > left->s_dim[0].end;

    right->S = query->S;
    right->S_dim = query->S_dim;
    right->s_dim = (struct pair *) malloc (num_sets*sizeof(struct pair));
    right->parent = query->parent;
    right->s_dim[0].start = a_mid + 1;
    right->s_dim[0].end = a_dim.end;

    right->has_empty = right->s_dim[0].start > right->s_dim[0].end;

    center->S = query->S;
    center->S_dim.start = query->S_dim.start + 1;
    center->S_dim.end = query->S_dim.end;
    center->s_dim = (struct pair *) malloc 
            ((num_sets-1)*sizeof(struct pair));
    center->parent = query;

    int i;
    //for (i = query->S_dim.start + 1; i <= query->S_dim.end; ++i) {
    for (i = 1; i < query->S_dim.end - query->S_dim.start + 1; ++i) {
        struct interval *s = query->S[i + query->S_dim.start];
        struct pair s_dim = query->s_dim[i];

        struct pair s_center;
        int center_is_empty = 0;
        struct pair s_left;
        int left_is_empty = 0;
        struct pair s_right;
        int right_is_empty = 0;

        get_left_center_right(a,
                              a_mid,
                              a_dim, 
                              s, 
                              s_dim, 
                              &s_center, 
                              &center_is_empty, 
                              &s_left, 
                              &left_is_empty, 
                              &s_right, 
                              &right_is_empty);

        left->s_dim[i].start = s_left.start;
        left->s_dim[i].end = s_left.end;
        left->has_empty += left_is_empty;

        right->s_dim[i].start = s_right.start;
        right->s_dim[i].end = s_right.end;
        right->has_empty += right_is_empty;

        center->s_dim[i-1].start = s_center.start;
        center->s_dim[i-1].end = s_center.end;
        center->has_empty += center_is_empty;

        /*
        printf("\tl:%d %d %d\tc:%d %d %d\tr:%d %d %d\n", 
                            s_left.start, s_left.end, left_is_empty,
                            s_center.start, s_center.end, center_is_empty,
                            s_right.start, s_right.end, right_is_empty);
        */
    }
}
//}}}

//{{{ void get_center(struct interval *a,
void get_center(struct interval root,
                struct interval *s,
                struct pair s_dim,
                struct pair *s_center,
                int *center_is_empty)
{
    // s_left_i is the index of the last interval to end before
    // the current interval (root) starts
    int s_left_i = b_search_ends(root.start,
                                 s,
                                 s_dim.start - 1,
                                 s_dim.end + 1) 
                    - 1;

    // s_right_i is the index of the first interval to start after
    // the current interval (root) ends
    int s_right_i = b_search_starts(root.end,
                                    s,
                                    s_dim.start - 1,
                                    s_dim.end + 1);

    if ( (s_right_i <= s_dim.end) && (root.end == s[s_right_i].start) )
        ++s_right_i;

    // s_left_i and s_right_i do not intersect root
    // the current interval (root) intersects anything in the inclusive
    // range of s_left_i+1,s_right_i-1
    s_center->start = s_left_i + 1;
    s_center->end = s_right_i - 1;

    // if s_center.start > s_center.end, then the intersection is empty
    if (s_center->start > s_center->end)
        *center_is_empty = 1;
}
//}}} 

//{{{ void get_left_center_right(struct interval *a,
void get_left_center_right(struct interval *a,
                           int64_t a_mid,
                           struct pair a_dim,
                           struct interval *s,
                           struct pair s_dim,
                           struct pair *s_center,
                           int *center_is_empty,
                           struct pair *s_left,
                           int *left_is_empty,
                           struct pair *s_right,
                           int *right_is_empty)
{

    struct interval root = a[a_mid];

    // s_left_i is the index of the last interval to end before
    // the current interval (root) starts
    int64_t s_left_i = b_search_ends(root.start,
                                 s,
                                 s_dim.start - 1,
                                 s_dim.end + 1) 
                    - 1;


    // s_right_i is the index of the first interval to start after
    // the current interval (root) ends
    int64_t s_right_i = b_search_starts(root.end,
                                    s,
                                    s_dim.start - 1,
                                    s_dim.end + 1);

#ifdef SPLIT_DEBUG
#endif


    if ( (s_right_i <= s_dim.end) && (root.end == s[s_right_i].start) )
        ++s_right_i;

    // s_left_i and s_right_i do not intersect root
    // the current interval (root) intersects anything in the inclusive
    // range of s_left_i+1,s_right_i-1
    s_center->start = s_left_i + 1;
    s_center->end = s_right_i - 1;

    // if s_center.start > s_center.end, then the intersection is empty
    if (s_center->start > s_center->end)
        *center_is_empty = 1;

    // We now create sublist for future searches.  The base of those
    // searches will be everyting to the left (a_left =
    // a[a_dim.start]..a[a_mid -1]) and everything to the right of root
    // ( a_right = a[mid+1]..a[a_dim.end]).  We need to find the
    // sublists of s that are guanteed to include the intevarls that
    // will intersect a_left and a_right.  In the simple case, s_left =
    // s[s_dim.start]..s[s_center.start - 1] but it is possible that
    // a[mid-1] intersects s_center.start and that a[mid+1] intersect
    // s_center.end
    // Example:
    //      mid-1  mid    mid+1
    // a:   -----  ----   ------
    // s: ---  -------------  ----
    //    ^s_end_i            ^s_start_i
    s_left->start = s_dim.start;
    s_left->end = s_right_i - 1;

    /*
    if ( (a_mid > a_dim.start) && // bound check
            (s_center->start >= s_dim.start) && //bound check
            (s_center->start <= s_dim.end) && //bound check
            (a[a_mid - 1].end >= s[s_center->start].start) )
        s_left->end = s_center->start;
    */

    if (s_left->start > s_left->end)
        *left_is_empty = 1;

    s_right->start = s_left_i + 1;
    s_right->end = s_dim.end;

    /*
    if ( (a_mid < a_dim.end) && // bound check
            (s_center->start >= s_dim.start) && //bound check
            (s_center->end <= s_dim.end) && //bound check
            (a[a_mid + 1].start <= s[s_center->end].end) )
        s_right->start = s_center->end;
    */

    if (s_right->start > s_right->end)
        *right_is_empty = 1;

    /*
    printf("(%d,%d)", s_left->start, s_left->end);
    printf("(%d,%d)", s_center->start, s_center->end);
    printf("(%d,%d)", s_right->start, s_right->end);
    printf("\n");
    */
}
//}}} 

//{{{ int get_nway_sweep_list( int num_sets,
int get_nway_sweep_list(int num_sets,
                         int s_i,
                         struct pair *ordering,
                         struct int_list_list **r_head,
                         struct int_list_list **r_tail)
{
    int N = 1;
    // Create the first nway list
    struct int_list_list *head = (struct int_list_list *)
            malloc(sizeof(struct int_list_list));
    struct int_list_list *tail = head;

    head->size = num_sets;
    head->list = (int64_t *) malloc(head->size * sizeof(int64_t));
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
                N+=1;
                struct int_list_list *curr = head;
                while (curr != NULL) {

                    struct int_list_list *nway_copy = 
                            (struct int_list_list *)
                            malloc(sizeof(struct int_list_list));
                    nway_copy->size = num_sets;
                    nway_copy->list = (int64_t *) 
                            malloc(head->size * sizeof(int64_t));
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

    *r_head = head;
    *r_tail = tail;

    return N;
}
//}}}

//{{{ void sweep(struct interval **S,
void sweep(struct interval **S,
           int *set_sizes,
          int num_sets,
           struct int_list_list **R,
           int *num_R)
{

#ifdef IN_TIME_SPLIT
    start();
#endif

    *num_R = 0;

    struct int_list_list *nways_head, *nways_tail;
    nways_head = NULL;
    struct pair ordering[num_sets];

    int i;
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
        int64_t start_pos;

        // get the next element to place into an ordering
        // s_i is the index of the set in S that will constribute the
        // element start_pos is the starting position of that element
        int *s_i_p = priq_pop(q, &start_pos);
        int s_i = *s_i_p;

        // remove anything from the orderings that ends before start_pos
        for (i = 0; i < num_sets; i++) {
            int j;
            for (j = ordering[i].start; j <= ordering[i].end; ++j) {
                if (S[i][j].end < start_pos)  {
                    ordering[i].start += 1;
                }
            }
        }

        // Make sure that there is somethin left to push before pushing
        // the next elemetn from S[s_i]
        if (next[s_i] < set_sizes[s_i]) {
            priq_push(q, &set_ids[s_i], S[s_i][next[s_i]].start);
            next[s_i] += 1;
        }

        // Check to see if adding this element causes an n-way
        // intersection
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
            struct int_list_list *r_head, *r_tail;

            *num_R += get_nway_sweep_list(num_sets,
                                s_i,
                                ordering,
                                &r_head,
                                &r_tail);

            if (nways_head == NULL) {
                nways_head = r_head;
                nways_tail = r_tail;
            } else {
                nways_tail->next = r_head;
                nways_tail = r_tail;
            }
        }

        // Add the element to the ordering for that set
        ordering[s_i].end+=1;

        // check to see if we can stop scanning To stop scanning the last
        // element in a set must have been moved out of context
        for (i = 0; i < num_sets; i++) {
            if (ordering[i].start >= set_sizes[i]) {
                scan = 0;
            }
        }

        // we can also stop scanning if there are no other intervals to add
        if (q->n == 1) {
            scan = 0;
        }
    }

    *R = nways_head;

    priq_free(q);

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int sweep_time = report();
#endif

#ifdef IN_TIME_SPLIT
    printf("sweep:%lu\ttotal:%lu\n", sweep_time,sweep_time);
#endif




}
//}}}

//{{{ void sweep_subset(struct interval **S,
void sweep_subset(struct interval **S,
                  int num_sets,
                  struct pair *s_dim,
                  struct int_list_list **R_head,
                  struct int_list_list **R_tail,
                  int *num_R)
{
    *num_R = 0;

    struct int_list_list *nways_head, *nways_tail;
    nways_head = NULL;
    struct pair ordering[num_sets];

    int i;
    // Initialize the ordering for S
    for (i = 0; i < num_sets; i++) {
        //ordering[i].start = 0;
        //ordering[i].end = -1;
        ordering[i].start = s_dim[i].start;
        ordering[i].end = s_dim[i].start - 1;
    }

    int next[num_sets];
    // next will be the first interval defined by s_dim[i].start
    for (i = 0; i < num_sets; i++) {
        //next[i] = 0;
        next[i] = s_dim[i].start;
        //printf("%d %d\t", s_dim[i].start, s_dim[i].end);
    }
    //printf("\n");

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
        int64_t start_pos;

        // get the next element to place into an ordering
        // s_i is the index of the set in S that will constribute the
        // element start_pos is the starting position of that element
        int *s_i_p = priq_pop(q, &start_pos);
        int s_i = *s_i_p;
        //printf("%d %d\n", s_i, start_pos);

        // remove anything from the orderings that ends before start_pos
        for (i = 0; i < num_sets; i++) {
            int j;
            for (j = ordering[i].start; j <= ordering[i].end; ++j) {
                if (S[i][j].end < start_pos)  {
                    //printf("rem:%d\t%d %d\n",i,S[i][j].start,S[i][j].end);
                    ordering[i].start += 1;
                }
            }
        }

        // Make sure that there is somethin left to push before pushing
        // the next elemetn from S[s_i]
        //if (next[s_i] < set_sizes[s_i]) {
        // s_dim is an inclusive range
        if (next[s_i] <= s_dim[s_i].end) {
            priq_push(q, &set_ids[s_i], S[s_i][next[s_i]].start);
            next[s_i] += 1;
        }

        // Check to see if adding this element causes an n-way
        // intersection
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
            struct int_list_list *r_head, *r_tail;

            *num_R += get_nway_sweep_list(num_sets,
                                s_i,
                                ordering,
                                &r_head,
                                &r_tail);

            if (nways_head == NULL) {
                nways_head = r_head;
                nways_tail = r_tail;
            } else {
                nways_tail->next = r_head;
                nways_tail = r_tail;
            }
        }

        // Add the element to the ordering for that set
        ordering[s_i].end+=1;

        // check to see if we can stop scanning To stop scanning the last
        // element in a set must have been moved out of context
        for (i = 0; i < num_sets; i++) {
            //if (ordering[i].start >= set_sizes[i]) {
            if (ordering[i].start > s_dim[i].end) {
                scan = 0;
            }
        }

        // we can also stop scanning if there are no other intervals to add
        if (q->n == 1) {
            scan = 0;
        }
    }

    *R_head = nways_head;
    *R_tail = nways_tail;

    priq_free(q);
}
//}}}

//{{{ void split_o(struct interval **S,
void split_o(struct interval **S,
           int *set_sizes,
           int num_sets,
           struct int_list_list **R)
{
    struct split_search_node_list *leaf_head, *leaf_tail;
    struct split_search_node_list *to_clear_head, *to_clear_tail;

#ifdef IN_TIME_SPLIT
    start();
#endif
    split_sets_o(S,
               set_sizes,
               &to_clear_head,
               &to_clear_tail,
               &leaf_head,
               &leaf_tail,
               num_sets);

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int split_sets_time = report();
#endif

    struct int_list_list *R_head;

#ifdef IN_TIME_SPLIT
    start();
#endif

    int count = build_split_nway_o(leaf_head, &R_head, num_sets);

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int build_split_nway_time = report();
#endif
   

#ifdef IN_TIME_SPLIT
    unsigned long int total_time = split_sets_time + 
                                   build_split_nway_time;
    printf("split:%lu\tbuild:%lu\ttotal:%lu\t"
           "%f\t%f\n", 
                    split_sets_time,
                    build_split_nway_time,
                    total_time,
                   ( ((double)split_sets_time) / ((double)total_time)),
                   ( ((double)build_split_nway_time) / ((double)total_time)));
    
#endif

    *R = R_head;

}
//}}}

//{{{ void psplit_o(struct interval **S,
void psplit_o(struct interval **S,
              int *set_sizes,
              int num_sets,
              struct int_list_list **R,
              int num_threads)
{
    struct split_search_node_list *leaf_head, *leaf_tail;
    struct split_search_node_list *to_clear_head, *to_clear_tail;

#ifdef IN_TIME_SPLIT
    start();
#endif
    psplit_sets_o(S,
                  set_sizes,
                  &leaf_head,
                  &leaf_tail,
                  num_sets,
                  num_threads);

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int split_sets_time = report();
#endif

    struct int_list_list *R_head;

#ifdef IN_TIME_SPLIT
    start();
#endif
    //int count = build_split_nway_o(leaf_head, &R_head, num_sets);

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int build_split_nway_time = report();
#endif

#ifdef IN_TIME_SPLIT
    unsigned long int total_time = split_sets_time + 
                                   build_split_nway_time;
    printf("split:%lu\t"
           "build:%lu\t"
           "total:%lu\t"
           "%f\t%f\n",
                split_sets_time,
                build_split_nway_time,
                total_time,
                ( ((double)split_sets_time) / ((double)total_time)),
                ( ((double)build_split_nway_time) / ((double)total_time)));
    
#endif

    *R = R_head;
}
//}}}

//{{{ void split(struct interval **S,
void split(struct interval **S,
           int *set_sizes,
           int num_sets,
           struct int_list_list **R)
{
    struct split_search_node_list *leaf_head, *leaf_tail;
    struct split_search_node_list *to_clear_head, *to_clear_tail;

#ifdef IN_TIME_SPLIT
    start();
#endif
    split_sets(S,
               set_sizes,
               &to_clear_head,
               &to_clear_tail,
               &leaf_head,
               &leaf_tail,
               num_sets);

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int split_sets_time = report();
#endif

    struct int_list_list *R_head;

#ifdef IN_TIME_SPLIT
    start();
#endif

    int count = build_split_nway(leaf_head, &R_head, num_sets);

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int build_split_nway_time = report();
#endif

    // free up space
#ifdef IN_TIME_SPLIT
    start();
#endif
    struct split_search_node_list *curr_clear = to_clear_head;
    while (curr_clear != NULL) {
        struct split_search_node_list *next_clear = curr_clear->next;
        free_split_search_node(curr_clear->node);
        free(curr_clear);
        curr_clear = next_clear;
    }
#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int free_time = report();
#endif
    

#ifdef IN_TIME_SPLIT
    unsigned long int total_time = split_sets_time + 
                                   build_split_nway_time +
                                   free_time;
    printf("split:%lu\tbuild:%lu\tfree:%lu\ttotal:%lu\t"
           "%f\t%f\t%f\n", split_sets_time,
                           build_split_nway_time,
                           free_time,
                           total_time,
                   ( ((double)split_sets_time) / ((double)total_time)),
                   ( ((double)build_split_nway_time) / ((double)total_time)),
                   ( ((double)free_time) / ((double)total_time)));
    
#endif


    *R = R_head;

}
//}}}

//{{{ void split_sweep(struct interval **S,
void split_sweep(struct interval **S,
                 int *set_sizes,
                 int num_sets,
                 struct int_list_list **R)
{
    struct split_search_node_list *leaf_head, *leaf_tail;
    struct split_search_node_list *to_clear_head, *to_clear_tail;

#ifdef IN_TIME_SPLIT
    start();
#endif
    l1_split_sets_o(S,
                    set_sizes,
                    &to_clear_head,
                    &to_clear_tail,
                    &leaf_head,
                    &leaf_tail,
                    num_sets);
#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int split_sets_time = report();
#endif

#ifdef IN_TIME_SPLIT
    start();
#endif
    struct split_search_node_list *curr = leaf_head;

    struct int_list_list *R_head=NULL, *R_tail=NULL;
    while (curr != NULL) {
        struct split_search_node *node = curr->node;
        struct int_list_list *curr_head, *curr_tail;

        int num_R;
        sweep_subset(S, num_sets, node->s_dim, &curr_head, &curr_tail, &num_R);

        if (num_R > 0) {
            if (R_head == NULL) {
                R_head = curr_head;
                R_tail = curr_tail;
            } else {
                R_tail->next = curr_head;
                R_tail = curr_tail;
            }
        }

        curr = curr->next;
    }
#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int sweep_sets_time = report();
#endif

#ifdef IN_TIME_SPLIT
    unsigned long int total_time = split_sets_time + 
                                   sweep_sets_time;
    printf("split:%lu\tsweep:%lu\ttotal:%lu\t"
           "%f\t%f\n", split_sets_time,
                           sweep_sets_time,
                           total_time,
                   ( ((double)split_sets_time) / ((double)total_time)),
                   ( ((double)sweep_sets_time) / ((double)total_time)));
    
#endif

    *R = R_head;

}
//}}}

//{{{ void split_psweep(struct interval **S,
void split_psweep(struct interval **S,
                 int *set_sizes,
                 int num_sets,
                 struct int_list_list **R,
                 int num_threads,
                 int step_size)
{
    struct split_search_node_list *leaf_head, *leaf_tail;
    struct split_search_node_list *to_clear_head, *to_clear_tail;

#ifdef IN_TIME_SPLIT
    start();
#endif
    l1_split_sets_o(S,
                    set_sizes,
                    &to_clear_head,
                    &to_clear_tail,
                    &leaf_head,
                    &leaf_tail,
                    num_sets);
#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int split_sets_time = report();
#endif

#ifdef IN_TIME_SPLIT
    start();
#endif

    psweep_subset(S,
                  num_sets,
                  set_sizes,
                  leaf_head,
                  R,
                  num_threads,
                  step_size);

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int sweep_sets_time = report();
#endif

#ifdef IN_TIME_SPLIT
    unsigned long int total_time = split_sets_time + 
                                   sweep_sets_time;
    printf("split:%lu\tsweep:%lu\ttotal:%lu\t"
           "%f\t%f\n", split_sets_time,
                           sweep_sets_time,
                           total_time,
                   ( ((double)split_sets_time) / ((double)total_time)),
                   ( ((double)sweep_sets_time) / ((double)total_time)));
    
#endif

}
//}}}

//{{{void psweep_centers(struct interval **S,
void psweep_subset(struct interval **S,
                   int num_sets,
                   int *set_sizes, 
                   struct split_search_node_list *subset_head,
                   struct int_list_list **R,
                   int num_threads,
                   int step_size)
{
    pthread_t threads[num_threads];
    struct run_sweep_subset_args thread_args[num_threads];
    int i, rc;

    for (i = 0; i < num_threads; ++i) {
        thread_args[i].S = S;
        thread_args[i].num_sets = num_sets;
        thread_args[i].step_size = step_size;
        thread_args[i].subset_head = subset_head;
        thread_args[i].num_threads = num_threads;
        thread_args[i].id = i;
        thread_args[i].R_head = NULL;
        thread_args[i].R_tail = NULL;

        //printf("%d %d %d\n", i, thread_args[i].start, thread_args[i].end);

        rc = pthread_create(&threads[i],
                            NULL,
                            run_sweep_subset,
                            (void *) &thread_args[i]);

        assert( 0 == rc);
    }

    for (i = 0; i < num_threads; ++i) {
        rc = pthread_join(threads[i], NULL);
        assert( 0 == rc);
    }

    struct int_list_list *R_head=NULL, *R_tail=NULL;

    for (i = 0; i < num_threads; ++i) {
        if (thread_args[i].R_head != NULL) {
            if (R_head == NULL) {
                R_head = thread_args[i].R_head;
                R_tail = thread_args[i].R_tail;
            } else {
                R_tail->next = thread_args[i].R_head;
                R_tail = thread_args[i].R_tail;
            }
        }
    }

    *R = R_head;
}
//}}}

//{{{ void *run_sweep_subset(void *arg)
void *run_sweep_subset(void *arg)
{
    struct run_sweep_subset_args *p = ((struct run_sweep_subset_args *) arg);

    p->R_head = NULL;
    p->R_tail = NULL;

    // skip to first, id*step_size is the first node to consider
    int node_id = 0;
    struct split_search_node_list *curr = p->subset_head;

    while ( (curr != NULL) && (node_id < (p->id*p->step_size)) ){
        curr = curr->next;
        node_id += 1;
    }

    // sweep the current regions, then move to the next
    int num_to_sweep = p->step_size, num_to_skip, num_R;

    num_to_skip = (p->num_threads - 1) * p->step_size;

    /*
    fprintf(stderr,"id:%d\tto_skip:%d\tstep:%d\t"
            "num_to_sweep:%d\tnum_to_skip:%d\n",
            p->id,
            p->id*p->step_size,
            p->step_size,
            num_to_sweep,
            num_to_skip);
    */

    num_to_skip = 0;

    while (curr != NULL) {

        if (num_to_skip > 0) {
            fprintf(stderr,"id:%d\tto_skip:%d\n",p->id, num_to_skip);
            num_to_skip -= 1;
        } else if (num_to_skip == 0) {
            num_to_sweep = p->step_size;
            num_to_skip = -1;
        }

        //sweep the current region
        if (num_to_sweep > 0) {
            fprintf(stderr,"id:%d\tto_sweep:%d\t%p\n",
                        p->id,
                        num_to_sweep,
                        curr);

            struct int_list_list *curr_head, *curr_tail;
            sweep_subset(p->S,
                         p->num_sets,
                         curr->node->s_dim,
                         &curr_head,
                         &curr_tail,
                         &num_R);
            if (num_R > 0) {
                if (p->R_head == NULL) {
                    p->R_head = curr_head;
                    p->R_tail = curr_tail;
                } else {
                    p->R_tail->next = curr_head;
                    p->R_tail = curr_tail;
                }
            }

            num_to_sweep -= 1;
        }  else if (num_to_sweep == 0) {
            num_to_skip = (p->num_threads - 1) * p->step_size;
            num_to_sweep = -1;
        }

        curr = curr->next;
    }
}
//}}}

//{{{ void split_centers(struct interval **S,
void split_centers(struct interval **S,
                 int *set_sizes,
                 int num_sets,
                 struct int_list_list **R)
{
    // alloc a long array of pairs to hold all of the center slices
    // for each element in the first set
    struct pair *centers = (struct pair*)
            malloc(num_sets*set_sizes[0]*sizeof(struct split_search_node));
    int *empties = (int *) malloc(set_sizes[0]*sizeof(int));

#ifdef IN_TIME_SPLIT
    start();
#endif

    l1_split_sets_centers (S,
                           set_sizes,
                           num_sets,
                           centers,
                           empties);
#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int split_sets_time = report();
#endif

#ifdef IN_TIME_SPLIT
    start();
#endif
    int i;
    struct int_list_list *R_head=NULL, *R_tail=NULL;
    for(i = 0; i < set_sizes[0]; ++i) {
        if (empties[i] == 0) {
            /*   
            int j;
            for(j = 0; j < num_sets; ++j) {
                printf("%d %d\t", centers[j+i*num_sets].start,
                                  centers[j+i*num_sets].end);
            }
            printf("\n");
            */ 
            int num_R;
            struct int_list_list *curr_head, *curr_tail;
            sweep_subset(S,
                         num_sets,
                         &centers[i*num_sets],
                         &curr_head,
                         &curr_tail,
                         &num_R);

            if (num_R > 0) {
                if (R_head == NULL) {
                    R_head = curr_head;
                    R_tail = curr_tail;
                } else {
                    R_tail->next = curr_head;
                    R_tail = curr_tail;
                }
            }
        }
    }

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int sweep_sets_time = report();
#endif

#ifdef IN_TIME_SPLIT
    unsigned long int total_time = split_sets_time + 
                                   sweep_sets_time;
    printf("split:%lu\tsweep:%lu\ttotal:%lu\t"
           "%f\t%f\n", split_sets_time,
                           sweep_sets_time,
                           total_time,
                   ( ((double)split_sets_time) / ((double)total_time)),
                   ( ((double)sweep_sets_time) / ((double)total_time)));
    
#endif

    *R = R_head;
}
//}}}

//{{{ void psplit_sweep(struct interval **S,
void psplit_centers(struct interval **S,
                 int *set_sizes,
                 int num_sets,
                 struct int_list_list **R,
                 int num_threads)
{
    // alloc a long array of pairs to hold all of the center slices
    // for each element in the first set
    struct pair *centers = (struct pair*)
            malloc(num_sets*set_sizes[0]*sizeof(struct split_search_node));
    int *empties = (int *) malloc(set_sizes[0]*sizeof(int));

#ifdef IN_TIME_SPLIT
    start();
#endif
    pl1_split_sets_centers (S,
                           set_sizes,
                           num_sets,
                           centers,
                           empties,
                           num_threads);
#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int split_sets_time = report();
#endif

#ifdef IN_TIME_SPLIT
    start();
#endif
    psweep_centers(S, num_sets, set_sizes, centers, empties, R, num_threads);

#ifdef IN_TIME_SPLIT
    stop();
    unsigned long int sweep_sets_time = report();
#endif

#ifdef IN_TIME_SPLIT
    unsigned long int total_time = split_sets_time + 
                                   sweep_sets_time;
    printf("split:%lu\tsweep:%lu\ttotal:%lu\t"
           "%f\t%f\n", split_sets_time,
                           sweep_sets_time,
                           total_time,
                   ( ((double)split_sets_time) / ((double)total_time)),
                   ( ((double)sweep_sets_time) / ((double)total_time)));
    
#endif

}
//}}}

//{{{void psweep_centers(struct interval **S,
void psweep_centers(struct interval **S,
                    int num_sets,
                    int *set_sizes, 
                    struct pair *centers,
                    int *empties,
                    struct int_list_list **R,
                    int num_threads)
{
    pthread_t threads[num_threads];
    struct run_sweep_center_args thread_args[num_threads];
    int step_size = (set_sizes[0] + num_threads - 1) / num_threads;
    int i, rc;

    for (i = 0; i < num_threads; ++i) {
        thread_args[i].S = S;
        thread_args[i].num_sets = num_sets;
        thread_args[i].centers = centers;
        thread_args[i].empties = empties;
        thread_args[i].start = i*step_size;
        thread_args[i].end = MIN(i*step_size + step_size, set_sizes[0]);
        thread_args[i].R_head = NULL;
        thread_args[i].R_tail = NULL;

        //printf("%d %d %d\n", i, thread_args[i].start, thread_args[i].end);

        rc = pthread_create(&threads[i],
                            NULL,
                            run_sweep_center,
                            (void *) &thread_args[i]);

        assert( 0 == rc);
    }

    for (i = 0; i < num_threads; ++i) {
        rc = pthread_join(threads[i], NULL);
        assert( 0 == rc);
    }

    struct int_list_list *R_head=NULL, *R_tail=NULL;

    for (i = 0; i < num_threads; ++i) {
        if (thread_args[i].R_head != NULL) {
            if (R_head == NULL) {
                R_head = thread_args[i].R_head;
                R_tail = thread_args[i].R_tail;
            } else {
                R_tail->next = thread_args[i].R_head;
                R_tail = thread_args[i].R_tail;
            }
        }
    }

    *R = R_head;
}
//}}}

//{{{ void *run_sweep_center(void *arg)
void *run_sweep_center(void *arg)
{
    struct run_sweep_center_args *p = ((struct run_sweep_center_args *) arg);

    p->R_head = NULL;
    p->R_tail = NULL;

    int i;
    for (i = p->start; i < p->end; ++i) {
        if (p->empties[i] == 0) {
            int num_R;
            struct int_list_list *curr_head, *curr_tail;
            sweep_subset(p->S,
                         p->num_sets,
                         &(p->centers[i*p->num_sets]),
                         &curr_head,
                         &curr_tail,
                         &num_R);
            if (num_R > 0) {
                if (p->R_head == NULL) {
                    p->R_head = curr_head;
                    p->R_tail = curr_tail;
                } else {
                    p->R_tail->next = curr_head;
                    p->R_tail = curr_tail;
                }
            }
        }
    }
}
//}}}

//{{{ void pl1_split_sets_centers (struct interval **S,
void pl1_split_sets_centers (struct interval **S,
                            int *set_sizes,
                            int num_sets,
                            struct pair *centers,
                            int *empties,
                            int num_threads)
{
    pthread_t threads[num_threads];
    struct get_center_split_args thread_args[num_threads];
    int step_size = (set_sizes[0] + num_threads - 1) / num_threads;
    int i, rc;

    for (i = 0; i < num_threads; ++i) {
        thread_args[i].S = S;
        thread_args[i].num_sets = num_sets;
        thread_args[i].set_sizes = set_sizes;
        thread_args[i].centers = centers;
        thread_args[i].empties = empties;
        thread_args[i].start = i*step_size;
        thread_args[i].end = MIN(i*step_size + step_size, set_sizes[0]);

        //printf("%d %d %d\n", i, thread_args[i].start, thread_args[i].end);

        rc = pthread_create(&threads[i],
                            NULL,
                            run_get_center_split,
                            (void *) &thread_args[i]);

        assert( 0 == rc);
    }

    for (i = 0; i < num_threads; ++i) {
        rc = pthread_join(threads[i], NULL);
        assert( 0 == rc);
    }
}
//}}}

//{{{void run_get_center_split(void *arg)
void *run_get_center_split(void *arg)
{
    struct get_center_split_args *p = ((struct get_center_split_args *) arg);

    int i;
    for (i = p->start; i < p->end; ++i) {
        p->centers[i*p->num_sets].start = i;        
        p->centers[i*p->num_sets].end = i;        

        get_center_split(p->S,
                         p->num_sets,
                         p->set_sizes,
                         p->S[0][i],
                         &(p->centers[i*p->num_sets]),
                         &(p->empties[i]));
    }
}
//}}}

//{{{ void l1_split_sets_centers (struct interval **S,
void l1_split_sets_centers (struct interval **S,
                            int *set_sizes,
                            int num_sets,
                            struct pair *centers,
                            int *empties)
{
    int i;
    // for each element in S[0], get the center split
    for (i = 0; i < set_sizes[0]; ++i) {
        // the first element in the center is going to be this element
        centers[i*num_sets].start = i;        
        centers[i*num_sets].end = i;        
        get_center_split(S,
                         num_sets,
                         set_sizes,
                         S[0][i],
                         &centers[i*num_sets],
                         &empties[i]);
    }
}
//}}}

//{{{void l1_split_sets_o (struct interval **S,
void l1_split_sets_o (struct interval **S,
                      int *set_sizes,
                      struct split_search_node_list **to_clear_head,
                      struct split_search_node_list **to_clear_tail,
                      struct split_search_node_list **leaf_head,
                      struct split_search_node_list **leaf_tail,
                      int num_sets)
{
    struct split_search_node *root_node = 
        (struct split_search_node *) malloc (
        sizeof(struct split_search_node));

    //add_to_clear_list(to_clear_head, to_clear_tail, root_node);

    //root_node->parent = NULL;
    root_node->S = S;
    root_node->S_dim.start = 0;
    root_node->S_dim.end = num_sets - 1;
    root_node->s_dim = (struct pair *) 
        malloc (num_sets * sizeof(struct pair));

    int i;
    for (i = 0; i < num_sets; ++i) {
        root_node->s_dim[i].start = 0;
        root_node->s_dim[i].end = set_sizes[i] - 1;
    }

    root_node->has_empty = 0;
    root_node->next = NULL;

    struct split_search_node *head, *tail;
    head = root_node;
    tail = root_node;
    
    *leaf_head = NULL;

    struct split_search_node *curr = head;
    while (curr != NULL) {
        struct split_search_node *left = 
                (struct split_search_node *) malloc (
                sizeof(struct split_search_node));
        left->next = NULL;
        left->has_empty = 0;

        struct split_search_node *center =
                (struct split_search_node *) malloc (
                sizeof(struct split_search_node));
        center->next = NULL;
        center->has_empty = 0;

        struct split_search_node *right =
                (struct split_search_node *) malloc (
                sizeof(struct split_search_node));
        right->next = NULL;
        right->has_empty = 0;

        split_search_o(curr, left, center, right);

        /* 
         * For any of the splits that have all non-zero subs sets,
         * add the split onto the queue to be re-split
         */
        if (left->has_empty == 0) {
            tail->next = left;
            tail = left;
            //add_to_clear_list(to_clear_head, to_clear_tail, left);
        } else {
            free_split_search_node(left);
        }

        if (right->has_empty == 0) {
            tail->next = right;
            tail = right;
            //add_to_clear_list(to_clear_head, to_clear_tail, right);
        } else {
            free_split_search_node(right);
        }

        if ( center->has_empty == 0) {
            /*
            if (center->S_dim.start < center->S_dim.end) {
                tail->next = center;
                tail = center;
            } else {
            */
            struct split_search_node_list *next_leaf;
            next_leaf = (struct split_search_node_list *)
                    malloc (sizeof(struct split_search_node_list));
            next_leaf->node = center;
            next_leaf->next = NULL;

            if ((*leaf_head) == NULL) {
                (*leaf_head) = next_leaf;
                (*leaf_tail) = *leaf_head;
            } else {
                (*leaf_tail)->next = next_leaf;
                (*leaf_tail) = next_leaf;
            }
            //}
        } else {
            free_split_search_node(center);
        }

        struct split_search_node *next = curr->next;
        free_split_search_node(curr);
        curr = next;
    }
}
//}}}

//{{{void psplit_sets_o (struct interval **S,
void psplit_sets_o (struct interval **S,
                    int *set_sizes,
                    struct split_search_node_list **leaf_head,
                    struct split_search_node_list **leaf_tail,
                    int num_sets,
                    int num_threads)
{
    struct split_search_node *root_node = 
        (struct split_search_node *) malloc (
        sizeof(struct split_search_node));

    root_node->S = S;
    root_node->S_dim.start = 0;
    root_node->S_dim.end = num_sets - 1;
    root_node->s_dim = (struct pair *) 
        malloc (num_sets * sizeof(struct pair));

    int i;
    for (i = 0; i < num_sets; ++i) {
        root_node->s_dim[i].start = 0;
        root_node->s_dim[i].end = set_sizes[i] - 1;
    }

    root_node->has_empty = 0;

    // set up a caboose at the end of the list so that we don't move to a null
    // node 
    struct split_search_node *caboose = 
        (struct split_search_node *) malloc (
        sizeof(struct split_search_node));
    caboose->S = NULL;

    root_node->next = caboose;

    struct split_search_node *head, *tail;
    head = root_node;
    tail = root_node;
    
    *leaf_head = NULL;

    pthread_t threads[num_threads];
    struct pone_split_o_args thread_args[num_threads];
    static pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t cond_mutex = PTHREAD_COND_INITIALIZER;
    int rc;

    int waiting = 0, work = 1;
    for (i = 0; i < num_threads; ++i) {
        thread_args[i].curr = &head;
        thread_args[i].tail = &tail;
        thread_args[i].waiting = &waiting;
        thread_args[i].work = &work;
        thread_args[i].num_threads = num_threads;
        thread_args[i].work_mutex = &work_mutex;
        thread_args[i].cond_mutex = &cond_mutex;

        rc = pthread_create(&threads[i],
                            NULL,
                            pone_split_o,
                            (void *) &thread_args[i]);

        assert(0 == rc);
    }

    for (i = 0; i < num_threads; ++i) {
        rc = pthread_join(threads[i],NULL);
        assert(0 == rc);
    }
}
//}}}

//{{{ void *pone_split_o(void *ptr)
void *pone_split_o(void *ptr)
{
    struct pone_split_o_args *args = (struct pone_split_o_args *)ptr;

    args->leaf_head = NULL;
    args->leaf_tail = NULL;
        
    while(1) {

        //{{{ Get work or wait for work
        // set the lock to get the next piece of work
        pthread_mutex_lock(args->work_mutex);

        /*
         * if there is no work, then the thread will
         * - up the number of waiting threads
         * - check to see how many other threads are waiting
         *   - if eveyone else is waiting, then no work is left to be done
         *     and that thread starts the shutdown process
         */
        while (*(args->work) == 0) {
            // up the number waiting
            *(args->waiting) = *(args->waiting) + 1;

            // if everyone is waiting then get out
            if (*(args->waiting) == args->num_threads) {
                *(args->work) = -1;
                pthread_mutex_unlock(args->work_mutex);
                pthread_cond_signal(args->cond_mutex);
                return;
            }

            pthread_cond_wait(args->cond_mutex, args->work_mutex);
            *(args->waiting) = *(args->waiting) - 1;
        }

        // die if all the work is done
        if (*(args->work) < 0) {
            pthread_mutex_unlock(args->work_mutex);
            pthread_cond_signal(args->cond_mutex);
            return;
        }

        // Grab the work and move the counter of works left down by one
        struct split_search_node *curr = *(args->curr);

        *(args->curr) = (*(args->curr))->next;
        assert (*(args->curr) != NULL);
        *(args->work) = *(args->work) - 1;

        // Release lock
        pthread_mutex_unlock(args->work_mutex);
        //}}}

        //{{{ do the work
        
        struct split_search_node *left = NULL, 
                                 *right = NULL, 
                                 *center = NULL;

        struct split_search_node_list *leaf = NULL;

        one_split_o(curr, &left, &right, &center, &leaf);

        struct split_search_node *new_work_head = NULL, 
                                 *new_work_tail = NULL; 

        int work_added = 0;

        if (left != NULL) {
            if (new_work_head == NULL)
                new_work_head = left;
            else
                new_work_tail->next = left;

            new_work_tail = left;
            ++work_added;
        }

        if (right != NULL) {
            if (new_work_head == NULL)
                new_work_head = right;
            else
                new_work_tail->next = right;

            new_work_tail = right;
            ++work_added;
        }

        if (leaf != NULL) {

            if (args->leaf_head == NULL)
                args->leaf_head = leaf;
            else
                args->leaf_tail->next = leaf;

            args->leaf_tail = leaf;

        } else if (center != NULL) {
            if (new_work_head == NULL)
                new_work_head = center;
            else
                new_work_tail->next = center;

            new_work_tail = center;
            ++work_added;
        }
        //}}}

        //{{{ add more work to the queue
        pthread_mutex_lock(args->work_mutex);

        // see if new work needs to be added
        if (work_added > 0) {
            // see if we have hit the end of the list
            if ( (*(args->curr))->S == NULL ) {
                new_work_tail->next = *(args->curr);
                *(args->curr) = new_work_head;
                *(args->tail) = new_work_tail;
            } else {
                new_work_tail->next = (*(args->tail))->next; 

                (*(args->tail))->next = new_work_head;
                *(args->tail) = new_work_tail;
            }

            *(args->work) = *(args->work) + work_added;
        }

        pthread_mutex_unlock(args->work_mutex);
        pthread_cond_signal(args->cond_mutex);

        free_split_search_node(curr);
        //}}}
    
    }
}
//}}}

//{{{ void one_split_o(split_search_node *curr = head;
void one_split_o(struct split_search_node *curr,
                 struct split_search_node **left,
                 struct split_search_node **right,
                 struct split_search_node **center,
                 struct split_search_node_list **leaf)
{
    *left = (struct split_search_node *) malloc (
            sizeof(struct split_search_node));
    (*left)->next = NULL;
    (*left)->has_empty = 0;

    *center = (struct split_search_node *) malloc (
            sizeof(struct split_search_node));
    (*center)->next = NULL;
    (*center)->has_empty = 0;

    *right = (struct split_search_node *) malloc (
            sizeof(struct split_search_node));
    (*right)->next = NULL;
    (*right)->has_empty = 0;

    split_search_o(curr, *left, *center, *right);

    /* 
     * For any of the splits that have all non-zero subs sets,
     * add the split onto the queue to be re-split
     */
    if ((*left)->has_empty != 0) {
        free_split_search_node(*left);
        *left = NULL;
    }

    if ((*right)->has_empty != 0) {
        free_split_search_node(*right);
        *right = NULL;
    }

    if ( (*center)->has_empty == 0) {
        /* 
         * Do not re-split the center if the next level is the last level
         * If the next level is the last level, add center onto the list of
         * leaf nodes
         */
        if ((*center)->S_dim.start >= (*center)->S_dim.end) {
            //struct split_search_node_list *next_leaf;
            *leaf = (struct split_search_node_list *)
                    malloc (sizeof(struct split_search_node_list));
            (*leaf)->node = *center;
            (*leaf)->next = NULL;
        }
    } else {
        free_split_search_node(*center);
        *center = NULL;
    }
}
//}}}

//{{{void split_sets_o (struct interval **S,
void split_sets_o(struct interval **S,
                 int *set_sizes,
                 struct split_search_node_list **to_clear_head,
                 struct split_search_node_list **to_clear_tail,
                 struct split_search_node_list **leaf_head,
                 struct split_search_node_list **leaf_tail,
                 int num_sets)
{
    struct split_search_node *root_node = 
        (struct split_search_node *) malloc (
        sizeof(struct split_search_node));

    root_node->S = S;
    root_node->S_dim.start = 0;
    root_node->S_dim.end = num_sets - 1;
    root_node->s_dim = (struct pair *) 
        malloc (num_sets * sizeof(struct pair));

    int i;
    for (i = 0; i < num_sets; ++i) {
        root_node->s_dim[i].start = 0;
        root_node->s_dim[i].end = set_sizes[i] - 1;

#ifdef SPLIT_DEBUG
        fprintf(stderr, "set sizes:%d\t%d\n", 0, set_sizes[i] - 1);
#endif
    }

    root_node->has_empty = 0;
    root_node->next = NULL;

    struct split_search_node *head, *tail;
    head = root_node;
    tail = root_node;
    
    *leaf_head = NULL;

    struct split_search_node *curr = head;
    while (curr != NULL) {

        struct split_search_node *left = NULL, 
                                 *right = NULL, 
                                 *center = NULL;
        struct split_search_node_list *leaf = NULL;

        one_split_o(curr, &left, &right, &center, &leaf);

        if (left != NULL) {
            tail->next = left;
            tail = left;
        }

        if (right != NULL) {
            tail->next = right;
            tail = right;
        }

        if (leaf != NULL) {

            if ((*leaf_head) == NULL) {
                (*leaf_head) = leaf;
                (*leaf_tail) = *leaf_head;
            } else {
                (*leaf_tail)->next = leaf;
                (*leaf_tail) = leaf;
            }

        } else if (center != NULL) {
            tail->next = center;
            tail = center;
        }

        struct split_search_node *next = curr->next;
        free_split_search_node(curr);
        curr = next;
    }
}
//}}}

//{{{void split_sets (struct interval **S,
void split_sets (struct interval **S,
                 int *set_sizes,
                 struct split_search_node_list **to_clear_head,
                 struct split_search_node_list **to_clear_tail,
                 struct split_search_node_list **leaf_head,
                 struct split_search_node_list **leaf_tail,
                 int num_sets)
{
    *to_clear_head = NULL;
    *to_clear_tail = NULL;

    struct split_search_node *root_node = 
        (struct split_search_node *) malloc (
        sizeof(struct split_search_node));

    add_to_clear_list(to_clear_head, to_clear_tail, root_node);

    root_node->parent = NULL;
    root_node->S = S;
    root_node->S_dim.start = 0;
    root_node->S_dim.end = num_sets - 1;
    root_node->s_dim = (struct pair *) 
        malloc (num_sets * sizeof(struct pair));

    int i;
    for (i = 0; i < num_sets; ++i) {
        root_node->s_dim[i].start = 0;
        root_node->s_dim[i].end = set_sizes[i] - 1;
    }

    root_node->has_empty = 0;
    root_node->next = NULL;
    struct split_search_node *head, *tail;
    head = root_node;
    tail = root_node;
    
    *leaf_head = NULL;

    struct split_search_node *curr = head;
    while (curr != NULL) {
        struct split_search_node *left = 
                (struct split_search_node *) malloc (
                sizeof(struct split_search_node));
        left->next = NULL;
        left->has_empty = 0;

        struct split_search_node *center =
                (struct split_search_node *) malloc (
                sizeof(struct split_search_node));
        center->next = NULL;
        center->has_empty = 0;

        struct split_search_node *right =
                (struct split_search_node *) malloc (
                sizeof(struct split_search_node));
        right->next = NULL;
        right->has_empty = 0;

        split_search(curr, left, center, right);
        //printf("left\n");
        //print_slice(0, left);
        //printf("center\n");
        //print_slice(0, center);
        //printf("right\n");
        //print_slice(0, right);

        /* 
         * For any of the splits that have all non-zero subs sets,
         * add the split onto the queue to be re-split
         */
        if (left->has_empty == 0) {
            tail->next = left;
            tail = left;
            add_to_clear_list(to_clear_head, to_clear_tail, left);
        } else {
            free_split_search_node(left);
        }

        if (right->has_empty == 0) {
            tail->next = right;
            tail = right;
            add_to_clear_list(to_clear_head, to_clear_tail, right);
        } else {
            free_split_search_node(right);
        }

        if ( center->has_empty == 0) {
            /* 
             * Do not re-split the center if the next level is the last level
             * If the next level is the last level, add center onto the list of
             * leaf nodes
             */
            add_to_clear_list(to_clear_head, to_clear_tail, center);
            if (center->S_dim.start < center->S_dim.end) {
                tail->next = center;
                tail = center;
            } else {
                struct split_search_node_list *next_leaf;
                next_leaf = (struct split_search_node_list *)
                        malloc (sizeof(struct split_search_node_list));
                next_leaf->node = center;
                next_leaf->next = NULL;

                if ((*leaf_head) == NULL) {
                    (*leaf_head) = next_leaf;
                    (*leaf_tail) = *leaf_head;
                } else {
                    (*leaf_tail)->next = next_leaf;
                    (*leaf_tail) = next_leaf;
                }
            }
        } else {
            free_split_search_node(center);
        }

        curr = curr->next;
    }
 
}
//}}}

//{{{int build_split_nway_o(struct split_search_node_list *leaf_head,
int build_split_nway_o(struct split_search_node_list *leaf_head,
                     struct int_list_list **R_head,
                     int num_sets)
{
    struct split_search_node_list *curr_leaf = leaf_head;

    struct int_list_list *R_tail;
    *R_head = NULL;
    int count = 0;
    while (curr_leaf != NULL) {
        struct int_list_list *new_nway = (struct int_list_list *)
            malloc (sizeof (struct int_list_list));
        new_nway->size = num_sets;
        new_nway->next = NULL;
        new_nway->list = (int64_t *)malloc(num_sets * sizeof(int64_t));

        int i;
        for (i = 0; i < num_sets; ++i) {
            new_nway->list[i] = curr_leaf->node->s_dim[i].start; 
        }

        if (*R_head == NULL) {
            (*R_head) = new_nway;
            R_tail = new_nway;
        } else {
            R_tail->next = new_nway;
            R_tail = new_nway;
        }

        // it is possible that the last set has more than one item in it
        // if so make a copy up new_nway and add it to R_tail
        for (i = curr_leaf->node->s_dim[num_sets - 1].start + 1; 
             i <= curr_leaf->node->s_dim[num_sets - 1].end; 
             ++i) {

            struct int_list_list *nway_cpy = (struct int_list_list *)
                malloc (sizeof (struct int_list_list));
            nway_cpy->size = num_sets;
            nway_cpy->next = NULL;
            nway_cpy->list = (int64_t *)malloc(num_sets * sizeof(int64_t));
            memcpy(nway_cpy->list, new_nway->list, num_sets * sizeof(int64_t));
            nway_cpy->list[num_sets - 1] = i;

            if (*R_head == NULL) {
                (*R_head) = nway_cpy;
                R_tail = nway_cpy;
            } else {
                R_tail->next = nway_cpy;
                R_tail = nway_cpy;
            }
        }
        
        struct split_search_node_list *next = curr_leaf->next;
        free(curr_leaf);
        curr_leaf = next;
        count += 1;
    }

    return count;
}
//}}}

//{{{int build_split_nway(struct split_search_node_list *leaf_head,
int build_split_nway(struct split_search_node_list *leaf_head,
                     struct int_list_list **R_head,
                     int num_sets)
{
    struct split_search_node_list *curr_leaf = leaf_head;

    struct int_list_list *R_tail;
    *R_head = NULL;
    int count = 0;
    while (curr_leaf != NULL) {
        // build path to leaf
        // add in reverse order, top element is first
        struct split_search_node *curr_parent = curr_leaf->node->parent;
        int path[num_sets];
        int i = 0;
        while (curr_parent != NULL) {
            struct pair a_dim;
            a_dim = curr_parent->s_dim[0];
            int a_mid = (a_dim.end+1 + a_dim.start-1)/2;
            path[num_sets - 1 - i - 1] = a_mid;
            curr_parent = curr_parent->parent;
            ++i;
        }

        for (i = curr_leaf->node->s_dim[0].start;
             i <= curr_leaf->node->s_dim[0].end;
             ++i) {
            struct int_list_list *new_nway = 
                (struct int_list_list *)
                malloc (sizeof (struct int_list_list));
            new_nway->size = num_sets;
            new_nway->next = NULL;
            path[num_sets - 1] = i;

            new_nway->list = (int64_t *)malloc(num_sets * sizeof(int64_t));
            memcpy(new_nway->list, path ,num_sets * sizeof(int64_t));

            if (*R_head == NULL) {
                (*R_head) = new_nway;
                R_tail = new_nway;
            } else {
                R_tail->next = new_nway;
                R_tail = new_nway;
            }
        }
        struct split_search_node_list *next = curr_leaf->next;
        free(curr_leaf);
        curr_leaf = next;
        count += 1;
    }

    return count;
}
//}}}

//{{{void free_split_search_node (struct split_search_node *n)
void free_split_search_node (struct split_search_node *n)
{
    if (n->s_dim != NULL) {
        free(n->s_dim);
        n->s_dim = NULL;
    }
    free(n);
}
//}}}

//{{{void free_int_list_list(struct int_list_list *l)
void free_int_list_list(struct int_list_list *l)
{
    struct int_list_list *curr = l;
    while (curr != NULL) {
        struct int_list_list *next = curr->next;
        free(curr->list);
        free(curr);
        curr = next;

    }
}
//}}}

//{{{ void add_to_clear_list(struct split_search_node_list **to_clear_head,
void add_to_clear_list(struct split_search_node_list **to_clear_head,
                       struct split_search_node_list **to_clear_tail,
                       struct split_search_node *node)
{
    struct split_search_node_list *next_clear =
        (struct split_search_node_list *)
        malloc (sizeof(struct split_search_node_list));
    next_clear->node = node;
    next_clear->next = NULL;
    if (*to_clear_head == NULL) {
        *to_clear_head = next_clear;
        *to_clear_tail = next_clear;
    } else {
        (*to_clear_tail)->next = next_clear;
        *to_clear_tail = next_clear;
    }
}
//}}}

#if 0
//{{{ void psplit(struct interval **S,
void psplit(struct interval **S,
           int *set_sizes,
           int num_sets,
           struct int_list_list **R,
           int num_threads)
{
    struct split_search_node_list *leaf_head, *leaf_tail;
    struct split_search_node_list *to_clear_head, *to_clear_tail;

    start();
    psplit_sets(S,
               set_sizes,
               &to_clear_head,
               &to_clear_tail,
               &leaf_head,
               &leaf_tail,
               num_sets,
               num_threads);
    stop();
    printf("psplit_sets\t%lu\n", report());

    struct int_list_list *R_head;

    start();
    int count = build_split_nway(leaf_head, &R_head, num_sets);
    stop();
    printf("build_split_nway\t%lu\n", report());

    // free up space
    start();
    struct split_search_node_list *curr_clear = to_clear_head;
    while (curr_clear != NULL) {
        struct split_search_node_list *next_clear = curr_clear->next;
        free_split_search_node(curr_clear->node);
        free(curr_clear);
        curr_clear = next_clear;
    }
    stop();
    printf("free\t%lu\n", report());

    *R = R_head;
}
//}}}
//{{{void psplit_sets (struct interval **S,
void psplit_sets (struct interval **S,
                 int *set_sizes,
                 struct split_search_node_list **to_clear_head,
                 struct split_search_node_list **to_clear_tail,
                 struct split_search_node_list **leaf_head,
                 struct split_search_node_list **leaf_tail,
                 int num_sets,
                 int num_threads)
{
    *to_clear_head = NULL;
    *to_clear_tail = NULL;

    struct split_search_node *root_node = 
        (struct split_search_node *) malloc (
        sizeof(struct split_search_node));

    pthread_mutex_t clear_mutex, split_mutex, leaf_mutex;

    if (pthread_mutex_init(&clear_mutex,NULL)) {
        perror("pthread_mutex_init: clear_mutex");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&split_mutex,NULL)) {
        perror("pthread_mutex_init: split_mutex");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&leaf_mutex,NULL)) {
        perror("pthread_mutex_init: leaf_mutes");
        exit(EXIT_FAILURE);
    }


    TS_add_to_split_search_node_list(to_clear_head,
                                     to_clear_tail,
                                     root_node,
                                     &clear_mutex);

    root_node->parent = NULL;
    root_node->S = S;
    root_node->S_dim.start = 0;
    root_node->S_dim.end = num_sets - 1;
    root_node->s_dim = (struct pair *) 
        malloc (num_sets * sizeof(struct pair));

    int i;
    for (i = 0; i < num_sets; ++i) {
        root_node->s_dim[i].start = 0;
        root_node->s_dim[i].end = set_sizes[i] - 1;
    }

    root_node->has_empty = 0;
    root_node->next = NULL;
    struct split_search_node *head, *tail;
    head = root_node;
    tail = root_node;
    
    *leaf_head = NULL;

    /*
    start();
    struct split_search_node *curr = head;
    while (curr != NULL) {
        struct one_split_args args;
        args.curr = &curr;
        args.tail = &tail;
        args.to_clear_head = to_clear_head;
        args.to_clear_tail = to_clear_tail;
        args.leaf_head = leaf_head;
        args.leaf_tail = leaf_tail;
        args.clear_mutex = &clear_mutex;
        args.split_mutex = &split_mutex;
        args.leaf_mutex = &leaf_mutex;
        one_split(args);

        curr = curr->next;
    }
    stop();
    printf("split:%lu\n", report());
    */

    struct threadpool *pool;
    if ((pool = threadpool_init(num_threads)) == NULL) {
        printf("Error! Failed to create a thread pool struct.\n");
        exit(EXIT_FAILURE);
    }

    int ret, failed_count;

    struct split_search_node *curr = head, *last = head;

    int to_stop = 0;

    while (to_stop == 0 ) {

        int num_added = 0;
        while (curr != NULL) {
            fprintf(stderr, "curr:%p\n", curr);
            struct one_split_args *args = (struct one_split_args *)
                    malloc(sizeof(struct one_split_args));
            args->curr = curr;
            args->tail = &tail;
            args->to_clear_head = to_clear_head;
            args->to_clear_tail = to_clear_tail;
            args->leaf_head = leaf_head;
            args->leaf_tail = leaf_tail;
            args->clear_mutex = &clear_mutex;
            args->split_mutex = &split_mutex;
            args->leaf_mutex = &leaf_mutex;

            ret = threadpool_add_task(pool,one_split,args,1);

            if (ret == -1) {
                printf("An error had occurred while adding a task.");
                exit(EXIT_FAILURE);
            }

            if (ret == -2) {
                failed_count++;
            }

            last = curr;
            curr = curr->next;
            ++num_added;
        }

        /*
        fprintf(stderr, "num_added:%d curr:%p last:%p\n",
                num_added,
                curr,
                last);
        */

        /*
        while ((threadpool_size(pool) > 0) || 
               (threadpool_running(pool) > 0) ) {
            usleep(1);
        }
        */
        
        curr = last->next;

        if ( (threadpool_size(pool) == 0) &&
             (threadpool_running(pool) > 0) &&
             (num_added == 0) )
            to_stop = 1;
    }
}
//}}}
//{{{ void one_split(void *ptr)
void one_split(void *ptr)
{
    struct one_split_args *args = (struct one_split_args *)ptr;
        
    struct split_search_node *curr = args->curr;
    struct split_search_node **tail = args->tail;
    struct split_search_node_list **to_clear_head = args->to_clear_head;
    struct split_search_node_list **to_clear_tail = args->to_clear_tail;
    struct split_search_node_list **leaf_head = args->leaf_head;
    struct split_search_node_list **leaf_tail = args->leaf_tail;
    pthread_mutex_t *clear_mutex = args->clear_mutex;
    pthread_mutex_t *split_mutex = args->split_mutex;
    pthread_mutex_t *leaf_mutex = args->leaf_mutex;

    struct split_search_node *left = 
            (struct split_search_node *) malloc (
            sizeof(struct split_search_node));
    left->next = NULL;
    left->has_empty = 0;

    struct split_search_node *center =
            (struct split_search_node *) malloc (
            sizeof(struct split_search_node));
    center->next = NULL;
    center->has_empty = 0;

    struct split_search_node *right =
            (struct split_search_node *) malloc (
            sizeof(struct split_search_node));
    right->next = NULL;
    right->has_empty = 0;

    //fprintf(stderr, "\t%d\tcurr:%p\n", pthread_self(), curr);

    split_search(curr, left, center, right);

    /* 
     * For any of the splits that have all non-zero subs sets,
     * add the split onto the queue to be re-split
     */
    if (left->has_empty == 0) {
        TS_add_split_search_node(tail, left, split_mutex);
        //(*tail)->next = left;
        //(*tail) = left;
        TS_add_to_split_search_node_list(to_clear_head,
                                         to_clear_tail,
                                         left,
                                         clear_mutex);
    } else {
        free_split_search_node(left);
    }

    if (right->has_empty == 0) {
        TS_add_split_search_node(tail, right, split_mutex);
        //(*tail)->next = right;
        //(*tail) = right;
        TS_add_to_split_search_node_list(to_clear_head,
                                         to_clear_tail,
                                         right,
                                         clear_mutex);
    } else {
        free_split_search_node(right);
    }

    if ( center->has_empty == 0) {
        /* 
         * Do not re-split the center if the next level is the last level
         * If the next level is the last level, add center onto the list of
         * leaf nodes
         */
        TS_add_to_split_search_node_list(to_clear_head,
                                         to_clear_tail,
                                         center,
                                         clear_mutex);
        if (center->S_dim.start < center->S_dim.end) {
            TS_add_split_search_node(tail, center, split_mutex);
            //(*tail)->next = center;
            //(*tail) = center;
        } else {
            TS_add_to_split_search_node_list(leaf_head,
                                             leaf_tail,
                                             center,
                                             leaf_mutex);
        }
    } else {
        free_split_search_node(center);
    }
}
//}}}
//{{{ void TS_add_split_search_node(struct split_search_node **tail,
void TS_add_split_search_node(struct split_search_node **tail,
                              struct split_search_node *new_node,
                              pthread_mutex_t *mutex)
{
    pthread_mutex_lock(mutex);

    (*tail)->next = new_node;
    (*tail) = new_node;

    pthread_mutex_unlock(mutex);
}
//}}}
//{{{ void TS_add_to_clear_list(struct split_search_node_list **to_clear_head,
void TS_add_to_split_search_node_list(
                       struct split_search_node_list **head,
                       struct split_search_node_list **tail,
                       struct split_search_node *node,
                       pthread_mutex_t *mutex)
{
    struct split_search_node_list *new_node =
        (struct split_search_node_list *)
        malloc (sizeof(struct split_search_node_list));
    new_node->node = node;
    new_node->next = NULL;

    pthread_mutex_lock(mutex);

    if (*head == NULL) {
        *head = new_node;
        *tail = new_node;
    } else {
        (*tail)->next = new_node;
        *tail = new_node;
    }

    pthread_mutex_unlock(mutex);
}
//}}}
#endif

#if 0
//{{{ void get_common_set(struct interval **S,
void get_common_set(struct interval **S,
                    int *set_sizes,
                    struct int_list_list *R,
                    int num_R,
                    struct tag *T1,
                    struct tag *T2,
                    struct tag **newT,
                    struct interval **X)
{

#ifdef DEBUGITTR
    printf("=T1=>\n");
    print_tags(T1);
    printf("<=T1=\n");
    printf("=T2=>\n");
    print_tags(T2);
    printf("<=T2=\n");
#endif

    *X = (struct interval *) malloc(num_R * sizeof(struct interval));

    *newT = (struct tag*) malloc(sizeof(struct tag));
    (*newT)->num_sets = (T1->num_sets) + (T2->num_sets);
    (*newT)->num_intervals = num_R;

    // It is important that T1 and T2 are in the same order that intervals ids
    // are given in R
    (*newT)->set_ids = (int *) malloc((*newT)->num_sets * sizeof(int));
    int i,j;

    i = 0;
    for (j = 0; j < T1->num_sets; ++j) {
        (*newT)->set_ids[i] = T1->set_ids[j];
        ++i;
    }

    for (j = 0; j < T2->num_sets; ++j) {
        (*newT)->set_ids[i] = T2->set_ids[j];
        ++i;
    }

    (*newT)->interval_ids = (int *)
            malloc((*newT)->num_sets * (*newT)->num_intervals * sizeof(int));

    struct int_list_list *curr = R;
    i = 0;
    while (curr != NULL) {
        // curr->list is an arry of interval ids that are in an intersection
        // here we are only looking at pairs of interseitons so 
        // curr->list[0] is the index in the first set and 
        // curr->list[1] is the index in the second set

        // Create a new interval that is the region common to the two
        // intersecting intervals
        (*X)[i].start= max(S[0][curr->list[0]].start,
                        S[1][curr->list[1]].start);
        (*X)[i].end= min(S[0][curr->list[0]].end,
                        S[1][curr->list[1]].end);

        // Concat the two tags from these intervals (curr->list[0] in T1 and
        // curr->list[1] in T2) into the tags for the new interval set

        // T1->interval_ids[curr->list[0]] is the index of the first label for
        // the interval that is part of this intersection, it will extend for
        // (T1->num_sets - 1) more bases
        int offset = i * (*newT)->num_sets; 
        int o = 0;

        for (j = 0; j < T1->num_sets; ++j) {
            // j should point to the set id currently being considered
            // curr->list[0] is the index of the interval in the current set
            // that is part of the intersection
#ifdef DEBUGITTER
            printf("curr set:%d\tcurr intr:%d\tcurr tag:%d\n",
                    T1->set_ids[j],
                    curr->list[0],
                    T1->interval_ids[ (curr->list[0]*T1->num_sets) + j ]);
#endif
            (*newT)->interval_ids[offset + o] =
                    T1->interval_ids[ (curr->list[0]*T1->num_sets) + j ];
            ++o;
        }
        for (j = 0; j < T2->num_sets; ++j) {
#ifdef DEBUGITTER
            printf("curr set:%d\tcurr intr:%d\tcurr tag:%d\n",
                    T2->set_ids[j],
                    curr->list[1],
                    T2->interval_ids[ (curr->list[1]*T2->num_sets) + j ]);
#endif
            (*newT)->interval_ids[offset + o] =
                    T2->interval_ids[ (curr->list[1]*T2->num_sets) + j ];
            ++o;
        }
        ++i;
        curr = curr->next;
    }

#ifdef DEBUGITTER
    printf("=newT=>\n");
    print_tags(*newT);
    printf("<=newT=\n");
#endif
}
//}}}
//{{{void read_interval_sets(char *file_name,
void read_interval_sets(char *file_name,
                        struct interval ***S,
                        int **set_sizes,
                        int *num_sets) 
{
    int MAX_LINE_SIZE=8388608;
    char *line = (char *) malloc(MAX_LINE_SIZE * sizeof(char));
    FILE *fp;
    char *l_p;
    char *save_set_ptr;

    *num_sets = 0;

    fp = fopen(file_name, "r");

    if (fp == NULL)
        perror("Error opening file.");

    struct interval_list_list *il_head = NULL, *il_tail = NULL;
    struct int_list *list_size_head = NULL, *list_size_tail = NULL;

    while( fgets(line, MAX_LINE_SIZE, fp) != NULL ) {
        *num_sets = *num_sets + 1;
        l_p = strtok_r(line, "\t", &save_set_ptr);

        struct interval_list *head = NULL, *tail = NULL;
        int set_size = 0; 

        while (l_p != NULL) {
            struct interval_list *i = (struct interval_list *)
                    malloc(sizeof(struct interval_list));
            i->curr = (struct interval *) malloc(sizeof(struct interval));

            char *save_interval_ptr;
            /*
            i->curr->start = atoi(strtok_r(l_p, " ", &save_interval_ptr));
            i->curr->end = atoi(strtok_r(NULL, " ", &save_interval_ptr));
            */
            i->curr->start = strtoul(strtok_r(l_p, " ", &save_interval_ptr),
                                     NULL,0);
            i->curr->end = strtoul(strtok_r(NULL, " ", &save_interval_ptr),
                                   NULL,0);
            i->next = NULL;

            if (head == NULL)
                head = i;
            else
                tail->next = i;
            
            tail = i;

            set_size += 1;
            l_p = strtok_r(NULL, "\t", &save_set_ptr);
        }

        // take the set_size and add it to the link list of set sizes
        struct int_list *curr_list_size = (struct int_list *)
                malloc(sizeof(struct int_list));
        curr_list_size->value = set_size;
        curr_list_size->next = NULL;
        if (list_size_head == NULL)
            list_size_head = curr_list_size;
        else
            list_size_tail->next = curr_list_size;
        list_size_tail = curr_list_size;


        // take the linked list of intervals and add it to the linked 
        // list of interval lists
        struct interval_list_list *n = (struct interval_list_list *)
                malloc(sizeof(struct interval_list_list));
        n->curr = head;
        n->next = NULL;
        
        if (il_head ==NULL)
            il_head = n;
        else
            il_tail->next = n;
        
        il_tail = n;
    }

    // copy the set size linked list to array of set sizes
    *set_sizes = (int *) malloc(*num_sets * sizeof(int));
    int j = 0;
    struct int_list *size_curr = list_size_head;
    while ( size_curr != NULL ){
        (*set_sizes)[j] = size_curr->value;
        size_curr = size_curr->next;
        ++j;
    }

    // copy the linked lists of intervals to array of intervals
    *S = (struct interval **) malloc(*num_sets * sizeof(struct interval *));
    struct interval_list_list *il_curr = il_head;
    j = 0;
    while ( (il_curr != NULL) ) {
        (*S)[j] = (struct interval *)
               malloc(sizeof(struct interval) * (*set_sizes)[j]);

        struct interval_list *i_curr = il_curr->curr;
        int k = 0;
        while (i_curr != NULL) {
            (*S)[j][k].start = i_curr->curr->start;
            (*S)[j][k].end = i_curr->curr->end;
            i_curr = i_curr->next;
            ++k;
        }
        
        il_curr = il_curr->next;
        ++j;
    }
}
//}}}
//{{{void gen_simple_sets(struct interval ***S,
void gen_simple_sets(struct interval ***S,
                     int **set_sizes,
                     int num_sets,
                     int num_elements,
                     int len,
                     int seed)
{
    *S = (struct interval **) malloc(num_sets * sizeof(struct interval *));
    *set_sizes = (int *) malloc(num_sets * sizeof(int));

    int i,j;
    for (i = 0; i < num_sets; i++)
        (*set_sizes)[i] = num_elements;

    srand(seed);
    for (i = 0; i < num_sets; i++) {
        (*S)[i] = (struct interval *)
               malloc(sizeof(struct interval) * (*set_sizes)[i]);
        int last_end = 0;
        for (j = 0; j < (*set_sizes)[i]; j++) {
            int space = rand() % 20;
            (*S)[i][j].start = last_end + space;
            last_end = last_end + space + len;
            (*S)[i][j].end = last_end + space + len;
        }
    }
}
//}}}
//{{{void gen_simple_sets_in_range(struct interval ***S,
void gen_simple_sets_in_range(struct interval ***S,
                     int **set_sizes,
                     int num_sets,
                     int num_elements,
                     int len,
                     int range,
                     int seed)
{
    *S = (struct interval **) malloc(num_sets * sizeof(struct interval *));
    *set_sizes = (int *) malloc(num_sets * sizeof(int));

    int i,j;
    for (i = 0; i < num_sets; i++)
        (*set_sizes)[i] = num_elements;

    srand(seed);
    for (i = 0; i < num_sets; i++) {
        (*S)[i] = (struct interval *)
               malloc(sizeof(struct interval) * (*set_sizes)[i]);
        for (j = 0; j < (*set_sizes)[i]; j++) {
            (*S)[i][j].start = rand() % range;
            (*S)[i][j].end = (*S)[i][j].start + len;
            /*
            int space = rand() % 20;
            (*S)[i][j].start = last_end + space;
            last_end = last_end + space + len;
            (*S)[i][j].end = last_end + space + len;
            */
        }

    }

    for (i = 0; i < num_sets; ++i) {
        qsort((*S)[i],
              (*set_sizes)[i],
              sizeof(struct interval),
              compare_interval_by_start);
    }
}
//}}}
//{{{ int point_overlap_test(int start,
int point_overlap_test(int start,
                       int end,
                       int *points,
                       int num_points,
                       int len)
{
    int i, num_overlaps = 0;

    for (i = 0; i < num_points; ++i) {
        if ( (start <= points[i] + len) &&
             (end >= points[i] - len) )
            ++num_overlaps;
    }

    return num_overlaps;
}
//}}}
//{{{void gen_simple_sets_in_range(struct interval ***S,
void gen_sets_in_range_with_num_inter(struct interval ***S,
                                      int **set_sizes,
                                      int num_sets,
                                      int num_elements,
                                      int len,
                                      int range,
                                      int seed,
                                      int num_i)
{
    *S = (struct interval **) malloc(num_sets * sizeof(struct interval *));
    *set_sizes = (int *) malloc(num_sets * sizeof(int));


    int i,j;
    for (i = 0; i < num_sets; i++)
        (*set_sizes)[i] = num_elements;

    srand(seed);

    int i_points[num_i];
    for (i = 0; i < num_i; i++)
        i_points[i] = rand() % range;

    qsort(i_points, num_i, sizeof(int), compare_int);

    for (i = 0; i < num_sets; i++) {
        (*S)[i] = (struct interval *)
               malloc(sizeof(struct interval) * (*set_sizes)[i]);

        for (j = 0; j < num_i; j++) {
            (*S)[i][j].start = i_points[j] - (rand() % len);
            (*S)[i][j].end = (*S)[i][j].start + len;
        }

        //for ( ; j < (*set_sizes)[i]; j++) {
        while ( j < (*set_sizes)[i] ) {
            (*S)[i][j].start = rand() % range;
            (*S)[i][j].end = (*S)[i][j].start + len;

            int num_overlaps = point_overlap_test((*S)[i][j].start,
                                                  (*S)[i][j].end,
                                                  i_points,
                                                  num_i,
                                                  len);
            if (num_overlaps == 0)
                ++j;
        }
    }

    for (i = 0; i < num_sets; ++i) {
        qsort((*S)[i],
              (*set_sizes)[i],
              sizeof(struct interval),
              compare_interval_by_start);
    }
}
//}}}
//{{{void parse_args()
int parse_args(int argc,
                char **argv,
                struct interval ***S,
                int **set_sizes,
                int *num_sets,
                int *to_print,
                int *num_threads)
{
    char c;

    char *file_name = NULL;
    *num_sets = 0;
    int num_elements = 0;
    int seed = 1;
    int len = 0;
    int range = 0;
    *to_print = 0;
    *num_threads = 1;
    int num_inters = 0;

    while ( (c = getopt(argc, argv, "f:n:i:s:l:p:r:t:I:") ) != -1) 
        switch(c) {
            case 'f':
                file_name = optarg;
                break;
            case 'I':
                num_inters = atoi(optarg);
                break;
            case 't':
                *num_threads = atoi(optarg);
                break;
            case 'n':
                *num_sets = atoi(optarg);
                break;
            case 'i':
                num_elements = atoi(optarg);
                break;
            case 's':
                seed = atoi(optarg);
                break;
            case 'r':
                range = atoi(optarg);
                break;
            case 'l':
                len = atoi(optarg);
                break;
            case 'p':
                *to_print = atoi(optarg);
                break;
            case 'h':
                usage(argv[0]);
                return(1);
            default:
                usage(argv[0]);
                return(1);
        }

    if (file_name != NULL)
        read_interval_sets(file_name, S, set_sizes, num_sets);
    else if ( (num_sets > 0) && (num_elements > 0) && (len > 0) ) {
        if (range == 0)
            gen_simple_sets(S,
                            set_sizes,
                            *num_sets,
                            num_elements,
                            len,
                            seed);
        else if (num_inters != 0)
            gen_sets_in_range_with_num_inter(S,
                                             set_sizes,
                                             *num_sets,
                                             num_elements,
                                             len,
                                             range,
                                             seed,
                                             num_inters);
        else
            gen_simple_sets_in_range(S,
                                     set_sizes,
                                     *num_sets,
                                     num_elements,
                                     len,
                                     range,
                                     seed);
    } else {
        if (*num_sets <= 0)
            fprintf(stderr, "\nnumber of sets not given\n");
        if (num_elements <= 0)
            fprintf(stderr, "\nnumber of intervals not given\n");
        if (len <= 0)
            fprintf(stderr, "\ninterval length not given\n");
        fprintf(stderr, "\n");

        usage(argv[0]);
        exit(1);
    }

    return(0);
}
//}}}
//{{{ void usage(char *prog)
void usage(char *prog)
{
    fprintf(stderr,"usage:%s [options]\n"
            "\t-f\tfile name\n"
            "\t-n\tnumber of sets\n"
            "\t-i\tnumber of intervals per set\n"
            "\t-I\tnumber of intersections\n"
            "\t-l\tinterval length\n"
            "\t-r\trange\n"
            "\t-p\tto print set\n"
            "\t-s\trandom seed\n", prog);
}
//}}}
//{{{void print_nway_common_interval(struct int_list_list *R)
void print_nway_common_interval(struct int_list_list *R,
                                struct interval **S)
{
    struct int_list_list *curr = R;
    while (curr != NULL) {
        int j;
        int64_t end = LONG_MAX, start =0;
        for (j = 0; j < curr->size; ++j) {
            end = MIN(end, S[j][curr->list[j]].end);
            start = MAX(start, S[j][curr->list[j]].start);
        }
        printf("%" PRId64 "\t%" PRId64 "\n", start, end);
        curr = curr->next;
    }
}
//}}}
//{{{void print_nway_intervals (struct int_list_list *R)
void print_nway_intervals (struct int_list_list *R,
                           struct interval **S)
{
    struct int_list_list *curr = R;
    while (curr != NULL) {
        int j;
        for (j = 0; j < curr->size; ++j) {
            if (j != 0)
                printf("\t");
            printf("%" PRId64 ",%" PRId64, S[j][curr->list[j]].start, 
                                           S[j][curr->list[j]].end);
        }
        printf("\n");
        curr = curr->next;
    }
}
//}}}
//{{{void print_nway_indicies(struct int_list_list *R)
void print_nway_indicies(struct int_list_list *R,
                         struct interval **S)
{
    struct int_list_list *curr = R;
    while (curr != NULL) {
        int j;
        for (j = 0; j < curr->size; ++j) {
            if (j != 0)
                printf("\t");
            printf("%" PRId64, curr->list[j]);
        }
        printf("\n");
        curr = curr->next;
    }
}
//}}}
//{{{ void nway_split(int num_sets,
void nway_split(int num_sets,
                int *set_sizes,
                struct interval **S,
                int *num_nways,
                int to_print)
{
    // Start with a root node in the intersect tree that has no value, and
    // contains the full set of sets
    struct intersect_tree *root = (struct intersect_tree *)
                                  malloc(sizeof(struct intersect_tree));
    root->parent = NULL;
    root->index = -1;
    // s_range is an inclusive range of set indicies, the length of the range
    // is end-start+1
    root->s_range.start = 0;
    root->s_range.end = num_sets - 1;
    root->q = (struct pair *)
              malloc((root->s_range.end - root->s_range.start + 1)*
                     sizeof(struct pair));

    // Initialize the q to contain the full range of all sets
    int i;
    for (i = 0; i < root->s_range.end - root->s_range.start + 1; ++i) {
        root->q[i].start = 0;
        root->q[i].end = set_sizes[i] - 1;
    }

    // This is a hierarchal style search, so we must maintain a queue of
    // to-be-searched paths,  root will always point to the intiatal search,
    // head will point to the next item to search and tail will point to the
    // last non-null element on the queue
    struct intersect_tree_node *search_root = (struct intersect_tree_node *)
            malloc(sizeof(struct intersect_tree_node));
    search_root->value = root;
    search_root->next = NULL;
    struct intersect_tree_node *search_head = search_root;
    struct intersect_tree_node *search_tail = search_root;

    // Keep a list of leave nodes
    struct intersect_tree_node *leaves_root = NULL;
    struct intersect_tree_node *leaves_tail = leaves_root;

    // Each itteration is over a set of subsets.  Each subset is a range within
    // the full set.  The search key is the mid point of the first interval in
    // the set.
    while (search_head != NULL) {

        // Each search will include a number of subsets.  The first set in that
        // subset will be the queries, and the subsequent sets will be the
        // databases.  The first subset is search in a divide and
        // conquer/binary search style.  The intial search is keys on the
        // middle element of the first subset, the next two searhes will be the
        // middle elemtns of the left and right portions of the first subset
        // and so on.  Each search will prouduce at most two more searches, and
        // each new search will be placed on a queue.  Q_head is the next
        // element to be search and Q_tail is last non-null element on the
        // queue.  New searches are added after Q_tail
        int num_subsets = search_head->value->s_range.end -
                          search_head->value->s_range.start + 1;

        struct search_q *Q_head = NULL;

        if (num_subsets > 0) {
            Q_head = (struct search_q *) malloc(sizeof(struct search_q));
            Q_head->q = search_head->value->q;
            Q_head->next = NULL;
        } else {
            // if we are at the last level of the hiearcy, then add the
            // currnent leaf to the list of leafs,  this will be used to print
            // the nway intersection in the end
            struct intersect_tree_node *next_leaf =
                (struct intersect_tree_node *)
                malloc(sizeof(struct intersect_tree_node));
            next_leaf->value = search_head->value;
            next_leaf->next = NULL;

            if (leaves_root == NULL) {
                leaves_root = next_leaf;
                leaves_tail = next_leaf;
            } else {
                leaves_tail->next = next_leaf;
                leaves_tail = next_leaf;
            }
        }

        struct search_q *Q_tail = Q_head;

        while ((num_subsets > 0) && (Q_head != NULL)) {
            // subset_dims contains the ranges of each subset that are to be
            // considered for the current search
            struct pair *subset_dims = Q_head->q;

            // DEBUG, print the ranges
            //for (i = 0; i < num_subsets; ++i) {
            //printf("%d,%d ", subset_dims[i].start, subset_dims[i].end);
            //}
            //printf("\n");

            // Since this is a hierarchal search, every round will include a
            // smaller number of subsets in S.  The first element in the subset
            // will be at index S[index_offset]
            int index_offset = search_head->value->s_range.start;

            struct interval *a = S[index_offset];
            //struct pair a_dim = subset_dims[index_offset];
            struct pair a_dim = subset_dims[0];
            int a_mid = (a_dim.end+1 + a_dim.start-1)/2;

            // The result of the current search is stored in the intersect tree.
            // curr_search->q contains the range of intervals in the subsets
            // S[search_head->value->s_range.start+1..
            //   search_head->value->s_range.end].  These subsets will be
            // searched in the next round of the hierarchal search process
            struct intersect_tree *curr_search = (struct intersect_tree *)
                                                 malloc(sizeof(struct intersect_tree));

            //add a node to the parent's list
            curr_search->parent = search_head->value;
            //struct intersect_tree_node *next_child =
            //(struct intersect_tree_node * )
            //malloc(sizeof(struct intersect_tree_node));
            //next_child->value = curr_search;
            //next_child->next = search_head->value->children;
            //search_head->value->children = next_child;

            curr_search->index = a_mid;
            curr_search->s_range.start = search_head->value->s_range.start + 1;
            curr_search->s_range.end = search_head->value->s_range.end;
            curr_search->q = (struct pair *)
                             malloc((num_subsets - 1)* sizeof(struct pair));

            // Each search will produce at most 2 sub-searches for the current
            // level in the hierarchy
            int left_is_empty = 0;
            struct search_q *Q_left = (struct search_q *)
                                      malloc(sizeof(struct search_q));
            Q_left->q = (struct pair *)
                        malloc(num_subsets * sizeof(struct pair));
            Q_left->next = NULL;

            Q_left->q[0].start = a_dim.start;
            Q_left->q[0].end = a_mid - 1;

            if (Q_left->q[0].start > Q_left->q[0].end)
                left_is_empty = 1;

            int center_is_empty = 0;

            int right_is_empty = 0;
            struct search_q *Q_right = (struct search_q *)
                                       malloc(sizeof(struct search_q));
            Q_right->q = (struct pair *)
                         malloc(num_subsets * sizeof(struct pair));
            Q_right->next = NULL;

            Q_right->q[0].start = a_mid + 1;
            Q_right->q[0].end = a_dim.end;

            if (Q_right->q[0].start > Q_right->q[0].end)
                right_is_empty = 1;

            // There are three different indicies at work here.
            // 1. S
            //   These sets contain the full sets and the range is 0..num_sets
            // 2. subset_dims Q_left->q and Q_right->q.
            //   These contain only the sets that are in the current level of
            //   the hierachy search and the range 0..num_subsets
            // 3. curr_search->q.
            //   This contains only the sets that are in the next level of the
            //   hierchy and the range is [0..num_subsets-1]
            // The for loop iterator i mateches #2.
            // To get the propper index of S:
            //   i + search_head->value->s_range.start.  For convience we
            //   have let index_offset = search_head->value->s_range.start
            // To get the propper index of curr_search->q:
            //   i - 1
            for (i = 1; i < num_subsets; ++i) {
                struct pair s_left, s_center, s_right;
                //fprintf(stderr,"->\n");
                //fprintf(stderr,"num_subsets:%d\n",num_subsets);
                //fprintf(stderr,"index_offset:%d\n",index_offset);
                //fprintf(stderr,"i:%d\n",i);
                //fprintf(stderr,"i+index_offset:%d\n",i+index_offset);
                //fprintf(stderr,"S[i+index_offset]:%p\n",S[i+index_offset]);
                //fprintf(stderr,"subset_dims[i]:%p\n",subset_dims);
                get_left_center_right(a,
                                      a_mid,
                                      a_dim,
                                      S[i+index_offset],
                                      subset_dims[i],
                                      &s_center,
                                      &center_is_empty,
                                      &s_left,
                                      &left_is_empty,
                                      &s_right,
                                      &right_is_empty);

                Q_left->q[i].start = s_left.start;
                Q_left->q[i].end = s_left.end;

                Q_right->q[i].start = s_right.start;
                Q_right->q[i].end = s_right.end;

                curr_search->q[i-1].start = s_center.start;
                curr_search->q[i-1].end = s_center.end;
                //printf("center_is_empty:%d\n",center_is_empty);
                //fprintf(stderr,"<-\n");
            }

            // If all of the left subsets are non-empty, add the left side of
            // the queue to the list to be processed later
            if (left_is_empty == 0) {
                Q_tail->next = Q_left;
                Q_tail = Q_left;
            } else {
                free(Q_left->q);
                free(Q_left);
            }

            // If all of the right subsets are non-empty, add the right side of
            // the queue to the list to be processed later
            if (right_is_empty == 0) {
                Q_tail->next = Q_right;
                Q_tail = Q_right;
            } else {
                free(Q_right->q);
                free(Q_right);
            }

            if (center_is_empty == 0) {
                //print_intersect_tree_node(curr_search);
                struct intersect_tree_node *search_next =
                    (struct intersect_tree_node *)
                    malloc(sizeof(struct intersect_tree_node));
                search_next->value = curr_search;
                search_next->next = NULL;
                search_tail->next = search_next;
                search_tail = search_next;
            } else {
                free(curr_search->q);
                curr_search = NULL;
            }

            struct search_q *tmp = Q_head;
            Q_head = Q_head->next;
            free(tmp->q);
            free(tmp);
        }
        search_head = search_head->next;
    }

    *num_nways = 0;
    struct intersect_tree_node *leaves_head = leaves_root;
    while (leaves_head != NULL) {
        leaves_head = leaves_head->next;
        *num_nways = *num_nways + 1;
    }

    if (to_print != 0 ) {
        struct intersect_tree_node *leaves_head = leaves_root;
        int nway_intersect[num_sets];
        while (leaves_head != NULL) {
            int i = num_sets - 1;
            struct intersect_tree *curr_node = leaves_head->value;
            while (curr_node->index != -1) {
                nway_intersect[i--] = curr_node->index;
                curr_node = curr_node->parent;
            }

            printf("(");
            for (i = 0; i < num_sets; ++i) {
                if (i != 0)
                    printf(" ");
                printf("%d", nway_intersect[i]);
            }
            printf(")\n");

            leaves_head = leaves_head->next;
        }
    }
}
//}}}
//{{{void print_interval_sets(struct interval **S,
void print_interval_sets(struct interval **S,
                         int num_sets,
                         int *set_sizes)
{
    int i;
    for (i = 0; i < num_sets; ++i) 
        print_interval_set(S[i], set_sizes[i]);
}
//}}}
//{{{void print_interval_set(struct interval *S,
void print_interval_set(struct interval *S,
                         int set_size)
{
    int j;
    for (j = 0; j < set_size; ++j) {
        if (j != 0)
            printf("\t");
        printf("%d %d", S[j].start, S[j].end);
    }
    printf("\n");
}
//}}}
//{{{void print_slice(char *name,
void print_slice(int id,
                 struct split_search_node *slice)
{
    /*
    struct split_search_node {
        struct split_search_node *parent;
        struct split_search_node *next;
        struct interval **S;
        struct pair S_dim;
        struct pair *s_dim;
        int has_empty;
    };
    */


    int i,j;
    for(i = 0; i < slice->S_dim.end - slice->S_dim.start + 1; ++i) {
        for(j = slice->s_dim[i].start; j <= slice->s_dim[i].end; ++j) {
            if (j != slice->s_dim[i].start)
                printf("\t");
            printf("%d %d", slice->S[i][j].start, slice->S[i][j].end);
        }
        printf("\n");
    }
}
//}}}
//{{{void print_path(struct split_search_node *node)
void print_path(struct split_search_node *node)
{
    struct pair a_dim;
    a_dim = node->s_dim[0];
    int a_mid = (a_dim.end+1 + a_dim.start-1)/2;
    printf("%d->",a_mid); 

    struct split_search_node *curr = node->parent;
    while (curr != NULL) {
        struct pair a_dim;
        a_dim = curr->s_dim[0];
        int a_mid = (a_dim.end+1 + a_dim.start-1)/2;
        printf("%d->",a_mid); 

        curr = curr->parent;
    }
    printf("\n");
}
//}}}
//{{{void print_tags(struct tag *T)
void print_tags(struct tag *T)
{

    int i,j;

    for (i = 0; i < T->num_intervals; ++i) {

        int I[T->num_sets];

        int offset = i * T->num_sets;

        for (j = 0; j < T->num_sets; ++j)
            I[j] = T->interval_ids[offset + j];
        for (j = 0; j < T->num_sets; ++j) {
            if (j!=0)
                printf("\t");
            printf("%d",I[j]);
        }
        printf("\n");
    }

    /*
    for (i = 0; i < T->num_intervals; ++i) {

        int offset = i * T->num_sets;
        for (j = 0; j < T->num_sets; ++j) {
            if (j!=0)
                printf("\t");
            printf("%d,%d", T->set_ids[j],
                            T->interval_ids[offset + j]);
        }
        printf("\n");
    }
    */
}
//}}}
//{{{void print_intersection(struct int_list_list *R) 
void print_intersection(struct int_list_list *R) {
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
//}}}
//{{{ int compare_interval_by_start (const void *a, const void *b)
int compare_int (const void *a, const void *b)
{
    int *a_i = (int *)a;
    int *b_i = (int *)b;
    return *a_i - *b_i;
}
//}}}
//{{{ int compare_interval_by_start (const void *a, const void *b)
int compare_interval_by_start (const void *a, const void *b)
{
    struct interval *a_i = (struct interval *)a;
    struct interval *b_i = (struct interval *)b;
    if (a_i->start < b_i->start)
        return -1;
    else if (a_i->start > b_i->start)
        return 1;
    else
        return 0;
}
//}}}
//{{{ void print_intersect_tree_node(struct intersect_tree *t)
void print_intersect_tree_node(struct intersect_tree *t)
{
    printf("parent:%p\t"
           "index:%d\t"
           "s_range:%d,%d\t",
           t->parent,
           t->index,
           t->s_range.start,
           t->s_range.end);
    printf("\n");
}
//}}}
//{{{ struct interval **rand_flat_sets(int num_sets,
struct interval **rand_flat_sets(int num_sets,
                                 int num_intervals,
                                 int max_interval_size,
                                 int max_gap_size,
                                 int **set_sizes)
{
    // each s is an array of intervals and S is an array of s's
    struct interval **S = (struct interval **)
                          malloc (num_sets * sizeof(struct interval *));

    *set_sizes = (int *) malloc(num_sets * sizeof(int));

    int i;
    for (i = 0; i < num_sets; ++i) {
        struct interval *s = (struct interval *)
                             malloc (num_intervals * sizeof(struct interval));

        int j;
        unsigned int j_last = 0;
        for (j = 0; j < num_intervals; ++j) {
            s[j].start = j_last + rand() % max_gap_size + 1;
            s[j].end = s[j].start + rand() % max_interval_size + 1;
            j_last = s[j].end;
        }
        S[i] = s;
        (*set_sizes)[i] = num_intervals;
    }

    return S;
}
//}}}
//{{{ struct interval **rand_set_flat_sets(int num_sets,
struct interval **rand_set_flat_sets(int num_sets,
                                     int num_intervals,
                                     int interval_size,
                                     int domain_size,
                                     int num_nways,
                                     int **set_sizes)
{
    int i;

    // Start by picking the common points
    int *nway_pts = (int *) malloc (num_nways * sizeof(int));
    for (i = 0; i < num_nways; ++i)
        nway_pts[i] = rand() % domain_size;

    // each s is an array of intervals and S is an array of s's
    struct interval **S = (struct interval **)
                          malloc (num_sets * sizeof(struct interval *));

    *set_sizes = (int *) malloc(num_sets * sizeof(int));

    // Place the common points first
    for (i = 0; i < num_sets; ++i) {
        (*set_sizes)[i] = num_intervals;
        struct interval *s = (struct interval *)
                             malloc (num_intervals * sizeof(struct interval));
        int j;
        for (j = 0; j < num_nways; ++j) {
            int spot = rand() % interval_size;
            if (nway_pts[j] - spot <= 0)
                s[j].start = 0;
            else
                s[j].start = nway_pts[j] - spot;

            s[j].end = s[j].start + interval_size;
        }
        S[i] = s;
    }

    // Place the remaining points
    for (i = 0; i < num_sets; ++i) {
        int j;
        for (j = num_nways; j < num_intervals; ++j) {
            S[i][j].start = rand() % domain_size;
            S[i][j].end = S[i][j].start + interval_size;
        }
    }

    // Sort
    for (i = 0; i < num_sets; ++i) {
        qsort(S[i],
              num_intervals,
              sizeof(struct interval),
              compare_interval_by_start);
    }

    return S;
}
//}}}
//{{{void nway_sweep(int num_sets,
void nway_sweep(int num_sets,
		int *set_sizes,
		struct interval **S,
                int *num_nways,
		int to_print)
{
    // Initialize the ordering for S
    struct pair ordering[num_sets];
    int i;
    for (i = 0; i < num_sets; i++) {
        ordering[i].start = 0;
        ordering[i].end = -1;
    }

    // Initialize the priority q
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

    *num_nways = 0;
    int scan = 1;
    while (scan == 1) {
        int64_t start_pos;

        // get the next element to place into an ordering
        // s_i is the index of the set in S that will constribute the element
        // start_pos is the starting position of that element
        int *s_i = priq_pop(q, &start_pos);

        if (s_i == NULL)
            break;

        // remove anything from the orderings that ends before start_pos
        for (i = 0; i < num_sets; i++) {
            int j;
            for (j = ordering[i].start; j <= ordering[i].end; ++j) {
                if (S[i][j].end < start_pos)  {
                    ordering[i].start += 1;
                }
            }
        }


        // Make sure that there is somethin left to push before pushing
        // the next elemetn from S[s_i]
        if (next[*s_i] < set_sizes[*s_i]) 
            priq_push(q, &set_ids[*s_i], S[*s_i][next[*s_i]].start);
        next[*s_i] += 1;

        // Check to see if adding this element causes an n-way intersection
        int is_nway = 0;
        for (i = 0; i < num_sets; i++)
            if (i != *s_i)
                if (ordering[i].end >= ordering[i].start)
                    is_nway += 1;

        if (is_nway == num_sets - 1) {
            *num_nways += count_nway(num_sets, *s_i, ordering);
            if (to_print > 0)
                print_nway(num_sets, *s_i, start_pos, ordering);
        }


        // Add the element to the ordering for that set
        ordering[*s_i].end+=1;

        // check to see if we can stop scanning To stop scanning the last
        // element in a set must have been moved out of context
        // Or all sets are out of elements
        for (i = 0; i < num_sets; i++) {
            if (ordering[i].start >= set_sizes[i])
                scan = 0;
        }
    }
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
    head->list = (int64_t *) malloc(head->size * sizeof(int64_t));
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
                    nway_copy->list = (int64_t *) 
                            malloc(head->size * sizeof(int64_t));
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
        //printf("%d\t", start_pos);
        int j;
        for (j = 0; j < num_sets; ++j) {
            if (j != 0)
                printf("\t");
            printf("%d", curr->list[j]);
        }
        printf("\n");
        curr->list[s_i] = ordering[s_i].end + 1;
        curr = curr->next;
    }
}
//}}}
//{{{ void nway_step(int num_sets,
void nway_step(int num_sets,
               int *set_sizes,
               struct interval **S,
               int to_print)
{
    int i;
    int x;
    for (i = 0; i < (num_sets - 1); i+=2) {
        nway_sweep(2,set_sizes + i, S + i, &x, 1);
        printf("\n");
    }
}
//}}}
//{{{int count_nway( int num_sets,
int count_nway(int num_sets,
               int s_i,
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
#endif
