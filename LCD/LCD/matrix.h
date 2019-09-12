#pragma once

#ifndef MATRIX_H
#define MATRIX_H

#define MATRIX_X_SIZE (64)
#define MATRIX_Y_SIZE (8)

#include "custom_print.h"
#include "ascii.h"

typedef char matrix_t [MATRIX_Y_SIZE][MATRIX_X_SIZE];

int build_matrix(int offset, chained_string_t* head, matrix_t matrix);
void dispay_matrix(matrix_t matrix);

#endif //MATRIX_H

