#include "nway.h"

#ifndef __UTILS_H__
#define __UTILS_H__

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

int compare_int (const void *a, const void *b);

int compare_interval_by_start (const void *a, const void *b);

int point_overlap_test(int64_t start,
                       int64_t end,
                       int64_t *points,
                       int num_points,
                       int len);

int parse_args(int argc,
               char **argv,
               struct interval ***S,
               int **set_sizes,
               int *num_sets,
               int *to_print,
               int *num_threads);

void usage(char *prog);

void gen_simple_sets(struct interval ***S,
                     int **set_sizes,
                     int num_sets,
                     int num_elements,
                     int len,
                     int seed);

void gen_sets_in_range_with_num_inter(struct interval ***S,
                                      int **set_sizes,
                                      int num_sets,
                                      int num_elements,
                                      int len,
                                      int range,
                                      int seed,
                                      int num_i);

void gen_simple_sets_in_range(struct interval ***S,
                              int **set_sizes,
                              int num_sets,
                              int num_elements,
                              int len,
                              int range,
                              int seed);

void read_interval_sets(char *file_name,
                        struct interval ***S,
                        int **set_sizes,
                        int *num_sets);

void print_nway_common_interval(struct int_list_list *R,
                                struct interval **S);
void print_nway_intervals (struct int_list_list *R,
                           struct interval **S);
void print_nway_indicies(struct int_list_list *R,
                         struct interval **S);

void print_interval_sets(struct interval **S,
                         int num_sets,
                         int *set_sizes);

void print_interval_set(struct interval *S,
                         int set_size);


#endif

