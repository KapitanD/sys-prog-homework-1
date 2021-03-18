#include "vector.h"
#include <stdio.h>
#include <string.h>

void sort(int* v, int left, int right);
void merge(int* v, int left, int right);

VectorInt* VectorInt_make_vector() {
    VectorInt* v = malloc(sizeof(VectorInt));
    v -> size = 0;
    v -> capacity = 1;
    v -> data = malloc(sizeof(int));
    return v;
}

void expand_capacity(VectorInt* v) {
    int *new_data = malloc(v -> capacity * 2 * sizeof(int));
    memcpy(new_data, v -> data, v -> capacity * sizeof(int));
    free(v -> data);
    v -> data = new_data;
    v -> capacity = v -> capacity * 2;
}

void VectorInt_push_back(VectorInt* v, int a) {
    if (v -> size < v -> capacity) {
        v -> data[v -> size] = a;
        v -> size++;
    } else {
        expand_capacity(v);
        v -> data[v -> size] = a;
        v -> size++;
    }
}

void VectorInt_sort(VectorInt* v) {
    sort(v -> data, 0, v -> size);    
}

void sort(int* v, int left, int right) {
    if (left + 1 >= right) {
        return;
    }
    int mid = (left + right) / 2;
    sort(v, left, mid);
    sort(v, mid, right);
    merge(v, left, right);
    
}

void merge(int* v, int left, int right) {
    int mid = (left + right) / 2;
    int i = 0, j = 0;
    int* result = malloc(sizeof(int) * (right - left));
    while (left + i < mid && mid + j < right) {
        if (v[left + i] < v[mid + j]){
            result[i + j] = v[left + i];
            i++;
        } else {
            result[i + j] = v[mid + j];
            j++;
        }
    }
    while (left + i < mid) {
        result[i + j] = v[left + i];
        i++;
    }
    while (mid + j < right) {
        result[i + j] = v[mid + j];
        j++;
    }
    for (int k = 0; k < i + j; k++) {
        v[left + k] = result[k];
    }
    free(result);
}