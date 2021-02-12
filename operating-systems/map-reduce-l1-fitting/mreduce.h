//
// Created by curt white on 2020-02-15.
//

#ifndef LEAST_SQUARE_MREDUCE_H
#define LEAST_SQUARE_MREDUCE_H

void **map(void **input, int input_len, void *(*fn)(void*, void*, int), int num_threads, void *context);
#endif //LEAST_SQUARE_MREDUCE_H