#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <pthread.h>

using namespace std;
using namespace chrono;

const int MAX_SIZE = 100;  // Maximum matrix size
const int MAX_THREADS = 4;  // Maximum number of threads

int A[MAX_SIZE][MAX_SIZE];
int B[MAX_SIZE][MAX_SIZE];
int C[MAX_SIZE][MAX_SIZE];
int N;

struct ThreadData {
    int startRow;
    int endRow;
};

// Function to initialize a matrix with random values
void initializeMatrix(int matrix[MAX_SIZE][MAX_SIZE], int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = rand() % 10;  // Initialize with random values
        }
    }
}

// Function to multiply matrices for a specific range of rows
void* multiplyMatricesParallel(void* data) {
    ThreadData* td = static_cast<ThreadData*>(data);

    for (int i = td->startRow; i < td->endRow; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(0));  // Seed for random number generation

    // Vary the matrix sizes
    for (N = 2; N <= MAX_SIZE; N += 10) {
        auto start_time = high_resolution_clock::now();  // Start measuring time

        // Initialize matrices A, B
        initializeMatrix(A, N);
        initializeMatrix(B, N);

        // Create threads data
        pthread_t threads[MAX_THREADS];
        ThreadData threadData[MAX_THREADS];

        // Vary the number of threads
        for (int numThreads = 2; numThreads <= MAX_THREADS; ++numThreads) {
            // Calculate the number of rows each thread will handle
            int rowsPerThread = N / numThreads;
            int remainingRows = N % numThreads;

            // Create threads
            for (int i = 0; i < numThreads; ++i) {
                threadData[i].startRow = i * rowsPerThread;
                threadData[i].endRow = (i + 1) * rowsPerThread;
                if (i == numThreads - 1) {
                    // Distribute remaining rows to the last thread
                    threadData[i].endRow += remainingRows;
                }
                pthread_create(&threads[i], NULL, multiplyMatricesParallel, &threadData[i]);
            }

            // Join threads
            for (int i = 0; i < numThreads; ++i) {
                pthread_join(threads[i], NULL);
            }
            
            auto end_time = high_resolution_clock::now();  // Stop measuring time
            auto duration = duration_cast<microseconds>(end_time - start_time);

            // Print execution time and details
            cout << "Matrix Size: " << N << " x " << N << ", Threads: " << numThreads << ", Execution Time: " << duration.count() << " microseconds" << endl;
        }
    }

    return 0;
}
