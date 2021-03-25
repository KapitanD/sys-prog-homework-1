#include <stdio.h>
#include "vector.h"


VectorInt* read(FILE *input) {
    int tmp = 0;
    VectorInt* v = VectorInt_make_vector();
    while(fscanf(input, "%d ", &tmp) != EOF) {
        VectorInt_push_back(v, tmp);
    }
    return v;
}


int main(int argc, char** argv) {
    if (argc == 1) {
        printf("No input files provided\n");
        return 1;
    }
    if (argc == 2) {
        printf("No output file provided");
        return 2;
    }

    int size_input_files = argc - 2;

    FILE **input_files = malloc(sizeof(FILE) * size_input_files);
    VectorInt** vs = malloc(sizeof(VectorInt*) * size_input_files);
    for (int i = 0; i < size_input_files; ++i) {
        input_files[i] = fopen(argv[i + 2], "r+");
        if (input_files[i] == NULL) {
            printf("Can't open input file %s", argv[i + 2]);
            return 4;
        }
        vs[i] = read(input_files[i]);
        VectorInt_sort(vs[i]);
        fclose(input_files[i]);
    }

    FILE* output = fopen(argv[1], "w+");
    if (output == NULL) {
        printf("Can't open input file %s", argv[1]);
        return 4;
    }

    VectorInt* merged_vector = VectorInt_merge_vectors(vs, size_input_files);
    for (int i = 0; i < merged_vector -> size; ++i) {
        fprintf(output, "%d ", merged_vector -> data[i]);
    }
    return 0;
}