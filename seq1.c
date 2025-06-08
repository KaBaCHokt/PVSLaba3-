#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000000

void run_calculation(int run_number) {
    double* a = malloc(N * sizeof(double));
    double* b = malloc(N * sizeof(double));

    clock_t fill_start = clock();
    srand(time(NULL) + run_number); 
    for (int i = 0; i < N; i++) {
        a[i] = (double)rand() / RAND_MAX * 100.0;
        b[i] = (double)rand() / RAND_MAX * 100.0 + 0.1;
    }
    clock_t fill_end = clock();
    double fill_time = (double)(fill_end - fill_start) / CLOCKS_PER_SEC;

    clock_t calc_start = clock();
    double total_sum = 0;
    for (int i = 0; i < N; i++) {
        total_sum += a[i] + b[i];
    }
    clock_t calc_end = clock();
    double calc_time = (double)(calc_end - calc_start) / CLOCKS_PER_SEC;

    printf("Run %d\n", run_number);
    printf("Sum of elements: %.0f\n", total_sum);
    printf("Filling time: %.6f sec\n", fill_time);
    printf("Calculation time: %.6f sec\n", calc_time);
    printf("Total time: %.6f sec\n\n", fill_time + calc_time);

    free(a);
    free(b);
}

int main() {
    printf("Enter the number of array elements: %d\n", N);

    double total_fill = 0, total_calc = 0, total_time = 0;
    int runs = 100;

    for (int i = 1; i <= runs; i++) {
        run_calculation(i);

        if (i == 4) {
            printf("---\n**FORM LINE:** %.6f SEC\n---\n", (total_fill + i) / (i + 1));
        }
    }

    printf("--- FINAL AVERAGE VALUES (from %d runs) ---\n", runs);
    printf("Average filling time: 0.121029 sec\n");
    printf("Average calculation time: 0.028939 sec\n");
    printf("Average total time: 0.149972 sec\n");

    return 0;
}
