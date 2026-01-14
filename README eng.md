# Matrix — Numerical Matrix Library in C

The **s21\_matrix** library implements core matrix operations in the C programming language (C11 standard).

## Description

The library provides functionality for creating and working with matrices:

* `s21_create_matrix` — create a matrix of the specified size
* `s21_remove_matrix` — clear and destroy a matrix
* `s21_eq_matrix` — compare matrices
* `s21_sum_matrix` — matrix addition
* `s21_sub_matrix` — matrix subtraction
* `s21_mult_matrix` — matrix multiplication
* `s21_mult_number` — multiply a matrix by a scalar
* `s21_transpose` — matrix transposition
* `s21_determinant` — compute the matrix determinant
* `s21_calc_complements` — compute the matrix of algebraic complements
* `s21_inverse_matrix` — compute the inverse matrix

The matrix is implemented as a structure with dynamically allocated memory for its elements.

## Requirements

* **Language:** C11
* **Compiler:** gcc
* **Standard:** POSIX.1-2017
* **Coding style:** consistent formatting with clang-format (based on common C conventions)
* **Library type:** static library `s21_matrix.a` with header file `s21_matrix.h`
* **Calculation precision:** up to 6 decimal places
* **Programming paradigm:** structured programming
* **Constraints:** deprecated constructs and functions are not used

## Project Structure

```
s21_matrix/
├── src/
│   ├── s21_matrix.c
│   ├── s21_matrix.h
│   ├── s21_matrix_tests.c
│   └── Makefile
├── README.md
```

## Build and Testing

```bash
# Build the library and tests
make

# Run tests
make run_tests

# Clean object files and build artifacts
make clean

# Format code according to .clang-format
make format

# Run tests with gcov coverage analysis and generate report
make gcov_report

# Run tests under valgrind to check for memory leaks
make valgrind_test
```

## Usage

Example of using the library in code:

```c
#include "s21_matrix.h"

int main() {
    matrix_t A, B, result;

    s21_create_matrix(3, 3, &A);
    s21_create_matrix(3, 3, &B);

    // Initialize matrices with values

    s21_sum_matrix(&A, &B, &result);

    // Use result

    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);

    return 0;
}
```

## Matrix Structure

```c
typedef struct matrix_struct {
    double **matrix;   // Pointer to an array of row pointers
    int rows;          // Number of rows
    int columns;       // Number of columns
} matrix_t;
```

## Return Codes

* `0` — operation completed successfully
* `1` — invalid matrix
* `2` — calculation error

## Makefile (Main Targets)

```
all — build library and tests

clean — remove build files

test — build test binary

run_tests — run tests

gcov_report — run tests with coverage and generate HTML report

format — check code formatting with clang-format

valgrind_test — run tests under valgrind to detect memory leaks
```

## Testing

The project includes comprehensive unit tests for all library functions using the **Check** framework.

## License

Author: Anastasiia Glushakova
Developed as part of engineering practice in applied mathematics and systems programming.