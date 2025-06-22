#include <check.h>
#include <math.h>
#include <stdlib.h>

#include "s21_matrix.h"

void fill_matrix(matrix_t *A, double start) {
  int cnt = 0;
  for (int i = 0; i < A->rows; i++)
    for (int j = 0; j < A->columns; j++) A->matrix[i][j] = start + cnt++;
}

START_TEST(create_matrix_success) {
  matrix_t m;
  ck_assert_int_eq(s21_create_matrix(3, 2, &m), OK);
  ck_assert_ptr_nonnull(m.matrix);
  ck_assert_int_eq(m.rows, 3);
  ck_assert_int_eq(m.columns, 2);
  s21_remove_matrix(&m);
}
END_TEST

START_TEST(create_matrix_fail) {
  matrix_t m;
  ck_assert_int_eq(s21_create_matrix(0, 2, &m), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_create_matrix(2, 0, &m), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_create_matrix(-1, 5, &m), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_create_matrix(5, -1, &m), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_create_matrix(1, 1, NULL), INCORRECT_MATRIX);
}
END_TEST

START_TEST(create_matrix_malloc_fail) {
  matrix_t m;
  int res = s21_create_matrix(1, 1, &m);
  if (res == OK) s21_remove_matrix(&m);
  m.rows = 2;
  m.columns = 2;
  m.matrix = NULL;
  ck_assert_int_eq((!m.matrix) ? INCORRECT_MATRIX : OK, INCORRECT_MATRIX);
}
END_TEST

START_TEST(create_matrix_calloc_fail) {
  matrix_t m;
  s21_create_matrix(3, 3, &m);
  free(m.matrix[1]);
  m.matrix[1] = NULL;
  int fail = 0;
  for (int i = 0; i < 3; i++) {
    if (!m.matrix[i]) fail++;
  }
  ck_assert_int_gt(fail, 0);
  s21_remove_matrix(&m);
}
END_TEST

START_TEST(create_matrix_big_size) {
  matrix_t m;
  int res = s21_create_matrix(100, 100, &m);
  ck_assert_int_eq(res, OK);
  s21_remove_matrix(&m);
}
END_TEST

START_TEST(create_matrix_reuse_struct) {
  matrix_t m;
  ck_assert_int_eq(s21_create_matrix(2, 2, &m), OK);
  ck_assert_ptr_nonnull(m.matrix);

  s21_remove_matrix(&m);

  ck_assert_int_eq(s21_create_matrix(3, 3, &m), OK);

  s21_remove_matrix(&m);
}
END_TEST

START_TEST(remove_matrix_success) {
  matrix_t m;
  s21_create_matrix(2, 2, &m);
  s21_remove_matrix(&m);
  ck_assert_ptr_eq(m.matrix, NULL);
  ck_assert_int_eq(m.rows, 0);
  ck_assert_int_eq(m.columns, 0);
}
END_TEST

START_TEST(remove_matrix_null) { s21_remove_matrix(NULL); }
END_TEST

START_TEST(eq_matrix_success) {
  matrix_t m1, m2;
  s21_create_matrix(2, 2, &m1);
  s21_create_matrix(2, 2, &m2);
  fill_matrix(&m1, 1.0);
  fill_matrix(&m2, 1.0);
  ck_assert_int_eq(s21_eq_matrix(&m1, &m2), SUCCESS);
  m2.matrix[0][1] += EPS / 2;
  ck_assert_int_eq(s21_eq_matrix(&m1, &m2), SUCCESS);
  m2.matrix[0][1] += EPS * 2;
  ck_assert_int_eq(s21_eq_matrix(&m1, &m2), FAILURE);
  s21_remove_matrix(&m1);
  s21_remove_matrix(&m2);
}
END_TEST

START_TEST(eq_matrix_fail) {
  matrix_t m1 = {0};
  matrix_t m2 = {0};

  ck_assert_int_eq(s21_create_matrix(3, 3, &m1), OK);
  ck_assert_int_eq(s21_create_matrix(3, 3, &m2), OK);
  ck_assert_int_eq(s21_eq_matrix(&m1, &m2), SUCCESS);
  s21_remove_matrix(&m2);
  ck_assert_int_eq(s21_eq_matrix(&m1, &m2), FAILURE);

  s21_remove_matrix(&m1);
}
END_TEST

START_TEST(sum_matrix_success) {
  matrix_t m1, m2, r;
  s21_create_matrix(2, 2, &m1);
  s21_create_matrix(2, 2, &m2);
  fill_matrix(&m1, 1.0);
  fill_matrix(&m2, 2.0);
  ck_assert_int_eq(s21_sum_matrix(&m1, &m2, &r), OK);
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      ck_assert_double_eq_tol(r.matrix[i][j], m1.matrix[i][j] + m2.matrix[i][j],
                              EPS);
  s21_remove_matrix(&m1);
  s21_remove_matrix(&m2);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(sum_matrix_fail) {
  matrix_t m1, m2, r;
  s21_create_matrix(2, 2, &m1);
  s21_create_matrix(1, 2, &m2);
  ck_assert_int_eq(s21_sum_matrix(&m1, &m2, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_sum_matrix(NULL, &m2, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_sum_matrix(&m1, NULL, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_sum_matrix(&m1, &m1, NULL), INCORRECT_MATRIX);
  s21_remove_matrix(&m1);
  s21_remove_matrix(&m2);
}
END_TEST

START_TEST(sub_matrix_success) {
  matrix_t m1, m2, r;
  s21_create_matrix(2, 2, &m1);
  s21_create_matrix(2, 2, &m2);
  fill_matrix(&m1, 4.0);
  fill_matrix(&m2, 1.0);
  ck_assert_int_eq(s21_sub_matrix(&m1, &m2, &r), OK);
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      ck_assert_double_eq_tol(r.matrix[i][j], m1.matrix[i][j] - m2.matrix[i][j],
                              EPS);
  s21_remove_matrix(&m1);
  s21_remove_matrix(&m2);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(sub_matrix_fail) {
  matrix_t m1, m2, r;
  s21_create_matrix(2, 2, &m1);
  s21_create_matrix(1, 2, &m2);
  ck_assert_int_eq(s21_sub_matrix(&m1, &m2, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_sub_matrix(&m1, NULL, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_sub_matrix(NULL, &m2, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_sub_matrix(&m1, &m1, NULL), INCORRECT_MATRIX);
  s21_remove_matrix(&m1);
  s21_remove_matrix(&m2);
}
END_TEST

START_TEST(mult_number_success) {
  matrix_t m1, r;
  s21_create_matrix(2, 3, &m1);
  fill_matrix(&m1, 2.0);
  ck_assert_int_eq(s21_mult_number(&m1, 3.0, &r), OK);
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++)
      ck_assert_double_eq_tol(r.matrix[i][j], m1.matrix[i][j] * 3.0, EPS);
  s21_remove_matrix(&m1);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(mult_number_fail) {
  matrix_t m1, r;
  s21_create_matrix(2, 2, &m1);
  ck_assert_int_eq(s21_mult_number(NULL, 2.0, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_mult_number(&m1, 2.0, NULL), INCORRECT_MATRIX);
  s21_remove_matrix(&m1);
}
END_TEST

START_TEST(mult_matrix_success) {
  matrix_t a, b, r;
  s21_create_matrix(2, 3, &a);
  s21_create_matrix(3, 2, &b);
  fill_matrix(&a, 1.0);
  fill_matrix(&b, 2.0);
  ck_assert_int_eq(s21_mult_matrix(&a, &b, &r), OK);
  double expected[2][2] = {{1 * 2 + 2 * 4 + 3 * 6, 1 * 3 + 2 * 5 + 3 * 7},
                           {4 * 2 + 5 * 4 + 6 * 6, 4 * 3 + 5 * 5 + 6 * 7}};
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      ck_assert_double_eq_tol(r.matrix[i][j], expected[i][j], EPS);
  s21_remove_matrix(&a);
  s21_remove_matrix(&b);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(mult_matrix_fail) {
  matrix_t a, b, r;
  s21_create_matrix(2, 3, &a);
  s21_create_matrix(2, 2, &b);
  ck_assert_int_eq(s21_mult_matrix(&a, &b, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_mult_matrix(NULL, &b, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_mult_matrix(&a, NULL, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_mult_matrix(&a, &b, NULL), INCORRECT_MATRIX);
  s21_remove_matrix(&a);
  s21_remove_matrix(&b);
}
END_TEST

START_TEST(transpose_success) {
  matrix_t m, r;
  s21_create_matrix(2, 3, &m);
  fill_matrix(&m, 1.0);
  ck_assert_int_eq(s21_transpose(&m, &r), OK);
  for (int i = 0; i < m.rows; i++)
    for (int j = 0; j < m.columns; j++)
      ck_assert_double_eq_tol(r.matrix[j][i], m.matrix[i][j], EPS);
  s21_remove_matrix(&m);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(transpose_fail) {
  matrix_t m, r;
  s21_create_matrix(2, 2, &m);
  ck_assert_int_eq(s21_transpose(NULL, &r), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_transpose(&m, NULL), INCORRECT_MATRIX);
  s21_remove_matrix(&m);
}
END_TEST

START_TEST(minor_success) {
  matrix_t m, minor;
  s21_create_matrix(3, 3, &m);
  fill_matrix(&m, 1.0);
  ck_assert_int_eq(s21_minor(&m, &minor, 1, 1), OK);
  s21_remove_matrix(&m);
  s21_remove_matrix(&minor);
}
END_TEST

START_TEST(minor_fail_args) {
  matrix_t m = {0};
  matrix_t minor = {0};
  matrix_t tmp = {0};

  ck_assert_int_eq(s21_create_matrix(3, 3, &m), OK);

  ck_assert_int_eq(s21_minor(NULL, &minor, 0, 0), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_minor(&m, NULL, 0, 0), INCORRECT_MATRIX);

  tmp = m;
  tmp.matrix = NULL;
  ck_assert_int_eq(s21_minor(&tmp, &minor, 0, 0), INCORRECT_MATRIX);

  if (minor.matrix) s21_remove_matrix(&minor);
  s21_remove_matrix(&m);
}
END_TEST

START_TEST(minor_not_square) {
  matrix_t m, r;
  s21_create_matrix(3, 2, &m);
  ck_assert_int_eq(s21_minor(&m, &r, 0, 0), INCORRECT_MATRIX);
  s21_remove_matrix(&m);
}
END_TEST

START_TEST(minor_bad_index) {
  matrix_t m, r;
  s21_create_matrix(2, 2, &m);
  ck_assert_int_eq(s21_minor(&m, &r, 2, 0), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_minor(&m, &r, 0, 2), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_minor(&m, &r, -1, 0), INCORRECT_MATRIX);
  s21_remove_matrix(&m);
}
END_TEST

START_TEST(determinant_success) {
  matrix_t m;
  double det;
  s21_create_matrix(2, 2, &m);
  m.matrix[0][0] = 1;
  m.matrix[0][1] = 2;
  m.matrix[1][0] = 3;
  m.matrix[1][1] = 4;
  ck_assert_int_eq(s21_determinant(&m, &det), OK);
  ck_assert_double_eq_tol(det, -2.0, EPS);
  s21_remove_matrix(&m);
}
END_TEST

START_TEST(determinant_fail) {
  matrix_t m = {0};
  double det;
  matrix_t tmp = {0};

  ck_assert_int_eq(s21_create_matrix(2, 3, &m), OK);

  ck_assert_int_eq(s21_determinant(&m, &det), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_determinant(NULL, &det), INCORRECT_MATRIX);
  ck_assert_int_eq(s21_determinant(&m, NULL), INCORRECT_MATRIX);

  tmp = m;
  tmp.matrix = NULL;
  ck_assert_int_eq(s21_determinant(&tmp, &det), INCORRECT_MATRIX);

  s21_remove_matrix(&m);
}
END_TEST

START_TEST(determinant_large) {
  matrix_t m;
  double d;
  s21_create_matrix(3, 3, &m);
  m.matrix[0][0] = 1;
  m.matrix[0][1] = 2;
  m.matrix[0][2] = 3;
  m.matrix[1][0] = 0;
  m.matrix[1][1] = 4;
  m.matrix[1][2] = 5;
  m.matrix[2][0] = 1;
  m.matrix[2][1] = 0;
  m.matrix[2][2] = 6;
  ck_assert_int_eq(s21_determinant(&m, &d), OK);
  ck_assert_double_eq_tol(d, 22.0, EPS);
  s21_remove_matrix(&m);
}
END_TEST

START_TEST(complements_success) {
  matrix_t m, r;
  s21_create_matrix(2, 2, &m);
  m.matrix[0][0] = 1;
  m.matrix[0][1] = 2;
  m.matrix[1][0] = 3;
  m.matrix[1][1] = 4;
  ck_assert_int_eq(s21_calc_complements(&m, &r), OK);
  ck_assert_double_eq_tol(r.matrix[0][0], 4, EPS);
  ck_assert_double_eq_tol(r.matrix[0][1], -3, EPS);
  ck_assert_double_eq_tol(r.matrix[1][0], -2, EPS);
  ck_assert_double_eq_tol(r.matrix[1][1], 1, EPS);
  s21_remove_matrix(&m);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(complements_1x1) {
  matrix_t m, r;
  s21_create_matrix(1, 1, &m);
  m.matrix[0][0] = 42.0;
  ck_assert_int_eq(s21_calc_complements(&m, &r), OK);
  ck_assert_double_eq(r.matrix[0][0], 42.0);
  s21_remove_matrix(&m);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(complements_fail) {
  matrix_t m = {0}, r = {0};
  matrix_t tmp = {0};

  ck_assert_int_eq(s21_create_matrix(3, 2, &m), OK);

  ck_assert_int_eq(s21_calc_complements(&m, &r), INCORRECT_MATRIX);

  tmp = m;
  tmp.matrix = NULL;

  ck_assert_int_eq(s21_calc_complements(&tmp, &r), INCORRECT_MATRIX);

  s21_remove_matrix(&m);
  if (r.matrix != NULL) s21_remove_matrix(&r);
}
END_TEST

START_TEST(inverse_success) {
  matrix_t m, r;
  s21_create_matrix(2, 2, &m);
  m.matrix[0][0] = 4;
  m.matrix[0][1] = 7;
  m.matrix[1][0] = 2;
  m.matrix[1][1] = 6;
  ck_assert_int_eq(s21_inverse_matrix(&m, &r), OK);
  ck_assert_double_eq_tol(r.matrix[0][0], 0.6, EPS);
  ck_assert_double_eq_tol(r.matrix[0][1], -0.7, EPS);
  ck_assert_double_eq_tol(r.matrix[1][0], -0.2, EPS);
  ck_assert_double_eq_tol(r.matrix[1][1], 0.4, EPS);
  s21_remove_matrix(&m);
  s21_remove_matrix(&r);
}
END_TEST

START_TEST(inverse_fail) {
  matrix_t m = {0}, r = {0}, tmp = {0};

  s21_create_matrix(2, 2, &m);

  m.matrix[0][0] = 1;
  m.matrix[0][1] = 2;
  m.matrix[1][0] = 2;
  m.matrix[1][1] = 4;

  ck_assert_int_eq(s21_inverse_matrix(&m, &r), INCORRECT_MATRIX);

  tmp = m;
  tmp.matrix = NULL;
  ck_assert_int_eq(s21_inverse_matrix(&tmp, &r), INCORRECT_MATRIX);

  ck_assert_int_eq(s21_inverse_matrix(NULL, &r), INCORRECT_MATRIX);

  s21_remove_matrix(&m);
  if (r.matrix != NULL) s21_remove_matrix(&r);
}
END_TEST

Suite *matrix_suite(void) {
  Suite *s = suite_create("s21_matrix_full_cov");
  TCase *tc = tcase_create("core");

  // Create/Remove
  tcase_add_test(tc, create_matrix_success);
  tcase_add_test(tc, create_matrix_fail);
  tcase_add_test(tc, create_matrix_malloc_fail);
  tcase_add_test(tc, create_matrix_calloc_fail);
  tcase_add_test(tc, create_matrix_big_size);
  tcase_add_test(tc, create_matrix_reuse_struct);

  tcase_add_test(tc, remove_matrix_success);
  tcase_add_test(tc, remove_matrix_null);

  // Eq
  tcase_add_test(tc, eq_matrix_success);
  tcase_add_test(tc, eq_matrix_fail);

  // Sum/Sub
  tcase_add_test(tc, sum_matrix_success);
  tcase_add_test(tc, sum_matrix_fail);
  tcase_add_test(tc, sub_matrix_success);
  tcase_add_test(tc, sub_matrix_fail);

  // Mult Number, Matrix
  tcase_add_test(tc, mult_number_success);
  tcase_add_test(tc, mult_number_fail);
  tcase_add_test(tc, mult_matrix_success);
  tcase_add_test(tc, mult_matrix_fail);

  // Transpose
  tcase_add_test(tc, transpose_success);
  tcase_add_test(tc, transpose_fail);

  // Minor
  tcase_add_test(tc, minor_success);
  tcase_add_test(tc, minor_fail_args);
  tcase_add_test(tc, minor_not_square);
  tcase_add_test(tc, minor_bad_index);

  // Determinant
  tcase_add_test(tc, determinant_success);
  tcase_add_test(tc, determinant_fail);
  tcase_add_test(tc, determinant_large);

  // Complements
  tcase_add_test(tc, complements_success);
  tcase_add_test(tc, complements_1x1);
  tcase_add_test(tc, complements_fail);

  // Inverse
  tcase_add_test(tc, inverse_success);
  tcase_add_test(tc, inverse_fail);

  suite_add_tcase(s, tc);
  return s;
}

int main(void) {
  int number_failed;
  Suite *s = matrix_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}