#include "mslab.h"
#include "timer.h"

struct mslab *mslab_init(size_t element_size, int slab_size)
{
    struct mslab *slab = (struct mslab *) malloc (sizeof(struct mslab));
    slab->slab_size = slab_size;
    slab->element_size = element_size;
    slab->head = (struct mslabs *) malloc (sizeof(struct mslabs));
    slab->head->next_free = 0;
    slab->head->next = NULL;
    slab->head->slab = (void *) malloc(slab_size * element_size);
    slab->tail = slab->head;

    return slab;
}

void mslab_grow(struct mslab *slab)
{
    struct mslabs *new_slab = (struct mslabs *) malloc (sizeof(struct mslabs));
    new_slab->slab = (void *) malloc(slab->slab_size * slab->element_size);
    new_slab->next = NULL;
    new_slab->next_free = 0;
    slab->tail->next = new_slab;
    slab->tail = new_slab;
}

void *mslab_get(struct mslab *slab)
{
    //struct timeval _t1 = in();
    struct mslabs *tail = slab->tail;
    // see if we need to grow the slab
    if (tail->next_free == slab->slab_size)
        mslab_grow(slab);
    //unsigned long t1 = out(_t1);

    //struct timeval _t2 = in();
    void *next_slab = (void *)
        (tail->slab + (tail->next_free * slab->element_size));
    //unsigned long t2 = out(_t2);

    //struct timeval _t3 = in();
    tail->next_free += 1;
    //unsigned long t3 = out(_t3);

    //fprintf(stderr,"%lu\t%lu\t%lu\n", t1,t2,t3);

    return next_slab;
}
