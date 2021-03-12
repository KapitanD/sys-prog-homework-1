#ifndef MY_VECTOR_INT
#define MY_VECTOR_INT
#include <stdlib.h>

struct _vector_int {
    size_t size, capacity;
    int* data;
};

#define vector_int struct _vector_int

vector_int* make_vector();
void expand_capacity(vector_int* v);
void push_back(vector_int* v, int a);
void sort(vector_int* v);
#endif