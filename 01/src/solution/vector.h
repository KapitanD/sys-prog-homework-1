#ifndef MY_VECTOR_INT_H
#define MY_VECTOR_INT_H
#include <stdlib.h>

struct _VectorInt {
    size_t size, capacity;
    int* data;
};

typedef struct _VectorInt VectorInt;

VectorInt* VectorInt_make_vector();
void VectorInt_push_back(VectorInt* v, int a);
void VectorInt_sort(VectorInt* v);
VectorInt* VectorInt_merge_vectors(VectorInt** vs, unsigned int size);
#endif