#include "s21_matrix.h"

int s21_create_matrix(int rows, int columns, matrix_t *result) {
  if (rows <= 0 || columns <= 0 || !result) {
    return INCORRECT_MATRIX;
  }
  result->rows = rows;
  result->columns = columns;

  result->matrix = (double **)malloc(rows * sizeof(double *));
  if (!result->matrix) {
    return INCORRECT_MATRIX;
  }
  for (int i = 0; i < rows; i++) {
    result->matrix[i] = (double *)calloc(columns, sizeof(double));
    if (!result->matrix[i]) {
      for (int j = 0; j < i; j++) {
        free(result->matrix[j]);
      }
      free(result->matrix);
      result->matrix = NULL;
      return INCORRECT_MATRIX;
    }
  }
  return OK;
}

void s21_remove_matrix(matrix_t *A) {
  if (A && A->matrix) {
    for (int i = 0; i < A->rows; i++) {
      free(A->matrix[i]);
    }
    free(A->matrix);
    A->matrix = NULL;
    A->rows = 0;
    A->columns = 0;
  }
}

int s21_eq_matrix(matrix_t *A, matrix_t *B) {
  if (!A || !B || !A->matrix || !B->matrix) {
    return FAILURE;
  }
  if (A->rows != B->rows || A->columns != B->columns) {
    return FAILURE;
  }
  for (int i = 0; i < A->rows; i++) {
    for (int j = 0; j < A->columns; j++) {
      if (fabs(A->matrix[i][j] - B->matrix[i][j]) > EPS) {
        return FAILURE;
      }
    }
  }
  return SUCCESS;
}

int s21_sum_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  if (!A || !B || !result) return INCORRECT_MATRIX;
  if (A->rows != B->rows || A->columns != B->columns) return INCORRECT_MATRIX;

  int status = s21_create_matrix(A->rows, A->columns, result);
  if (status == OK) {
    for (int i = 0; i < A->rows; i++) {
      for (int j = 0; j < A->columns; j++) {
        result->matrix[i][j] = A->matrix[i][j] + B->matrix[i][j];
      }
    }
  }
  return status;
}

int s21_sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  if (!A || !B || !result) return INCORRECT_MATRIX;
  if (A->rows != B->rows || A->columns != B->columns) return INCORRECT_MATRIX;

  int status = s21_create_matrix(A->rows, A->columns, result);
  if (status == OK) {
    for (int i = 0; i < A->rows; i++) {
      for (int j = 0; j < A->columns; j++) {
        result->matrix[i][j] = A->matrix[i][j] - B->matrix[i][j];
      }
    }
  }
  return status;
}

int s21_mult_number(matrix_t *A, double number, matrix_t *result) {
  if (!A || !result) return INCORRECT_MATRIX;

  int status = s21_create_matrix(A->rows, A->columns, result);
  if (status == OK) {
    for (int i = 0; i < A->rows; i++) {
      for (int j = 0; j < A->columns; j++) {
        result->matrix[i][j] = A->matrix[i][j] * number;
      }
    }
  }
  return status;
}

int s21_mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  if (!A || !B || !result) return INCORRECT_MATRIX;
  if (A->columns != B->rows) return INCORRECT_MATRIX;

  int status = s21_create_matrix(A->rows, B->columns, result);
  if (status == OK) {
    for (int i = 0; i < A->rows; i++) {
      for (int j = 0; j < B->columns; j++) {
        result->matrix[i][j] = 0.0;
        for (int k = 0; k < A->columns; k++) {
          result->matrix[i][j] += A->matrix[i][k] * B->matrix[k][j];
        }
      }
    }
  }
  return status;
}

int s21_transpose(matrix_t *A, matrix_t *result) {
  if (!A || !result) return INCORRECT_MATRIX;

  int status = s21_create_matrix(A->columns, A->rows, result);
  if (status == OK) {
    for (int i = 0; i < A->rows; i++) {
      for (int j = 0; j < A->columns; j++) {
        result->matrix[j][i] = A->matrix[i][j];
      }
    }
  }
  return status;
}

int s21_minor(matrix_t *src, matrix_t *dest, int row, int col) {
  if (!src || !dest || !src->matrix || src->rows != src->columns || row < 0 ||
      col < 0 || row >= src->rows || col >= src->columns) {
    return INCORRECT_MATRIX;
  }

  int size = src->rows;
  int dest_size = size - 1;
  int dest_i = 0, dest_j = 0;

  int status = s21_create_matrix(dest_size, dest_size, dest);
  if (status != OK) return status;

  for (int i = 0; i < size; i++) {
    if (i == row) continue;
    dest_j = 0;
    for (int j = 0; j < size; j++) {
      if (j == col) continue;
      dest->matrix[dest_i][dest_j] = src->matrix[i][j];
      dest_j++;
    }
    dest_i++;
  }
  return OK;
}

int s21_determinant(matrix_t *A, double *result) {
  if (!A || !A->matrix || A->rows != A->columns || !result)
    return INCORRECT_MATRIX;

  int size = A->rows;
  double det = 0.0;

  if (size == 1) {
    det = A->matrix[0][0];
  } else if (size == 2) {
    det = A->matrix[0][0] * A->matrix[1][1] - A->matrix[0][1] * A->matrix[1][0];
  } else {
    for (int col = 0; col < size; col++) {
      matrix_t minor;
      int status = s21_minor(A, &minor, 0, col);
      if (status != OK) return status;

      double minor_det = 0.0;
      status = s21_determinant(&minor, &minor_det);
      s21_remove_matrix(&minor);
      if (status != OK) return status;

      det += (col % 2 == 0 ? 1 : -1) * A->matrix[0][col] * minor_det;
    }
  }
  *result = det;
  return OK;
}

int s21_calc_complements(matrix_t *A, matrix_t *result) {
  if (!A || !A->matrix || A->rows != A->columns) return INCORRECT_MATRIX;

  int size = A->rows;
  int status = s21_create_matrix(size, size, result);
  if (status != OK) return status;

  if (size == 1) {
    result->matrix[0][0] = A->matrix[0][0];
    return OK;
  }

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      matrix_t minor;
      status = s21_minor(A, &minor, i, j);
      if (status != OK) {
        s21_remove_matrix(result);
        return status;
      }

      double minor_det = 0.0;
      status = s21_determinant(&minor, &minor_det);
      s21_remove_matrix(&minor);
      if (status != OK) {
        s21_remove_matrix(result);
        return status;
      }

      result->matrix[i][j] = ((i + j) % 2 == 0 ? 1 : -1) * minor_det;
    }
  }
  return OK;
}

int s21_inverse_matrix(matrix_t *A, matrix_t *result) {
  if (!A || !A->matrix || A->rows != A->columns) return INCORRECT_MATRIX;

  double det = 0.0;
  int status = s21_determinant(A, &det);
  if (status != OK) return status;
  if (fabs(det) < EPS) return INCORRECT_MATRIX;

  matrix_t complements;
  status = s21_calc_complements(A, &complements);
  if (status != OK) return status;

  matrix_t transposed;
  status = s21_create_matrix(A->rows, A->columns, &transposed);
  if (status != OK) {
    s21_remove_matrix(&complements);
    return status;
  }

  for (int i = 0; i < A->rows; i++) {
    for (int j = 0; j < A->columns; j++) {
      transposed.matrix[j][i] = complements.matrix[i][j];
    }
  }
  s21_remove_matrix(&complements);

  status = s21_mult_number(&transposed, 1.0 / det, result);
  s21_remove_matrix(&transposed);

  return status;
}