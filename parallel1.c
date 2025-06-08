#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define N 10000000
#define RUNS 100

int main(int argc, char** argv) {
    int rank, size;
    double* a = NULL, * b = NULL;
    double* local_a, * local_b;
    double total_sum = 0;
    double fill_time, calc_time, total_time;
    double total_fill = 0, total_calc = 0, total_total = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_size = N / size;
    local_a = malloc(local_size * sizeof(double));
    local_b = malloc(local_size * sizeof(double));

    if (rank == 0) {
        printf("Enter the number of array elements: %d\n", N);
        a = malloc(N * sizeof(double));
        b = malloc(N * sizeof(double));
    }

    for (int run = 1; run <= RUNS; run++) {
        // Заполнение массива
        double run_fill_start = MPI_Wtime();
        if (rank == 0) {
            srand(time(NULL) + run);
            for (int i = 0; i < N; i++) {
                a[i] = (double)rand() / RAND_MAX * 100.0;
                b[i] = (double)rand() / RAND_MAX * 100.0 + 0.1;
            }
        }
        double run_fill_end = MPI_Wtime();

        // Распределение данных и вычисления
        double run_calc_start = MPI_Wtime();
        MPI_Scatter(a, local_size, MPI_DOUBLE, local_a, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(b, local_size, MPI_DOUBLE, local_b, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        double local_sum = 0;
        for (int i = 0; i < local_size; i++) {
            local_sum += local_a[i] + local_b[i];
        }

        double global_sum;
        MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        double run_calc_end = MPI_Wtime();

        if (rank == 0) {
            fill_time = run_fill_end - run_fill_start;
            calc_time = run_calc_end - run_calc_start;
            total_time = fill_time + calc_time;

            total_fill += fill_time;
            total_calc += calc_time;
            total_total += total_time;

            if (run <= 4 || run >= 98) {
                printf("Run %d\n", run);
                printf("Sum of elements: %.0f\n", global_sum);
                printf("Filling time: %.6f sec\n", fill_time);
                printf("Calculation time: %.6f sec\n", calc_time);
                printf("Total time: %.6f sec\n\n", total_time);

                if (run == 4) {
                    printf("---\n**FORM LINE:** %.6f SEC\n---\n", total_time);
                }
            }
        }
    }

    if (rank == 0) {
        printf("=== FINAL AVERAGE VALUES (from %d runs) ===\n", RUNS);
        printf("Average filling time: %.6f sec\n", total_fill / RUNS);
        printf("Average calculation time: %.6f sec\n", total_calc / RUNS);
        printf("Average total time: %.6f sec\n", total_total / RUNS);

        free(a);
        free(b);
    }

    free(local_a);
    free(local_b);
    MPI_Finalize();
    return 0;
}

