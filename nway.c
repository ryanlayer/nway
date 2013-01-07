#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

//{{{ structs and helper functions
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
	//struct intersect_tree_node *children;
	int index;
	struct pair s_range;
	struct pair *q;	
};

struct intersect_tree_node
{
	struct intersect_tree *value;
	struct intersect_tree_node *next;
};

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


//{{{int b_search_starts(int key,
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

//{{{int b_search_ends(int key,
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

//{{{struct interval **rand_flat_sets(int num_sets,
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

//{{{void nway_sweep(int num_sets,
void nway_sweep(int num_sets,
				int *set_sizes,
				struct interval **S)
{

	// current is an array where current[i] is the index in set S[i] that is
	// next up
	int current[num_sets];

	// when an interval goes into context num_in_context goes up by one, and
	// when an interval leaves context num_in_context goes down by one
	int num_in_context = 0;

	// in_context is an arry where in_context[i] is the index of set S[i] that
	// is cuurent in context, by assumption each set is flat and at most one
	// interval per set can be in context at any give time
	int in_context[num_sets];
	int i;
	for (i = 0; i < num_sets; ++i) {
		// -1 means nothing is in context
		in_context[i] = -1;
		current[i] = 0;
	}

	// continue until the sweep has completely passed one of the sets, that is,
	// S[i][current[i]] == set_sizes[i], and in_contex[i] == -1
	int scan = 1;
	while (scan == 1) {

		// head is the index of the set containing the next interval to be
		// considered in the sweep
		int head = 0;

		for (i = 0; i < num_sets; ++i) {
			//printf("%d,%d ", S[i][current[i]].start, S[i][current[i]].end);
			if ( (current[i] < set_sizes[i]) &&
					(S[i][current[i]].start <= S[head][current[head]].start))
				head = i;
		}
		//S[head][current[head]] is about to be in context
		
		//See if anything must leave context first
		for (i = 0; i < num_sets; ++i) {
			if (in_context[i] > -1)
				if (S[i][in_context[i]].end <  S[head][current[head]].start) {
					in_context[i] = -1;
					--num_in_context;
				}
		}

		//S[head][current[head]] is now in context
		in_context[head] = current[head];
		++num_in_context;
		++(current[head]);

		/*
		printf("\n%d\n", num_in_context);
		printf("[");
		for (i = 0; i < num_sets; ++i) {
			if (i != 0)
				printf(" ");
			printf("%d", in_context[i]);
		}
		printf("]\n\n");
		*/


		//Check if there is an nway intersection
		/*  PRINT OUTPUT
		if (num_in_context == num_sets) {
			printf("(");
			for (i = 0; i < num_sets; ++i) {
				if (i != 0)
					printf(" ");
				printf("%d", in_context[i]);
			}
			printf(")\n");
		}
		*/

		// Check to see if the scan can stop
		for (i = 0; i < num_sets; ++i) {
			if ((current[i] == set_sizes[i]) && (in_context[i] == -1))
				scan = 0;
		}
		
	}
}
//}}}

//{{{void get_left_center_right(struct interval *a,
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
								 s_dim.end + 1) - 1;


	// s_right_i is the index of the first interval to start after 
	// the current interval (root) ends
	int s_right_i = b_search_starts(root.end,
									s,
									s_dim.start - 1,
									s_dim.end + 1);
	if ( (s_right_i <= s_dim.end) &&
			(root.end == s[s_right_i].start) )
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
	s_left->end = s_left_i;
	//fprintf(stderr,"a_mid:%d\n", a_mid);
	//fprintf(stderr,"a_dim.start:%d\n", a_dim.start);
	//fprintf(stderr,"s_center->start:%d\n", s_center->start);
	//fprintf(stderr,"s_dim.start:%d\n", s_dim.start);
	//fprintf(stderr,"s_dim.end:%d\n", s_dim.end);
	//fprintf(stderr,"a[a_mid - 1].end:%d\n", a[a_mid - 1].end);
	//fprintf(stderr,"s[s_center->start].start:%d\n", s[s_center->start].start);
	//fprintf(stderr,"s_center->start >= s_dim.start:%d\n",
		 //s_center->start >= s_dim.start);
	//fprintf(stderr,"s_center->start <= s_dim.end:%d\n",
		 //s_center->start <= s_dim.end);
	if ( (a_mid > a_dim.start) && // bound check
		 (s_center->start >= s_dim.start) && //bound check
		 (s_center->start <= s_dim.end) && //bound check
		 (a[a_mid - 1].end >= s[s_center->start].start) ) 
			s_left->end = s_center->start;

	if (s_left->start > s_left->end)
		*left_is_empty = 1;
	
	s_right->start = s_right_i;
	s_right->end = s_dim.end;
	if ( (a_mid < a_dim.end) && // bound check
		 (s_center->start >= s_dim.start) && //bound check
		 (s_center->end <= s_dim.end) && //bound check
		 (a[a_mid + 1].start <= s[s_center->end].end) ) 
			s_right->start = s_center->end;

	if (s_right->start > s_right->end)
		*right_is_empty = 1;

	//printf("(%d,%d)", s_left->start, s_left->end);
	//printf("(%d,%d)", s_center->start, s_center->end);
	//printf("(%d,%d)", s_right->start, s_right->end);
	//printf("\n");
}
//}}}

//{{{void nway_split(int num_sets,
void nway_split(int num_sets,
				int *set_sizes,
				struct interval **S)
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

	/*  PRINT OUTPUT
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
	*/
}
//}}}


int main(int argc, char **argv)
{
	if (argc != 5) {
		fprintf(stderr,"usage:\t%s "
				"<num sets> "
				"<num intervals> "
				"<max interval size> "
				"<max gap size>\n",argv[0]);
		return 1;
	}
	unsigned int seed = 1;
	srand(seed);

	int num_sets = atoi(argv[1]),
		num_intervals = atoi(argv[2]),
		max_interval_size = atoi(argv[3]),
		max_gap_size = atoi(argv[4]);

	int *set_sizes;


	struct interval **S = rand_flat_sets(num_sets,
										 num_intervals,
										 max_interval_size,
										 max_gap_size,
										 &set_sizes);

	start();
	nway_sweep(num_sets, set_sizes, S);
	stop();
	unsigned long sweep = report();

	start();
	nway_split(num_sets, set_sizes, S);
	stop();
	unsigned long split = report();
	printf("split:\t%lu\t"
		   "sweep:\t%lu\t" 
		   "split/sweep:\t%f\n", 
		   split, sweep,((double)sweep)/((double)split));
	//printf("split:\t%lu\n", split);
	//printf("sweep:\t%lu\n", sweep);
	//printf("sweep/split:\t%f\n", ((double)sweep)/((double)split));
}
