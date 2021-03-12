#include <stdio.h>
#include "vector.h"


vector_int* read(FILE *input) {
    int tmp = 0;
    vector_int* v = make_vector();
    while(fscanf(input, "%d ", &tmp) != EOF) {
        push_back(v, tmp);
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
    if (argc > 3) {
        printf("Sort for more then one file are not supported yet\n");
        return 3;
    }

    FILE *input = fopen(argv[2], "r+");
    if (input == NULL) {
        printf("Can't open input file %s", argv[1]);
        return 4;
    }

    vector_int* v = read(input);

    FILE* output = fopen(argv[1], "w+");
    if (output == NULL) {
        printf("Can't open input file %s", argv[1]);
        return 4;
    }

    for (int i = 0; i < v -> size; ++i) {
        fprintf(output, "%d ", v -> data[i]);
    }
    return 0;
}