#include "mslab.h"

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
    // see if we need to grow the slab
    if (slab->tail->next_free == slab->slab_size)
        mslab_grow(slab);

    void *next_slab = (void *)
        (slab->tail->slab + (slab->tail->next_free * slab->element_size));

    slab->tail->next_free += 1;

    return next_slab;


}
