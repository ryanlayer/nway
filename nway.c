#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include "pq.h"
#include "nway.h"



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

//{{{ int b_search_starts(int key,
int b_search_starts(int key,
                    struct interval *S,
                    int lo,
                    int hi)
{
    int mid = -1;
    while (hi - lo > 1) {
        mid = (hi + lo) / 2;
        if (S[mid].start < key)
            lo = mid;
        else
            hi = mid;
    }
    return hi;
}
//}}}

//{{{ int b_search_ends(int key,
int b_search_ends(int key,
                  struct interval *S,
                  int lo,
                  int hi)
{
    int mid = -1;
    while (hi - lo > 1) {
        mid = (hi + lo) / 2;
        if (S[mid].end < key)
            lo = mid;
        else
            hi = mid;
    }
    return hi;
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
        int start_pos;

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
    int a_mid = (a_dim.end+1 + a_dim.start-1)/2;
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

//{{{ void get_left_center_right(struct interval *a,
void get_left_center_right(struct interval *a,
                           int a_mid,
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
void print_slice(char *name,
                 struct split_search_node *slice)
{
    int i;
    for(i = 0; i < slice->S_dim.end - slice->S_dim.start + 1; ++i)
        printf("\t%d\t%d\n",
                slice->s_dim[i].start,
                slice->s_dim[i].end);
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

    /*
    for (i = 0; i < T->num_intervals; ++i) {

        int I[T->num_sets];

        int offset = i * T->num_sets;

        for (j = 0; j < T->num_sets; ++j)
            I[T->set_ids[j]] = T->interval_ids[offset + j];
        for (j = 0; j < T->num_sets; ++j) {
            if (j!=0)
                printf("\t");
            printf("%d",I[j]);
        }
        printf("\n");
    }
    */

    for (i = 0; i < T->num_intervals; ++i) {
        //if (i != 0)
            //printf("\n");

        int offset = i * T->num_sets;
        for (j = 0; j < T->num_sets; ++j) {
            if (j!=0)
                printf(";");
            printf("%d,%d", T->set_ids[j],
                            T->interval_ids[offset + j]);
        }
        printf("\n");
    }
    //printf("\n");
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
                N+=1;
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
        int start_pos;

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
            if (ordering[i].start >= set_sizes[s_i])
                scan = 0;
        }

        // we can also stop scanning if there are no other intervals to add
        if (q->n == 1)
            scan = 0;
    }

    *R = nways_head;

    priq_free(q);
}
//}}}

//{{{ void split(struct interval **S,
void split(struct interval **S,
           int *set_sizes,
           int num_sets,
           struct int_list_list **R)
{
    struct split_search_node_list *to_clear_head = NULL, 
                                 *to_clear_tail = NULL;

    struct split_search_node *root_node = 
        (struct split_search_node *) malloc (
        sizeof(struct split_search_node));

    add_to_clear_list(&to_clear_head, &to_clear_tail, root_node);

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
    


    struct split_search_node_list *leaf_head, *leaf_tail;
    leaf_head = NULL;

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

        /* 
         * For any of the splits that have all non-zero subs sets,
         * add the split onto the queue to be re-split
         */
        if (left->has_empty == 0) {
            tail->next = left;
            tail = left;
            add_to_clear_list(&to_clear_head, &to_clear_tail, left);
        } else {
            free_split_search_node(left);
        }

        if (right->has_empty == 0) {
            tail->next = right;
            tail = right;
            add_to_clear_list(&to_clear_head, &to_clear_tail, right);
        } else {
            free_split_search_node(right);
        }

        if ( center->has_empty == 0) {
            /* 
             * Do not re-split the center if the next level is the last level
             * If the next level is the last level, add center onto the list of
             * leaf nodes
             */
            add_to_clear_list(&to_clear_head, &to_clear_tail, center);
            if (center->S_dim.start < center->S_dim.end) {
                tail->next = center;
                tail = center;
            } else {
                struct split_search_node_list *next_leaf;
                next_leaf = (struct split_search_node_list *)
                        malloc (sizeof(struct split_search_node_list));
                next_leaf->node = center;
                next_leaf->next = NULL;

                if (leaf_head == NULL) {
                    leaf_head = next_leaf;
                    leaf_tail = leaf_head;
                } else {
                    leaf_tail->next = next_leaf;
                    leaf_tail = next_leaf;
                }
            }
        } else {
            free_split_search_node(center);
        }

        curr = curr->next;
    }

    struct split_search_node_list *curr_leaf = leaf_head;

    struct int_list_list *R_head, *R_tail;
    R_head = NULL;
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

            new_nway->list = (int *)malloc(num_sets * sizeof(int));
            memcpy(new_nway->list, path ,num_sets * sizeof(int));

            if (R_head == NULL) {
                R_head = new_nway;
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


    struct split_search_node_list *curr_clear = to_clear_head;
    while (curr_clear != NULL) {
        struct split_search_node_list *next_clear = curr_clear->next;
        free_split_search_node(curr_clear->node);
        free(curr_clear);
        curr_clear = next_clear;
    }

    *R = R_head;
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
            (*newT)->interval_ids[offset + o] =
                    T1->interval_ids[ curr->list[0] + j ];
            ++o;
        }
        for (j = 0; j < T2->num_sets; ++j) {
            (*newT)->interval_ids[offset + o] =
                    T2->interval_ids[ curr->list[1] + j ];
            ++o;
        }
        ++i;
        curr = curr->next;
    }

}

