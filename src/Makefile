CC = gcc
CFLAGS_BASE = -std=c11 -Wall -Wextra -Werror -pedantic -pthread
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
CHECK_LIBS = -lcheck -lsubunit -lpthread -lrt -lm

SRC = s21_matrix.c
LIB = s21_matrix.a
TEST_SRC = s21_matrix_tests.c
TEST_BIN = test_matrix

CLANG_FORMAT_CONFIG = ../.clang-format

GCOV ?= 0
ifeq ($(GCOV), 1)
    CFLAGS := $(CFLAGS_BASE) $(GCOV_FLAGS)
    CHECK_LIBS += -lgcov
else
    CFLAGS := $(CFLAGS_BASE)
endif

.PHONY: all clean test run_tests gcov_report valgrind_test format

all: clean $(LIB) test

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(SRC:.c=.o)
	ar rcs $@ $^

test: clean $(LIB) $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) $(LIB) $(CHECK_LIBS) -o $(TEST_BIN)

run_tests: test
	./$(TEST_BIN)

gcov_report: clean
	$(MAKE) GCOV=1 run_tests
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report

format:
	clang-format -n -style=file:$(CLANG_FORMAT_CONFIG) *.c *.h

valgrind_test: clean test
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(TEST_BIN) 2>&1 | tee valgrind_full.log

clean:
	rm -f *.o $(LIB) $(TEST_BIN) *.gcda *.gcno coverage.info valgrind_full.log
	rm -rf coverage_report
