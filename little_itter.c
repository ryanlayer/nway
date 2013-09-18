#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nway.h"


int main(int argc, char **argv)
{
    //{{{ Created the intervals in S
    int num_sets = atoi(argv[1]);
    int num_elements = atoi(argv[2]);
    int len = 10;
    int set_sizes[num_sets];

    struct interval *S[num_sets];

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

    struct split_search_node root_node;
    root_node.parent = NULL;
    root_node.S = S;
    root_node.S_dim.start = 0;
    root_node.S_dim.end = num_sets - 1;
    root_node.s_dim = (struct pair *) 
        malloc (num_sets * sizeof(struct pair));

    for (i = 0; i < num_sets; ++i) {
        root_node.s_dim[i].start = 0;
        root_node.s_dim[i].end = set_sizes[i] - 1;
    }

    root_node.has_empty = 0;
    root_node.next = NULL;
    struct split_search_node *head, *tail;
    head = &root_node;
    tail = &root_node;
    //}}}
    

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

        if (left->has_empty == 0) {
            tail->next = left;
            tail = left;
        }

        if (right->has_empty == 0) {
            tail->next = right;
            tail = right;
        }

        if ( (center->has_empty == 0) &&
             (center->S_dim.start < center->S_dim.end) ) {
            tail->next = center;
            tail = center;
        }

        if (center->S_dim.start == center->S_dim.end) {
            struct split_search_node_list *next_leaf;
            next_leaf = (struct split_search_node_list *)
                    malloc (sizeof(struct split_search_node_list *));
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

        curr = curr->next;
    }

    struct split_search_node_list *curr_leaf = leaf_head;

    int count = 0;
    while (curr_leaf != NULL) {
        // build path to leaf
        // add in reverse order, top element is first
        struct split_search_node *curr_parent = curr_leaf->node->parent;
        int path_len = 0;
        while (curr_parent != NULL) {
            path_len += 1;
            curr_parent = curr_parent->parent;
        }
        int path[path_len];

        curr_parent = curr_leaf->node->parent;
        int i = 0;
        while (curr_parent != NULL) {
            struct pair a_dim;
            a_dim = curr_parent->s_dim[0];
            int a_mid = (a_dim.end+1 + a_dim.start-1)/2;
            path[path_len - 1 - i] = a_mid;
            curr_parent = curr_parent->parent;
            ++i;
        }

        char path_str[255];
        int n = sprintf(path_str, "");
        for (i = 0; i < path_len; ++i) {
            if (i != 0)
                n = sprintf(path_str + strlen(path_str), "\t");
            n = sprintf(path_str + strlen(path_str), "%d", path[i]);
        }

        for (i = curr_leaf->node->s_dim[0].start;
             i <= curr_leaf->node->s_dim[0].end;
             ++i) {
            printf("%s\t%d\n", path_str, i);
        }
        curr_leaf = curr_leaf->next;
        count += 1;
    }
}
