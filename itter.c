#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pq.h"
#include "nway.h"

int main(int argc, char **argv)
{
    struct interval **S;
    int *set_sizes;
    int to_print;
    int num_sets;


    int r = parse_args(argc,
                       argv,
                       &S,
                       &set_sizes,
                       &num_sets,
                       &to_print);

    if (to_print != 0) {
        print_interval_sets(S, num_sets, set_sizes);
        printf("\n");
    }


    //Tag each interval
    int i,j;
    struct tag *T = (struct tag *) malloc(num_sets * sizeof(struct tag));
    for (i = 0; i < num_sets; ++i) {
        T[i].num_sets = 1;
        T[i].set_ids = (int *) malloc(T[i].num_sets * sizeof(int));
        T[i].set_ids[0] = i;

        T[i].num_intervals = set_sizes[i];
        T[i].interval_ids = (int *) malloc(T[i].num_intervals * sizeof(int));
        for (j = 0; j < T[i].num_intervals; j++) {
            T[i].interval_ids[j] = j;
        }
    }

    struct interval **curr_S = S;
    int *curr_set_sizes = set_sizes;
    struct tag *curr_T = T;

    int num_rounds = log2((float)num_sets);
    for (i = 0; i < num_rounds; ++i) {
        int num_pairs = num_sets / (i+1);

        struct interval **next_S = (struct interval **)
                malloc(num_pairs * sizeof(struct interval));
        int *next_set_sizes = (int *) malloc(num_pairs * sizeof(int));
        struct tag *next_T = (struct tag *)
                malloc(num_pairs * sizeof(struct tag));

#ifdef DEBUGITTER
        printf("->\n");
        print_interval_sets(curr_S,num_pairs,curr_set_sizes);
        printf("<-\n");
#endif

        for (j = 0; j < num_pairs; j+=2) {

            struct int_list_list *R;
            int num_R;

            struct interval **tmp_S = &curr_S[j];
            int *tmp_set_sizes = &curr_set_sizes[j];
            sweep(tmp_S, tmp_set_sizes, 2, &R, &num_R);

            struct interval *I;
            struct tag *tmp_T;

            get_common_set(tmp_S,
                           tmp_set_sizes,
                           R,
                           num_R,
                           &curr_T[j],
                           &curr_T[j+1],
                           &tmp_T,
                           &I);
            print_interval_set(I,num_R);
            //print_intersection(R);
#ifdef DEBUGITTER
            printf("-T0->\n");
            print_tags(&curr_T[j]);
            printf("<-T0-\n");
            printf("-T1->\n");
            print_tags(&curr_T[j+1]);
            printf("<-T1-\n");
            printf("-I->\n");
            print_intersection(R);
            printf("<-I-\n");
            printf("-T->\n");
            print_tags(tmp_T);
            printf("<-T-\n");
#endif

            next_S[j/2] = I;
            next_set_sizes[j/2] = num_R;
            next_T[j/2] = *tmp_T;

        }

        curr_S = next_S;
        curr_set_sizes = next_set_sizes;
        curr_T = next_T;
    }
    print_tags(curr_T);

#if 0
//{{{ loop over pairs
    struct interval *RS[2];
    int r_set_sizes[num_sets];
    int set_num = 0;
    for (j = 0; i < num_sets; j+=2) {
        struct interval **tmp_S = &S[j];
        int *tmp_set_sizes = &set_sizes[j];

        struct int_list_list *R;
        int num_R;
        sweep(tmp_S, tmp_set_sizes, 2, &R, &num_R);

        struct interval *N;
        struct tag *N_tags;

        get_common_set(tmp_S,
                       tmp_set_sizes,
                       R,
                       num_R,
                       &T[j],
                       &T[j+1],
                       &N_tags,
                       &N);

        print_intersection(R);
        printf("\n");
        print_tags(N_tags);
        printf("\n");

        RS[set_num] = N;
        r_set_sizes[set_num]=num_R;
        ++set_num;
    }
    print_interval_sets(RS,2,r_set_sizes);
    struct int_list_list *F;
    int num_F;
    sweep(RS, r_set_sizes, 2, &F, &num_F);
    printf("\n");
    print_intersection(F);
//}}}
#endif

#if 0
    struct int_list_list *R_1;
    int num_R_1;
    sweep(S, set_sizes, 2, &R_1, &num_R_1);
    struct interval *N_1;
    struct tag *N_tags_1;
    get_common_set(S,
                   set_sizes,
                   R_1,
                   num_R_1,
                   &T[0],
                   &T[1],
                   &N_tags_1,
                   &N_1);
    print_intersection(R_1);
    printf("\n");
    print_tags(N_tags_1);
    printf("\n");
#endif

#if 0
    struct int_list_list *R_2;
    int num_R_2;
    struct interval **S_2 = &S[2];
    int *set_sizes_2 = &set_sizes[2];
    sweep(S_2, set_sizes_2, 2, &R_2, &num_R_2);
    struct interval *N_2;
    struct tag *N_tags_2;
    get_common_set(S_2,
                   set_sizes_2,
                   R_2,
                   num_R_2,
                   &T[2],
                   &T[3],
                   &N_tags_2,
                   &N_2);
    print_intersection(R_2);
    printf("\n");
    print_tags(N_tags_2);
#endif


#if 0
    struct interval *X[2];
    struct tag *XT[2];
    int x_set_sizes[2];

    struct interval **S1 = &S[0];
    int *set_sizes_1 = &set_sizes[0];
    struct int_list_list *R_1;
    R_1 = NULL;
    //int num_R_1;
    sweep(S1, set_sizes_1, 2, &R_1, &x_set_sizes[0]);
    //struct interval *X_1;
    //struct tag *newT_1;
    get_common_set(S1,
                   set_sizes_1,
                   R_1,
                   x_set_sizes[0],
                   &T[0],
                   &T[1],
                   &XT[0],
                   &X[0]);

    struct interval **S2 = &S[2];
    int *set_sizes_2 = &set_sizes[2];
    struct int_list_list *R_2;
    R_2 = NULL;
    //int num_R_2;
    sweep(S2, set_sizes_2, 2, &R_2, &x_set_sizes[1]);
    //struct interval *X_2;
    //struct tag *newT_2;
    get_common_set(S2,
                   set_sizes_2,
                   R_2,
                   x_set_sizes[1],
                   &T[2],
                   &T[3],
                   &XT[1],
                   &X[1]);
 
    print_tags(XT[0]);
    printf("\n");
    print_tags(XT[1]);
    printf("\n");

    struct int_list_list *R_3;
    int num_R_3;
    sweep(X, x_set_sizes, 2, &R_3, &num_R_3);
    struct interval *X_3;
    struct tag *newT_3;
    get_common_set(X,
                   x_set_sizes,
                   R_3,
                   num_R_3,
                   XT[0],
                   XT[1],
                   &newT_3,
                   &X_3);

    print_tags(newT_3);

    //print_intersection(R);
    //print_interval_set(X,num_R);
   
    free_int_list_list(R_1);
    free_int_list_list(R_2);
#endif

    for (i = 0; i < num_sets; i++) 
        free(S[i]);
}

