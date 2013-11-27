#include "utils.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>


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

//{{{ int compare_int (const void *a, const void *b)
int compare_int (const void *a, const void *b)
{
    int64_t *a_i = (int64_t *)a;
    int64_t *b_i = (int64_t *)b;

    if (*a_i < *b_i)
        return -1;
    else if (*a_i > *b_i)
        return 1;
    else
        return 0;
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
        int64_t last_end = 0;
        for (j = 0; j < (*set_sizes)[i]; j++) {
            int64_t space = rand() % 10 * len;
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

    int64_t i_points[num_i];
    for (i = 0; i < num_i; i++)
        i_points[i] = rand() % range;

    qsort(i_points, num_i, sizeof(int64_t), compare_int);

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

//{{{ int point_overlap_test(int start,
int point_overlap_test(int64_t start,
                       int64_t end,
                       int64_t *points,
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
        printf("%" PRId64 " %" PRId64, S[j].start, S[j].end);
    }
    printf("\n");
}
//}}}
