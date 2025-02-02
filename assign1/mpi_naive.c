#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

double **allocate_matrix(int rows, int cols) {
    double **matrix = (double **)malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double *)malloc(cols * sizeof(double));
    }
    return matrix;
}

double *allocate_vector(int size) {
    return (double *)malloc(size * sizeof(double));
}

void generate_random_matrix(double **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (double)rand() / RAND_MAX;
        }
    }
}

void generate_random_vector(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = (double)rand() / RAND_MAX;
    }
}

void matrix_vector_multiply(double **matrix, double *vector, double *result, int rows, int cols, int rank, int num_procs) {
    int start_row = rank * rows / num_procs;
    int end_row = (rank + 1) * rows / num_procs;

    for (int i = start_row; i < end_row; i++) {
        result[i] = 0.0;
        for (int j = 0; j < cols; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }

    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, result, rows / num_procs, MPI_DOUBLE, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (argc != 3) {
        if (rank == 0) {
            printf("Usage: %s <matrix size> <vector size>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    double **matrix = allocate_matrix(rows, cols);
    double *vector = allocate_vector(cols);
    double *result = allocate_vector(rows);

    srand(time(NULL));
    generate_random_matrix(matrix, rows, cols);
    generate_random_vector(vector, cols);

    matrix_vector_multiply(matrix, vector, result, rows, cols, rank, num_procs);

    if (rank == 0) {
        printf("Result:\n");
        for (int i = 0; i < rows; i++) {
            printf("%.2f ", result[i]);
        }
        printf("\n");
    }

    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(vector);
    free(result);

    MPI_Finalize();
    return 0;
}
