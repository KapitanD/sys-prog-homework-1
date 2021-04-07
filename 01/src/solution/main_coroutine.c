#define __USE_MISC
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include "vector.h"
#include <time.h>

static ucontext_t ucontext_main;
static ucontext_t* ucontext_coroutines;
static int N_COROUT;

#define handle_error(msg) \
   do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define stack_size 1024 * 1024 * 1024

size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

void merge(int* v, long long int left, long long mid, long long right) {
    int i = 0, j = 0;
    size_t size = right - left;
    int* result = malloc(sizeof(int) * (size));
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

static void
sort_coroutine(int id, FILE *input, VectorInt* v, clock_t* coroutine_time) {
    clock_t start_time = clock();
    int next_id = (id + 1) % N_COROUT;
    int tmp = 0;
    while(fscanf(input, "%d ", &tmp) != EOF) {
        VectorInt_push_back(v, tmp);
        if (swapcontext(&ucontext_coroutines[id], &ucontext_coroutines[(id + 1) % N_COROUT]) == -1)
            handle_error("swapcontext");
    }
    fclose(input);
    size_t i = 1;
    size_t size = v -> size;
    while(i <= size) {
        size_t j = 0;
        while (j <= size - i) {
            if (i <= size) {
                merge(v -> data, j, j + i, min(j + 2 * i, size));   
            }
            if (swapcontext(&ucontext_coroutines[id], &ucontext_coroutines[(id + 1) % N_COROUT]) == -1)
                handle_error("swapcontext");
            j += 2 * i;
        }
        i *= 2;
    }
    *coroutine_time = clock() - start_time;
}

static void *
allocate_stack_sig()
{
	void *stack = malloc(stack_size);
	stack_t ss;
	ss.ss_sp = stack;
	ss.ss_size = stack_size;
	ss.ss_flags = 0;
	sigaltstack(&ss, NULL);
	return stack;
}

static void *
allocate_stack()
{
	return allocate_stack_sig();
}

int main(int argc, char** argv) {
    clock_t start_time = clock();
    if (argc == 1) {
        printf("No input files provided\n");
        return 1;
    }
    if (argc == 2) {
        printf("No output file provided");
        return 2;
    }

    N_COROUT = argc - 1;

    ucontext_coroutines = malloc(sizeof(ucontext_t) * N_COROUT);

    clock_t* coroutine_times = malloc(sizeof(clock_t) * N_COROUT);

    FILE **input_files = malloc(sizeof(FILE) * N_COROUT);

    VectorInt** vs = malloc(sizeof(VectorInt*) * N_COROUT);

    for (int i = 0; i < N_COROUT; ++i) {
        input_files[i] = fopen(argv[i + 1], "r+");
        if (input_files[i] == NULL) {
            printf("Can't open input file %s", argv[i + 1]);
            return 4;
        }
        vs[i] = VectorInt_make_vector();
        char *coroutine_stack = allocate_stack();
        if (getcontext(&ucontext_coroutines[i]) == -1)
		    handle_error("getcontext");
        ucontext_coroutines[i].uc_stack.ss_sp = coroutine_stack;
        ucontext_coroutines[i].uc_stack.ss_size = stack_size;
        if (i != 0) {
            ucontext_coroutines[i].uc_link = &ucontext_coroutines[i - 1];
        } else {
            ucontext_coroutines[i].uc_link = &ucontext_main;
        }
        makecontext(&ucontext_coroutines[i], sort_coroutine, 4, i, input_files[i], vs[i], &coroutine_times[i]);
    }

    if (swapcontext(&ucontext_main, &ucontext_coroutines[0]) == -1)
        handle_error("swapcontext");

    VectorInt* merged_vector = VectorInt_merge_vectors(vs, N_COROUT);

    FILE* output = fopen("test/files/out.txt", "w+");

    for (int i = 0; i < merged_vector -> size; ++i) {
        fprintf(output, "%d ", merged_vector -> data[i]);
    }

    printf("Total time: %f\n", ((float) start_time - clock())/CLOCKS_PER_SEC*1000000.);

    for (int i = 0; i < N_COROUT; ++i) {
        printf("Coroutine %d time: %f\n", i, ((float) coroutine_times[i])/CLOCKS_PER_SEC*1000000.);
    }
    free(ucontext_coroutines);
    free(coroutine_times);
    return 0;
}