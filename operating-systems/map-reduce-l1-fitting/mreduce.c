//
// Created by curt white on 2020-02-15.
//
#include "mreduce.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

typedef struct map_arg {
    void *context;
    void **input;
    void **result;
    void *(*fn)(void*, void*, int);
    int start_index;
    int end_index;
} map_arg;

void* map_sync(void* arg) {
    map_arg *args = (map_arg *) arg;
    for (int i = args->start_index; i < args->end_index; i++) {
        args->result[i] = (*args->fn) (args->input[i], args->context, i);
    }

    return NULL;
}

void **map(void **input, int input_len, void *(*fn)(void*, void*, int), int num_threads, void *context) {
    int items_each = input_len / num_threads;
    if (input_len < num_threads) {
        num_threads = input_len;
        items_each = 1;
    }

    void **data = (void **)malloc(sizeof(void *) * input_len);
    if (data == NULL) {
        fprintf(stderr, "Memory Allocation Error");
        exit(EXIT_FAILURE);
    }

    pthread_t *threads = (pthread_t *) malloc((sizeof(pthread_t) * num_threads));
    map_arg *args = (map_arg *) malloc(sizeof(map_arg) * num_threads);
    for (int i = 0; i < num_threads; i++) {
        int upper_bound = (i != num_threads - 1) ? (i + 1) * items_each : input_len;
        map_arg arg = { context, input, data, fn, i * items_each, upper_bound};
        args[i] = arg;

        int status = pthread_create(&threads[i], NULL, map_sync, (void *)&args[i]);
        if (status != 0) {
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(args);

    return data;
}