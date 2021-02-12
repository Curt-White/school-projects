//
// Created by curt white on 2020-02-13.
//

#ifndef LEAST_SQUARE_MATRIX_H
#define LEAST_SQUARE_MATRIX_H

#include <stdio.h>

typedef enum matrix_type {
    VIEW,
    NORM
} matrix_type;

typedef struct matrix {
    matrix_type type;
    int col_offset;
    int rows, cols;
    double **data;
} matrix_t;

typedef struct matrix_qr {
    matrix_t q;
    matrix_t r;
} matrix_qr;

typedef struct err_res {
    double m;
    double b;
    double sad;
} err_res;

typedef struct err_ctx {
    matrix_t *d;
    int start;
} err_ctx;

void matrix_display(matrix_t *mat);
void matrix_destroy(matrix_t *mat);
void matrix_set_val(matrix_t *a, int r, int c, double val);
double matrix_get_val(matrix_t *a, int r, int c);

matrix_t matrix_init(int rows, int cols);
matrix_t matrix_from_file(FILE *input, int enumerate, int ones, int *col_select, int len_col_select);
matrix_t matrix_select(matrix_t *a, int rs,  int re, int cs, int ce);
matrix_t l2_reg(matrix_t *a, matrix_t *b, int n);
matrix_t matrix_multiply(matrix_t *a, matrix_t *b);
matrix_t matrix_transpose(matrix_t *a);
matrix_t matrix_clone(matrix_t *a);

matrix_qr matrix_gs_ortho(matrix_t *a);

err_res l1_reg(matrix_t *a, int n);

#endif //LEAST_SQUARE_MATRIX_H
