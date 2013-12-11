#include "threadpool.h"
#include <pthread.h>
#include <stdint.h>
#ifndef __NWAY_H__
#define __NWAY_H__

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

struct get_center_split_args
{
    struct interval **S;
    int num_sets, start, end;
    int *set_sizes;
    struct pair *centers;
    int *empties;
};

struct run_sweep_subset_args
{
    struct interval **S;
    int num_sets, step_size, id, num_threads;
    struct split_search_node_list *subset_head;
    struct int_list_list *R_head,*R_tail;
};

struct run_sweep_center_args
{
    struct interval **S;
    int num_sets, start, end;
    struct pair *centers;
    int *empties;
    struct int_list_list *R_head,*R_tail;
};

struct one_split_args
{
    struct split_search_node *curr;
    struct split_search_node **tail;
    struct split_search_node_list **to_clear_head;
    struct split_search_node_list **to_clear_tail;
    struct split_search_node_list **leaf_head;
    struct split_search_node_list **leaf_tail;
    pthread_mutex_t *clear_mutex;
    pthread_mutex_t *split_mutex;
    pthread_mutex_t *leaf_mutex;
};

struct pone_split_o_args
{
    struct split_search_node **curr, **tail;
    struct split_search_node_list *leaf_head, *leaf_tail;
    int *work, *waiting, num_threads;
    pthread_mutex_t *work_mutex;
    pthread_cond_t *cond_mutex;
};

struct tag
{
    int *set_ids;
    int num_sets;
    int *interval_ids;
    int num_intervals;
};

struct interval
{
    int64_t start, end;
};

struct interval_list_list
{
    struct interval_list *curr;
    struct interval_list_list *next;
};


struct interval_list
{
    struct interval *curr;
    struct interval_list *next;
};

struct pair
{
    int64_t start, end;
};

struct search_q
{
    struct pair *q;
    struct search_q *next;
};

struct intersect_tree
{
    struct intersect_tree *parent;
    int64_t index;
    struct pair s_range;
    struct pair *q;	
};

struct intersect_tree_node
{
    struct intersect_tree *value;
    struct intersect_tree_node *next;
};

struct int_list
{
    int64_t value;
    struct int_list *next;
};

struct int_list_list
{
    int64_t *list;
    struct int_list_list *next;
    int64_t size;
};

struct split_search_node {
    struct split_search_node *parent;
    struct split_search_node *next;
    struct interval **S;
    struct pair S_dim;
    struct pair *s_dim;
    int has_empty;
};

struct split_search_node_list {
    struct split_search_node *node;
    struct split_search_node_list *next;
};

int compare_int (const void *a, const void *b);


void print_tags(struct tag *T);

int64_t b_search_starts(int64_t key,
		    struct interval *S,
		    int64_t lo,
		    int64_t hi);

int64_t b_search_ends(int64_t key,
		  struct interval *S,
		  int64_t lo,
		  int64_t hi);


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
			   int *right_is_empty);

void nway_split(int num_sets,
		int *set_sizes,
		struct interval **S,
                int *num_nways,
		int to_print);

void split_search(struct split_search_node *query,
                  struct split_search_node *left,
                  struct split_search_node *center,
                  struct split_search_node *right);

void split_search_o(struct split_search_node *query,
                  struct split_search_node *left,
                  struct split_search_node *center,
                  struct split_search_node *right);

void print_slice(int id,
                 struct split_search_node *slice);

void print_path(struct split_search_node *node);

void print_intersection(struct int_list_list *R);

void split();

void sweep(struct interval **S,
           int *set_sizes,
           int num_sets,
           struct int_list_list **R,
           int *num_R);

int get_nway_sweep_list(int num_sets,
                         int s_i,
                         struct pair *ordering,
                         struct int_list_list **r_head,
                         struct int_list_list **r_tail);

void split(struct interval **S,
           int *set_sizes,
           int num_sets,
           struct int_list_list **R);

void split_o(struct interval **S,
             int *set_sizes,
             int num_sets,
             struct int_list_list **R);

void split_sweep(struct interval **S,
                 int *set_sizes,
                 int num_sets,
                 struct int_list_list **R);
void free_split_search_node (struct split_search_node *n);

void free_int_list_list(struct int_list_list *l);

void add_to_clear_list(struct split_search_node_list **to_clear_head,
                       struct split_search_node_list **to_clear_tail,
                       struct split_search_node *node);


void split_sets (struct interval **S,
                 int *set_sizes,
                 struct split_search_node_list **to_clear_head,
                 struct split_search_node_list **to_clear_tail,
                 struct split_search_node_list **leaf_head,
                 struct split_search_node_list **leaf_tail,
                 int num_sets);

void split_sets_o (struct interval **S,
                 int *set_sizes,
                 struct split_search_node_list **to_clear_head,
                 struct split_search_node_list **to_clear_tail,
                 struct split_search_node_list **leaf_head,
                 struct split_search_node_list **leaf_tail,
                 int num_sets);

void l1_split_sets_o (struct interval **S,
                 int *set_sizes,
                 struct split_search_node_list **to_clear_head,
                 struct split_search_node_list **to_clear_tail,
                 struct split_search_node_list **leaf_head,
                 struct split_search_node_list **leaf_tail,
                 int num_sets);

void l1_split_sets_centers (struct interval **S,
                            int *set_sizes,
                            int num_sets,
                            struct pair *centers,
                            int *empties);

void psplit_centers(struct interval **S,
                 int *set_sizes,
                 int num_sets,
                 struct int_list_list **R,
                 int num_threads);

void pl1_split_sets_centers (struct interval **S,
                            int *set_sizes,
                            int num_sets,
                            struct pair *centers,
                            int *empties,
                            int num_threads);

void *run_get_center_split(void *arg);

void get_center_split(struct interval **S,
                      int num_sets,
                      int *set_sizes,
                      struct interval root,
                      struct pair *centers,
                      int *is_empty);

void get_center(struct interval root,
                struct interval *s,
                struct pair s_dim,
                struct pair *s_center,
                int *center_is_empty);

void split_centers(struct interval **S,
                   int *set_sizes,
                   int num_sets,
                   struct int_list_list **R);

void sweep_subset(struct interval **S,
                  int num_sets,
                  struct pair *s_dim,
                  struct int_list_list **R_head,
                  struct int_list_list **R_tail,
                  int *num_R);


int build_split_nway(struct split_search_node_list *leaf_head,
                     struct int_list_list **R_head,
                     int num_sets);

int build_split_nway_o(struct split_search_node_list *leaf_head,
                       struct int_list_list **R_head,
                       int num_sets);

#if 0
void one_split(void *ptr);

void TS_add_to_split_search_node_list(
                       struct split_search_node_list **head,
                       struct split_search_node_list **tail,
                       struct split_search_node *node,
                       pthread_mutex_t *mutex);

void TS_add_split_search_node(struct split_search_node **tail,
                              struct split_search_node *new_node,
                              pthread_mutex_t *mutex);
#endif

void psweep_centers(struct interval **S,
                    int num_sets,
                    int *set_sizes, 
                    struct pair *centers,
                    int *empties,
                    struct int_list_list **R,
                    int num_threads);

void *run_sweep_center(void *arg);

void split_psweep(struct interval **S,
                 int *set_sizes,
                 int num_sets,
                 struct int_list_list **R,
                 int num_threads,
                 int step_size);

void psweep_subset(struct interval **S,
                   int num_sets,
                   int *set_sizes, 
                   struct split_search_node_list *subset_head,
                   struct int_list_list **R,
                   int num_threads,
                   int step_size);

void *run_sweep_subset(void *arg);

void one_split_o(struct split_search_node *curr,
                 struct split_search_node **left,
                 struct split_search_node **right,
                 struct split_search_node **center,
                 struct split_search_node_list **leaf);

void psplit_o(struct interval **S,
              int *set_sizes,
              int num_sets,
              struct int_list_list **R,
              int num_threads);

void psplit_sets_o (struct interval **S,
                    int *set_sizes,
                    struct split_search_node_list **leaf_head,
                    struct split_search_node_list **leaf_tail,
                    int num_sets,
                    int num_threads);

void *pone_split_o(void *ptr);

void splitn_sweep(struct interval **S,
                 int *set_sizes,
                 int num_sets,
                 struct int_list_list **R,
                 int num_splits,
                 int num_threads,
                 int step_size);

void l1_n_split_sets_o (struct interval **S,
                        int *set_sizes,
                        struct split_search_node_list **to_clear_head,
                        struct split_search_node_list **to_clear_tail,
                        struct split_search_node_list **leaf_head,
                        struct split_search_node_list **leaf_tail,
                        int num_sets,
                        int num_splits);

#endif
