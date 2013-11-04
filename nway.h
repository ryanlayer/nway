#ifndef __NWAY_H__
#define __NWAY_H__


#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

struct tag
{
    int *set_ids;
    int num_sets;
    int *interval_ids;
    int num_intervals;
};

struct interval
{
    unsigned int start, end;
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

struct int_list
{
    int value;
    struct int_list *next;
};

struct int_list_list
{
    int *list;
    struct int_list_list *next;
    int size;
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

void print_interval_sets(struct interval **S,
                         int num_sets,
                         int *set_sizes);

void print_interval_set(struct interval *S,
                         int set_size);

void print_nway( int num_sets,
                 int s_i,
                 int start_pos,
                 struct pair *ordering);

void print_intersect_tree_node(struct intersect_tree *t);

void print_tags(struct tag *T);

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

struct interval **rand_set_flat_sets(int num_sets,
				     int num_intervals,
                                     int interval_size,
                                     int domain_size,
                                     int num_nways,
                                     int **set_sizes);

void nway_sweep(int num_sets,
		int *set_sizes,
		struct interval **S,
                int *num_nways,
		int to_print);

int count_nway(int num_sets,
               int s_i,
               struct pair *ordering);

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

void split_search(struct split_search_node *query,
                  struct split_search_node *left,
                  struct split_search_node *center,
                  struct split_search_node *right);

void print_slice(char *name,
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

void free_split_search_node (struct split_search_node *n);

void free_int_list_list(struct int_list_list *l);

void add_to_clear_list(struct split_search_node_list **to_clear_head,
                       struct split_search_node_list **to_clear_tail,
                       struct split_search_node *node);

void get_common_set(struct interval **S,
                    int *set_sizes,
                    struct int_list_list *R,
                    int num_R,
                    struct tag *T1,
                    struct tag *T2,
                    struct tag **newT,
                    struct interval **X);

void read_interval_sets(char *file_name,
                        struct interval ***S,
                        int **set_sizes,
                        int *num_sets);

void gen_simple_sets(struct interval ***S,
                     int **set_sizes,
                     int num_sets,
                     int num_elements,
                     int len,
                     int seed);

int parse_args(int argc,
                char **argv,
                struct interval ***S,
                int **set_sizes,
                int *num_sets,
                int *to_print);

void usage(char *prog);

void gen_simple_sets_in_range(struct interval ***S,
                     int **set_sizes,
                     int num_sets,
                     int num_elements,
                     int len,
                     int range,
                     int seed);
#endif

