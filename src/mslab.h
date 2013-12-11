#include <stdio.h>
#include <stdlib.h>

#ifndef __M_SLAB_H__
#define __M_SLAB_H__


struct mslabs
{
    void *slab;
    int next_free;
    struct mslabs *next;
};

struct mslab
{
    struct mslabs *head, *tail;
    int slab_size;
    size_t element_size;
};

void *mslab_get(struct mslab *slab);
struct mslab *mslab_init(size_t element_size, int slab_size);
void mslab_grow(struct mslab *slab);

#endif
