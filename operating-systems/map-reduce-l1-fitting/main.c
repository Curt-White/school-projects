//
// Created by curt white on 2020-02-10.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "matrix.h"

FILE *get_file(char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not find file: %s\n", strerror(errno));
        return NULL;
    }

    return file;
}

void test_create_matrix() {
    FILE *data = get_file("./d2.csv");
    // test emitting columns from file and an enumerated col
    int cols[0] = {};
    matrix_t s = matrix_from_file(data, 1, 0, cols, 0);
    matrix_display(&s);
    assert(s.cols == 1);

    // test selecting only second column from file
    fseek(data, 0, SEEK_SET);
    int cols2[1] = { 1 };
    matrix_t s2 = matrix_from_file(data, 0, 1, cols2, 1);
    matrix_display(&s2);
    assert(s2.cols == 2);

    // test destroy functions
    matrix_destroy(&s);
    matrix_destroy(&s2);
}

void test_sad() {
    FILE *data = get_file("./d2.csv");
    // test emitting columns from file and an enumerated col
    int cols[1] = { 0 };
    matrix_t s = matrix_from_file(data, 1, 0, cols, 1);
    matrix_t values = matrix_init(18, 18);

    // Fill table with values like in paper provided.
    for (int i = 0; i < 18; i++) {
        for (int j = 4; j < 22 - i; j++) {
            matrix_t a = matrix_select(&s, i, i + j, -1, -1);
            err_res res = l1_reg(&a, 2);
            matrix_set_val(&values, j - 4, i, res.sad);
        }
    }

    matrix_display(&values);
    matrix_destroy(&values);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("No file provided\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-test") == 0) {
        test_create_matrix();
        test_sad();
    } else {
        FILE *data = get_file(argv[1]);
        int cols[1] = {0};
        matrix_t s = matrix_from_file(data, 1, 0, cols, 1);
        // Can modify the second argument of this for num of threads
        err_res res = l1_reg(&s, 1);
        printf("\nSlope: %f | Intercept: %f | SAR: %f\n\n", res.m, res.b, res.sad);
    }

    // Least squares stuff
//    matrix_t a = matrix_select(&s, -1, -1, 0, 2);
//    matrix_t b = matrix_select(&s, -1, -1, 2, -1);
//    matrix_display(&a);
//    matrix_display(&b);

//    l2_reg(&a, &b, 1);
}