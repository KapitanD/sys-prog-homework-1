#include "vector.h"
#include <stdio.h>
#include <string.h>

static void sort(int* v, int left, int right);
static void merge(int* v, int left, int right, int mid);
static VectorInt* merge_vectors(VectorInt** vs, int left, int right);
static VectorInt* append(VectorInt* fst, VectorInt* snd);

VectorInt* VectorInt_make_vector() {
    VectorInt* v = malloc(sizeof(VectorInt));
    v -> size = 0;
    v -> capacity = 1;
    v -> data = malloc(sizeof(int));
    return v;
}

static void expand_capacity(VectorInt* v) {
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

static void sort(int* v, int left, int right) {
    if (left + 1 >= right) {
        return;
    }
    int mid = (left + right) / 2;
    sort(v, left, mid);
    sort(v, mid, right);
    merge(v, left, right, mid);
}

void merge(int* v, int left, int right, int mid) {
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

static VectorInt* append(VectorInt* fst, VectorInt* snd) {
    int* new_data = malloc(sizeof(int) * (fst -> size + snd -> size));
    memcpy(new_data, fst -> data, fst -> size * sizeof(int));
    memcpy(new_data + fst -> size, snd -> data, snd -> size * sizeof(int));
    VectorInt* new_vector = malloc(sizeof(VectorInt));
    new_vector -> size = fst -> size + snd -> size;
    new_vector -> capacity = new_vector -> size;
    new_vector ->  data = new_data;
}

static VectorInt* merge_vectors(VectorInt** vs, int left, int right) {
    if (left + 1 >= right) {
        return vs[left];
    }
    int mid = (left + right) / 2;
    VectorInt* fst_vector = merge_vectors(vs, left, mid);
    VectorInt* snd_vector = merge_vectors(vs, mid, right);
    VectorInt* result = append(fst_vector, snd_vector);
    merge(result -> data, 0, result -> size, fst_vector -> size);
    free(fst_vector -> data);
    free(fst_vector);
    free(snd_vector -> data);
    free(snd_vector);
    return result;
}

VectorInt* VectorInt_merge_vectors(VectorInt** vs, unsigned int size) {
    return merge_vectors(vs, 0, size);
}