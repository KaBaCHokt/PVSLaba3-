#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 316
#define COLS 316
#define RUNS 100

void run_calculation(int run_number, double* total_fill, double* total_calc, double* total_time) {
    double (*a)[COLS] = malloc(ROWS * sizeof(*a));
    double (*b)[COLS] = malloc(ROWS * sizeof(*b));
    double total_sum = 0.0;

    // Замер времени заполнения
    clock_t fill_start = clock();
    srand(time(NULL) + run_number);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            a[i][j] = (double)rand() / RAND_MAX * 100.0;
            b[i][j] = (double)rand() / RAND_MAX * 100.0 + 0.1;
        }
    }
    clock_t fill_end = clock();
    double fill_time = (double)(fill_end - fill_start) / CLOCKS_PER_SEC;

    // Замер времени вычислений
    clock_t calc_start = clock();
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            total_sum += a[i][j] + b[i][j];
        }
    }
    clock_t calc_end = clock();
    double calc_time = (double)(calc_end - calc_start) / CLOCKS_PER_SEC;
    double run_time = fill_time + calc_time;

    printf("Run %d\n", run_number);
    printf("Sum of elements: %.0f\n", total_sum);
    printf("Filling time: %.6f sec\n", fill_time);
    printf("Calculation time: %.6f sec\n", calc_time);
    printf("Total time: %.6f sec\n\n", run_time);

    *total_fill += fill_time;
    *total_calc += calc_time;
    *total_time += run_time;

    free(a);
    free(b);
}

int main() {
    printf("Enter the number of array elements: %d\n", ROWS * COLS);

    double total_fill = 0, total_calc = 0, total_time = 0;

    for (int i = 1; i <= RUNS; i++) {
        run_calculation(i, &total_fill, &total_calc, &total_time);

        if (i == 4) {
            printf("---\n**POCKET CITIES TO DISCOVER SEC**\n---\n");
        }
    }

    printf("=== FINAL AVERAGE VALUES (from %d runs) ===\n", RUNS);
    printf("Average filling time: %.6f sec\n", total_fill / RUNS);
    printf("Average calculation time: %.6f sec\n", total_calc / RUNS);
    printf("Average total time: %.6f sec\n", total_time / RUNS);

    return 0;
}

