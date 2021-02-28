#ifndef _LEGO_PROCESSOR_DEPENDENCY_VECTOR_H
#define _LEGO_PROCESSOR_DEPENDENCY_VECTPR_H

#include <lego/slab.h>

/*
 * dp_vector struct:
 * a vector which can grow itself
 * be used in dp_info struct to store the dependent pages 
 */

struct dp_vector{
    void* data;
    int elem_size;
    int allocated_size;
    int logical_size;
};


static void dp_vector_new(struct dp_vector* v, int elem_size){
    v->elem_size = elem_size;
    v->logical_size = 0;
    v->allocated_size = 4;
    v->data = kmalloc(v->elem_size * v->allocated_size, GFP_KERNEL);
}

static void dp_vector_dispose(struct dp_vector* v){
    kfree(v->data);
}

static void* dp_vector_Nth(struct dp_vector* v, int position){
    return (char*)v->data + position*v->elem_size;
}

static void dp_vector_grow(struct dp_vector* v){
    /* since there is no krealloc, we use kmalloc */
    void *tmp = kmalloc(2*v->allocated_size*v->elem_size, GFP_KERNEL);
    memcpy(tmp, v->data, v->logical_size*v->elem_size);
    kfree(v->data);
    v->data = tmp;
    v->allocated_size = 2*v->allocated_size;
}

static void dp_vector_pushback(struct dp_vector* v, void* elem_addr){
    if(v->logical_size == v->allocated_size){
        dp_vector_grow(v);
    }
    void* dest_addr = (char*)v->data + v->logical_size * v->elem_size;
    memcpy(dest_addr, elem_addr, v->elem_size);
    v->logical_size++;
}

static void dp_vector_delete(struct dp_vector* v, int position){
    void* dest_addr = (char*)v->data+position * v->elem_size;
    int byte_size = (v->logical_size-1-position) * v->elem_size;
    memmove(dest_addr,(char*)dest_addr + v->elem_size, byte_size);
    v->logical_size--;
}

static int dp_vector_size(struct dp_vector* v){
    if(v==NULL){
        return 0;
    }
    return v->logical_size;
}

static bool dp_vector_in(struct dp_vector* v, void* elem_addr){
    int i=0;
    for(i=0; i<dp_vector_size(v); i++){
        if ((struct pcache_meta *)(*dp_vector_Nth(v,i)) == (struct pcache_meta *)(*elem_addr)){
            return true;
        }
    }
    return false;
}




#endif /* _LEGO_PROCESSOR_DEPENDENCY_VECTPR_H */