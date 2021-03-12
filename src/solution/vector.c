#include "vector.h"
#include <stdio.h>
#include <string.h>

vector_int* make_vector() {
    vector_int* v = malloc(sizeof(vector_int));
    v -> size = 0;
    v -> capacity = 1;
    v -> data = malloc(sizeof(int));
    return v;
}

void expand_capacity(vector_int* v) {
    int *new_data = malloc(v -> capacity * 2 * sizeof(int));
    memcpy(new_data, v -> data, v -> capacity * sizeof(int));
    free(v -> data);
    v -> data = new_data;
    v -> capacity = v -> capacity * 2;
}

void push_back(vector_int* v, int a) {
    if (v -> size < v -> capacity) {
        v -> data[v -> size] = a;
        v -> size++;
    } else {
        expand_capacity(v);
        v -> data[v -> size] = a;
        v -> size++;
    }
}

void sort(vector_int* v) {
    
}