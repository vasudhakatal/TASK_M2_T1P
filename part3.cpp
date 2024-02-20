#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>  // Include OpenMP header

using namespace std;
using namespace chrono;

auto start_time = high_resolution_clock::now();  // Start measuring time
const int MAX_SIZE = 100;  // Maximum matrix size

void initializeMatrix(int matrix[MAX_SIZE][MAX_SIZE], int size) {
    #pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = rand() % 10;  // Initialize with random values
        }
    }
}

void multiplyMatrices(int A[MAX_SIZE][MAX_SIZE], int B[MAX_SIZE][MAX_SIZE], int C[MAX_SIZE][MAX_SIZE], int size) {
    #pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            C[i][j] = 0;
            #pragma omp parallel for reduction(+:C[i][j])
            for (int k = 0; k < size; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void printMatrix(int matrix[MAX_SIZE][MAX_SIZE], int size, const char* filename) {
    ofstream outputFile(filename);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            outputFile << matrix[i][j] << " ";
        }
        outputFile << endl;
    }
    outputFile.close();
}

int main() {
    srand(time(0));  // Seed for random number generation

    // Generate a random matrix size between 2 and MAX_SIZE
    int N = rand() % (MAX_SIZE - 1) + 2 ;

    int A[MAX_SIZE][MAX_SIZE];
    int B[MAX_SIZE][MAX_SIZE];
    int C[MAX_SIZE][MAX_SIZE];

    initializeMatrix(A, N);
    initializeMatrix(B, N);

    multiplyMatrices(A, B, C, N);

    auto end_time = high_resolution_clock::now();  // Stop measuring time
    auto duration = duration_cast<microseconds>(end_time - start_time);

    // Print execution time
    cout << "Execution Time: " << duration.count() << " microseconds" << endl;

    // Print matrices to files
    printMatrix(A, N, "matrix_A.txt");
    printMatrix(B, N, "matrix_B.txt");
    printMatrix(C, N, "matrix_C.txt");

    cout << "Matrices A, B, and C with size " << N << " have been generated." << endl;

    return 0;
}
