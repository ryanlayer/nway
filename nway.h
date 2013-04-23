#ifndef __NWAY_H__
#define __NWAY_H__

struct interval
{
	unsigned int start, end;
};

struct pair
{
	int start, end;
};

struct search_q
{
	struct pair *q;
	struct search_q *next;
};

struct intersect_tree
{
	struct intersect_tree *parent;
	int index;
	struct pair s_range;
	struct pair *q;	
};

struct intersect_tree_node
{
	struct intersect_tree *value;
	struct intersect_tree_node *next;
};

void print_intersect_tree_node(struct intersect_tree *t);

int b_search_starts(int key,
					struct interval *S,
					int lo,
					int hi);

int b_search_ends(int key,
					struct interval *S,
					int lo,
					int hi);

struct interval **rand_flat_sets(int num_sets,
					int num_intervals,
					int max_interval_size,
					int max_gap_size,
					int **set_sizes);

void nway_sweep(int num_sets,
				int *set_sizes,
				struct interval **S,
				int to_print);

void nway_sweep_pq(int num_sets,
				   int *set_sizes,
				   struct interval **S,
                   int *num_nways,
				   int to_print);

void nway_step(int num_sets,
				   int *set_sizes,
				   struct interval **S,
				   int to_print);

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
						   int *right_is_empty);

void nway_split(int num_sets,
				int *set_sizes,
				struct interval **S,
                int *num_nways,
				int to_print);
#endif
