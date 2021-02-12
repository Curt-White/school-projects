//
// Created by curt white on 2020-02-13.
//
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "matrix.h"
#include "mreduce.h"

#define DEF_ROW_SPACE 50
#define MAX_LINE_SIZE 100

typedef struct parse_map_ctx {
    int enumerate;
    int ones;
    int cols;
    int *col_select;
    int len_col_select;
} parse_map_ctx;

/**
 * Destroy a dynamically allocated array full of dynamically
 * allocated items. Will free the array and each element in the
 * array.
 * @param data - The array to deallocate
 * @param len - The length of the array to deallocate
 */
void array_destroy(void **data, int len) {
    int i = 0;
    while(i < len) {
        free(data[i]);
        i ++;
    }

    free(data);
}

/**
 * Look up a value from the matrix at the index of row and col.
 * TODO: Setup changes for faster transpose
 * TODO: modify to return error if out of bounds
 * @param a - Matrix to select from
 * @param r - Row number
 * @param c - Col number
 * @return - Value from location in the matrix
 */
double matrix_get_val(matrix_t *a, int r, int c) {
    return a->data[r][c + a->col_offset];
}

/**
 * Set a value at the given location column C and row R. Not necessary
 * to add the value using this function but it is intended to be safe.
 * TODO: Setup changes for faster transpose
 * TODO: modify to return error if out of bounds
 * @param a - A matrix to set value in
 * @param r - Row number
 * @param c - Column number
 * @param val - v
 */
void matrix_set_val(matrix_t *a, int r, int c, double val) {
    a->data[r][c + a->col_offset] = val;
}

// force exit upon memory allocation error
void exit_mem_err() {
    fprintf(stderr, "Error Allocating Memory\n");
    exit(EXIT_FAILURE);
}

/**
 * Deconstruct a matrix. If the matrix is not of type NORM then
 * the matrix will not be deallocated. This is to prevent views
 * from being deallocated since users may try to deallocate the original
 * matrix after.
 * @param mat - Matrix to destroy
 */
void matrix_destroy(matrix_t *mat) {
    if (mat->type != NORM) return;
    array_destroy((void **) mat->data, mat->rows);
}

/**
 * Fetch each row from a file
 * @param input - File to read from (CSV)
 * @param rows - Return value used to store the row count
 * @return - Pointer to the lines of the file
 */
char **get_rows(FILE *input, int *rows) {
    char **row_buffer = (char **)malloc(sizeof(char*) * DEF_ROW_SPACE);
    if (row_buffer == NULL) exit_mem_err();
    int buf_size = DEF_ROW_SPACE;
    int row = 0;

    while (1) {
        // Allocate one more space for \n if last line is not a new line
        row_buffer[row] = (char*)malloc(sizeof(char) * (MAX_LINE_SIZE + 1));
        if (row_buffer[row] == NULL) exit_mem_err();
        if(fgets(row_buffer[row], MAX_LINE_SIZE, input) == NULL) break;
        row++;

        if (row >= buf_size) {
            buf_size += DEF_ROW_SPACE;
            char **temp = realloc(row_buffer, buf_size * sizeof(char*));
            if (temp == NULL) exit_mem_err();

            row_buffer = temp;
        }
    }

    strncat(row_buffer[row - 1], "\n", 1);
    row_buffer[row] = NULL;
    *rows = row;

    return row_buffer;
}

/**
 * Print the matrix to console
 * @param mat
 */
void matrix_display(matrix_t *mat) {
    for (int i = 0; i < mat->rows; i += 1) {
        printf("|");
        for (int j = mat->col_offset; j < mat->col_offset + mat->cols; j += 1) {
            printf(" %.2f |", mat->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Strtok does not work properly in multi-threaded environment so
 * this is a small replacement for Strtok
 * @param loc - Start of a string
 * @param del - delimiter character
 * @return - location after first token found or NULL if end of string
 */
char *custom_tok(char *loc, char del) {
    char *n = loc;
    while (1) {
        if (*n == '\0') {
            return NULL;
        }
        if (*n == '\n' || *n == del) {
            *n = '\0';
            return n + 1;
        }

        n++;
    }
}

/**
 * Mapper function to parse CSV formatted columns
 * @param row - Current row in the CSV
 * @param context - Parsing context from mapper
 * @param index - Index from mapper
 * @return - output for mapper
 */
void *format_row(void* row, void *context, int index) {
    parse_map_ctx *ctx = (parse_map_ctx *) context;

    double *new_row = (double *) malloc(sizeof(double *) * ctx->cols);
    if (new_row == NULL) exit_mem_err();

    int curr_col = 0;
    int curr_idx = 0;
    int select_idx = 0;
    // Add a column of all ones
    if (ctx->ones != 0) {
        new_row[curr_idx] = 1;
        curr_idx ++;
    }
    // Add a column of enumerated values beginning at 1
    if (ctx->enumerate != 0) {
        new_row[curr_idx] = index + 1;
        curr_idx ++;
    }

    char *curr = (char *) row;
    char *next = custom_tok(curr, ',');
    do {
        // If using col select then only grab selected columns
        if (ctx->len_col_select > 0) {
            if (ctx->col_select[select_idx] == curr_col) {
                new_row[curr_idx] = strtod(curr, NULL);
                curr_idx ++;
                select_idx ++;
            }
        } else {
            curr_idx ++;
        }

        curr_col ++;
        curr = next;
        next = custom_tok(curr, ',');
    } while (next != NULL);

    return new_row;
}

/**
 * Create a matrix from a CSV file
 * @param input - The input file in CSV format
 * @param enumerate - Boolean enumerate the lines in the file?
 * @param col_select - A list of columns to select (not mandatory)
 * @param len_col_select - Number of columns to select
 * @return - A matrix structure
 */
matrix_t matrix_from_file(FILE *input, int enumerate, int ones, int *col_select, int len_col_select) {
    int row_count = 0;
    int col_count = 0;
    char **rows = get_rows(input, &row_count);
    if (col_select != NULL) {
        col_count = len_col_select;
    } else {
        for(int j = 0; j < strlen(rows[0]); j++) {
            if (rows[0][j] == ',') {
                col_count ++;
            }
        }
        col_count ++;
    }
    col_count += (enumerate != 0 ? 1 : 0 );
    col_count += (ones != 0 ? 1 : 0 );

    parse_map_ctx args = { enumerate, ones, col_count, col_select, len_col_select };
    // Subtracting 1 from row count and increase row pointer by one to excluded header row
    double **data = (double **)map((void **)(rows + 1), row_count - 1, format_row, 1, (void *)&args);
    matrix_t mat = { NORM, 0, row_count - 1, col_count, data };
    array_destroy((void **) rows, row_count);
    return mat;
}

/**
 * Create a matrix of all zeroes.
 * @param rows - Number of rows to add to matrix
 * @param cols - Number of columns to add to matrix
 * @return - A new matrix
 */
matrix_t matrix_init(int rows, int cols) {
    double **data = (double **) malloc(sizeof(double *) * rows);
    if (data == NULL) exit_mem_err();

    for (int i = 0; i < rows; i ++) {
        double *col = (double *) malloc(sizeof(double) * cols);
        if (col == NULL) exit_mem_err();
        memset(col, 0, sizeof(double) * cols);

        data[i] = col;
    }

    matrix_t mat = { NORM, 0, rows, cols, data };
    return mat;
}

/**
 * Select a section of a matrix and retrieve a view of the matrix where it references
 * a section of the matrix. If wish to select until the end use -1.
 * @param a - A Matrix to select a view of
 * @param rs - Starting row
 * @param re - End row not inclusive
 * @param cs - Start column
 * @param ce - End column not inclusive
 * @return
 */
matrix_t matrix_select(matrix_t *a, int rs,  int re, int cs, int ce) {
    int row_start = rs < 0 ? 0 : rs;
    int col_start = cs < 0 ? 0 : cs;

    if (ce > a->cols || re > a->rows) {
        fprintf(stderr, "Matrix: Column Index Out Of Bounds\n");
        exit(EXIT_FAILURE);
    }

    int row_end = re < 0 ? a->rows : re;
    int col_end = ce < 0 ? a->cols : ce;

    matrix_t sel = { VIEW, col_start, (row_end - row_start), (col_end - col_start), a->data + row_start };
    return sel;
}

/**
 * Clone a matrix and all of its contents.
 * @param a - Matrix to clone
 * @return - A clone of matrix a
 */
matrix_t matrix_clone(matrix_t *a) {
    double **data = (double **) malloc(sizeof(double *) * a->rows);
    if (data == NULL) exit_mem_err();

    for (int i = 0; i < a->rows; i++) {
        double *col = (double *) malloc(sizeof(double) * a->cols);
        if (col == NULL) exit_mem_err();
        memcpy(col, &a->data[i][a->col_offset], sizeof(double) * a->cols);
        data[i] = col;
    }

    matrix_t new = { NORM, 0, a->rows, a->cols, data };
    return new;
}

/**
 * Multiply two matrices in the form: A x B
 * @param a - Matrix a left arg
 * @param b - Matrix b right arg
 * @return - Result matrix
 */
matrix_t matrix_multiply(matrix_t *a, matrix_t *b) {
    if (a->cols != b->rows) {
        fprintf(stderr, "A's column count must match B's row count for multiplication\n");
        exit(EXIT_FAILURE);
    }

    matrix_t result = matrix_init(a->rows, b->cols);
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            double val = 0;
            for (int k = 0; k < b->rows; k++) {
                val += a->data[i][k] * b->data[k][j];
            }

            result.data[i][j] = val;
        }
    }

    return result;
}

/**
 * Find the transpose of a matrix
 * TODO: This is a really slow method need to abstract the transpose without actually moving rows
 * @param a - Matrix to transpose
 * @return - A transpose matrix of a
 */
matrix_t matrix_transpose(matrix_t *a) {
    double **data = (double **) malloc(sizeof(double *) * a->cols);
    if (data == NULL) exit_mem_err();

    for (int i = 0; i < a->cols; i++) {
        double *row = (double *) malloc(sizeof(double) * a->rows);
        if (row == NULL) exit_mem_err();

        for (int j = 0; j < a->rows; j++) {
            row[j] = a->data[j][i];
        }
        data[i] = row;
    }

    matrix_t transpose = { NORM, 0, a->cols, a->rows, data };
    return transpose;
}

/**
 * Assign the data in matrix b to matrix a. The dimensions of the two
 * matrices must be the same. Can use matrix views to select certain
 * sections of matrix and assign values to that section.
 * @param a - Matrix to be assigned to
 * @param b - Values to be assigned to a
 */
void matrix_assign(matrix_t *a, matrix_t *b) {
    if (a->rows != b->rows || a->cols != b->cols) {
        fprintf(stderr, "Matrices must have the same dimensions to assign\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            a->data[i][j + a->col_offset] = b->data[i][j + b->col_offset];
        }
    }
}

/**
 * Find the 2-norm or magnitude of a vector. This must be a single column
 * wide matrix.
 * @param a - Vector to find magnitude
 * @return - Magnitude of vector
 */
double matrix_2_norm(matrix_t *a) {
    if (a->cols != 1) {
        fprintf(stderr, "Bad dimensions, must be column vector\n");
        exit(EXIT_FAILURE);
    }

    double sum = 0;
    for (int i = 0; i < a->rows; ++i) {
        sum += pow(a->data[i][a->col_offset], 2);
    }

    return sqrt(sum);
}

// Element wise subtraction
void el_sub(matrix_t *a, double val) {
    for (int i = 0; i < a->rows; ++i) {
        for (int j = 0; j < a->cols; ++j) {
            a->data[i][j + a->col_offset] -= val;
        }
    }
}

// Element wise multiplication
void el_mul(matrix_t *a, double val) {
    for (int i = 0; i < a->rows; ++i) {
        for (int j = 0; j < a->cols; ++j) {
            a->data[i][j + a->col_offset] *= val;
        }
    }
}

// Element wise division
void el_div(matrix_t *a, double val) {
    for (int i = 0; i < a->rows; ++i) {
        for (int j = 0; j < a->cols; ++j) {
            a->data[i][j + a->col_offset] /= val;
        }
    }
}

// Subtract vector A from B
void vector_sub(matrix_t *a, matrix_t *b) {
    if (a->rows != b->rows) {
        fprintf(stderr, "Bad dimensions, must be same length\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < a->rows; i++) {
        a->data[i][a->col_offset] -= b->data[i][b->col_offset];
    }
}

/**
 * Solve a matrix a with and vector b.
 * TODO: Implement
 * @param a
 * @param b
 */
void matrix_solver(matrix_t *a, matrix_t *b) {

}

void *gs_mapper(void* row, void *context, int index) {
    matrix_t *a = (matrix_t *) row;
    matrix_qr qr = matrix_gs_ortho(a);
    matrix_destroy(&qr.q);

    matrix_t *r = (matrix_t *)malloc(sizeof(matrix_t));
    memcpy(r, (void *)&qr.r, sizeof(matrix_t));

    return (void *)r;
}

matrix_t matrix_concat(matrix_t **matrix_array, int n) {
    int cols = matrix_array[0]->cols;
    int rows = 0;

    for (int i = 0; i < n; i++) rows += matrix_array[i]->rows;

    double **data = (double **) malloc(sizeof(double *) * n);
    if (data == NULL) exit_mem_err();

    int curr_row = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < matrix_array[i]->rows; j++) {
            data[curr_row] = matrix_array[i]->data[j];
            curr_row ++;
        }

        free(matrix_array[i]->data);
    }

    matrix_t new = { NORM, 0, cols, rows, data };
    return  new;
}

/**
 * Least squares solution using graham schmidt orthogonalization to
 * calculate QR factorizations and using indirect TSQR methods to combine R factors
 * in each step.
 * @param a - input a matrix
 * @param b - input b matrix
 * @param n - number of threads
 * @return
 */
matrix_t l2_reg(matrix_t *a, matrix_t *b, int n) {
    matrix_t *mats = (matrix_t *) malloc(sizeof(matrix_t) * n);
    matrix_t **matsp = (matrix_t **) malloc(sizeof(matrix_t *) * n);

    int len = a->rows / n;
    for (int i = 0; i < n; ++i) {
        if (i == n - 1) {
            mats[i] = matrix_select(a, i * len, -1, -1, -1);
        } else {
            mats[i] = matrix_select(a, i * len, (i + 1) * len, -1, -1);
        }

        matsp[i] = &mats[i];
    }

    matrix_t **r = (matrix_t **)map((void **)matsp, n, gs_mapper, n, NULL);
    for (int j = 0; j < n; ++j) {
        matrix_display(r[j]);
    }

    return mats[0];
}

/**
 * A function for the map reduce system. Finds the sum of absolute
 * differences for some line passing through index and the start index.
 * @param row - Unused but necessary for all mapping function.
 * @param ctx - Context provided globally for entire mapping
 * @param index - Index of the current item in inputs array
 * @return pointer to a result with slope, intercept and sum of absolute errors
 */
void *sum_error(void *row, void *ctx, int index) {
    err_ctx *c = (err_ctx *)ctx;
    matrix_t *a = c->d;
    int idx1 = c->start;
    int idx2 = c->start + index + 1;
    double x1 = a->data[idx1][0], x2 = a->data[idx2][0];
    double y1 = a->data[idx1][1], y2 = a->data[idx2][1];
    double m = (y2 - y1) / (x2 - x1);
    double b = y1 - m * x1;

    double sum = 0;
    for (int i = 0; i < a->rows; i++) {
        double v1 = m * a->data[i][0] + b;
        sum += fabs(a->data[i][1] - v1);
    }

    err_res *fin = (err_res *) malloc(sizeof(err_res));
    fin->m = m; fin->b = b; fin->sad = sum;
    return (void *)fin;
}

/**
 * Compute the L1 best fit line for the
 * @param a - Matrix a as input
 * @param n - Number of threads for processing
 * @return - A structure of the result for the best fit line
 */
err_res l1_reg(matrix_t *a, int n) {
    err_ctx ctx = { a, 0};
    err_res min = { 0, 0, 0 };
    int init = 0;
    int total = 0;

    for (int i = 0; i < a->rows - 1; i++) {
        ctx.start = i;
        int rows = a->rows - i - 1;
        // This is wasteful but need empty input array for map
        void *res = malloc(sizeof(void *) * rows);
        if (res == NULL) exit_mem_err();
        void **r = map(res, rows, sum_error, n, &ctx);

        for (int j = 0; j < rows; j ++) {
            total += 1;
            err_res *curr = ((err_res *)r[j]);
            if (init == 0) {
                min = *curr;
                init = -1;
            } else if (curr->sad < min.sad) {
                min = *curr;
            }
        }

        free(res);
        array_destroy(r, rows);
    }

    return min;
}

/**
 * Graham-schmidt orthogonalization which creates two matrix
 * factors, an upper triangular matrix R and an orthogonal matrix Q. These
 * factors can be used to compute the least square.
 * @param a - An input matrix
 * @param q - An orthogonal matrix Q
 * @param r - An upper triangular matrix
 */
matrix_qr matrix_gs_ortho(matrix_t *a) {
    matrix_t r = matrix_init(a->cols, a->cols);
    matrix_t q = matrix_init(a->rows, a->cols);


    for (int i = 0; i < a->cols; i ++) {
        matrix_t ai = matrix_select(a, -1, -1, i, i + 1);
        matrix_t v = matrix_clone(&ai);

        for (int j = 0; j < i; j ++) {
            matrix_t qj = matrix_select(&q, -1, -1, j, j + 1);
            matrix_t qt = matrix_transpose(&qj);
            matrix_t res = matrix_multiply(&qt, &v);

            matrix_t r_curr = matrix_select(&r, j, j+1, i, i+1);
            matrix_assign(&r_curr, &res);

            matrix_t qjr = matrix_clone(&qj);
            el_mul(&qjr, matrix_get_val(&r, j, i));
            vector_sub(&v, &qjr);

            matrix_destroy(&qjr);
            matrix_destroy(&qt);
            matrix_destroy(&res);
        }

        matrix_set_val(&r, i, i, matrix_2_norm(&v));
        matrix_t qi = matrix_select(&q, -1, -1, i, i + 1);
        matrix_assign(&qi, &v);
        el_div(&qi, matrix_get_val(&r, i, i));
    }

    matrix_qr qr = { q, r };
    return  qr;
}