#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SIZE 9
#define UNASSIGNED 0
double start;

void print_grid(int grid[SIZE][SIZE]) {
  for (int row = 0; row < SIZE; row++) {
    for (int col = 0; col < SIZE; col++) {
      printf("%2d", grid[row][col]);
    }
    printf("\n");
  }
}

int is_exist_row(int grid[SIZE][SIZE], int row, int num) {
  for (int col = 0; col < 9; col++) {
    if (grid[row][col] == num) {
      return 1;
    }
  }
  return 0;
}

int is_exist_col(int grid[SIZE][SIZE], int col, int num) {
  for (int row = 0; row < 9; row++) {
    if (grid[row][col] == num) {
      return 1;
    }
  }
  return 0;
}

int is_exist_box(int grid[SIZE][SIZE], int startRow, int startCol, int num) {
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      if (grid[row + startRow][col + startCol] == num) {
        return 1;
      }
    }
  }
  return 0;
}

int is_safe_num(int grid[SIZE][SIZE], int row, int col, int num) {
  return !is_exist_row(grid, row, num) && !is_exist_col(grid, col, num) &&
         !is_exist_box(grid, row - (row % 3), col - (col % 3), num);
}

int find_unassigned(int grid[SIZE][SIZE], int *row, int *col) {
  for (*row = 0; *row < SIZE; (*row)++) {
    for (*col = 0; *col < SIZE; (*col)++) {
      if (grid[*row][*col] == UNASSIGNED) {
        return 1;
      }
    }
  }
  return 0;
}

void writeTime(float time) {
  FILE *file;
  file = fopen("./data.txt", "w");
  fprintf(file, "# Parallel Serial\n%f ", time);
  fclose(file);
}

int solve(int grid[SIZE][SIZE], int level) {

  int row = 0;
  int col = 0;
  if (!find_unassigned(grid, &row, &col))
    return 1;

  for (int num = 1; num <= SIZE; num++) {
    if (is_safe_num(grid, row, col, num)) {
#pragma omp task default(none) firstprivate(grid, row, col, num, level)        \
    shared(start) final(level > 1)
      {
        int copy_grid[SIZE][SIZE];
        memcpy(copy_grid, grid, SIZE * SIZE * sizeof(int));
        copy_grid[row][col] = num;
        if (solve(copy_grid, level + 1)) {
          // print_grid(copy_grid);
          double end = omp_get_wtime();
          writeTime(end - start);
          printf("Time taken in parallel: %fs\n", end - start);
          exit(0);
        }
      }
    }
  }
#pragma omp taskwait
  return 0;
}

int main(int argc, char **argv) {

  int sudoku[SIZE][SIZE] = {
      {0, 0, 0, 0, 0, 0, 0, 1, 0}, {4, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 2, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 5, 0, 4, 0, 7},
      {0, 0, 8, 0, 0, 0, 3, 0, 0}, {0, 0, 1, 0, 9, 0, 0, 0, 0},
      {3, 0, 0, 4, 0, 0, 2, 0, 0}, {0, 5, 0, 1, 0, 0, 0, 0, 0},
      {0, 0, 0, 8, 0, 6, 0, 0, 0}};

  start = omp_get_wtime();

  // print_grid(sudoku);

#pragma omp parallel default(none) shared(sudoku) num_threads(4)
#pragma omp single nowait
  { solve(sudoku, 1); }
}