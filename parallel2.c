#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define ROWS 316
#define COLS 316
#define RUNS 100

int main(int argc, char** argv) {
    int rank, size;
    double (*a)[COLS] = NULL, (*b)[COLS] = NULL;
    double (*sum)[COLS] = NULL, (*diff)[COLS] = NULL;
    double (*prod)[COLS] = NULL, (*quot)[COLS] = NULL;
    double (*local_a)[COLS], (*local_b)[COLS];
    double (*local_sum)[COLS], (*local_diff)[COLS];
    double (*local_prod)[COLS], (*local_quot)[COLS];

    double total_fill_time = 0.0;
    double total_calc_time = 0.0;
    double total_time = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_rows = ROWS / size;

    // Выделение памяти
    local_a = (double (*)[COLS])malloc(local_rows * sizeof(*local_a));
    local_b = (double (*)[COLS])malloc(local_rows * sizeof(*local_b));
    local_sum = (double (*)[COLS])malloc(local_rows * sizeof(*local_sum));
    local_diff = (double (*)[COLS])malloc(local_rows * sizeof(*local_diff));
    local_prod = (double (*)[COLS])malloc(local_rows * sizeof(*local_prod));
    local_quot = (double (*)[COLS])malloc(local_rows * sizeof(*local_quot));

    if (rank == 0) {
        a = (double (*)[COLS])malloc(ROWS * sizeof(*a));
        b = (double (*)[COLS])malloc(ROWS * sizeof(*b));
        sum = (double (*)[COLS])malloc(ROWS * sizeof(*sum));
        diff = (double (*)[COLS])malloc(ROWS * sizeof(*diff));
        prod = (double (*)[COLS])malloc(ROWS * sizeof(*prod));
        quot = (double (*)[COLS])malloc(ROWS * sizeof(*quot));
    }

    for (int run = 1; run <= RUNS; run++) {
        double fill_start, fill_end, calc_start, calc_end;

        if (rank == 0) {
            fill_start = MPI_Wtime();
            srand((unsigned int)time(NULL) + run);
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    a[i][j] = (double)rand() / RAND_MAX * 100.0;
                    b[i][j] = (double)rand() / RAND_MAX * 100.0 + 0.1;
                }
            }
            fill_end = MPI_Wtime();
        }

        MPI_Barrier(MPI_COMM_WORLD);
        calc_start = MPI_Wtime();

        // Распределение данных
        MPI_Scatter(a, local_rows * COLS, MPI_DOUBLE, local_a, local_rows * COLS, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(b, local_rows * COLS, MPI_DOUBLE, local_b, local_rows * COLS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Локальные вычисления
        for (int i = 0; i < local_rows; i++) {
            for (int j = 0; j < COLS; j++) {
                local_sum[i][j] = local_a[i][j] + local_b[i][j];
                local_diff[i][j] = local_a[i][j] - local_b[i][j];
                local_prod[i][j] = local_a[i][j] * local_b[i][j];
                local_quot[i][j] = local_a[i][j] / local_b[i][j];
            }
        }

        // Сбор данных
        MPI_Gather(local_sum, local_rows * COLS, MPI_DOUBLE, sum, local_rows * COLS, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        calc_end = MPI_Wtime();

        if (rank == 0) {
            double fill_time = fill_end - fill_start;
            double calc_time = calc_end - calc_start;
            double run_time = fill_time + calc_time;

            total_fill_time += fill_time;
            total_calc_time += calc_time;
            total_time += run_time;

            printf("Run %d\n", run);
            printf("Sum of elements: %.0f\n", sum[0][0]); // Пример вывода суммы первого элемента
            printf("filling time: %.6f sec\n", fill_time);
            printf("calculation time: %.6f sec\n", calc_time);
            printf("Total time: %.6f sec\n\n", run_time);
        }
    }

    if (rank == 0) {
        printf("--- FINAL AVERAGE VALUES (from %d runs) ---\n", RUNS);
        printf("Average filling time: %.6f sec\n", total_fill_time / RUNS);
        printf("Average calculation time: %.6f sec\n", total_calc_time / RUNS);
        printf("Average total time: %.6f sec\n", total_time / RUNS);

        free(a);
        free(b);
        free(sum);
        free(diff);
        free(prod);
        free(quot);
    }

    free(local_a);
    free(local_b);
    free(local_sum);
    free(local_diff);
    free(local_prod);
    free(local_quot);

    MPI_Finalize();
    return 0;
}