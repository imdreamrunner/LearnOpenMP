#include <omp.h>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include "MatUtil.h"

using namespace std;

void calculate_shortest_path(int* mat, const size_t& n) {
    for (int k = 0; k < n; k++) {
        // #pragma omp parallel for schedule(static, 120) collapse(2)
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int i0 = i*n + j;
                int i1 = i*n + k;
                int i2 = k*n + j;
                if (mat[i1] != -1 && mat[i2] != -1) {
                    int sum = mat[i1] + mat[i2];
                    if (mat[i0] == -1 || sum < mat[i0]) {
                        mat[i0] = sum;
                    }
                }
            }
        }
    }
}

unsigned long long time_diff(const struct timeval& tv1, const struct timeval& tv2) {
    return (tv2.tv_sec-tv1.tv_sec)*1000000 + tv2.tv_usec-tv1.tv_usec;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Invalid arguments" << endl;
        exit(1);
    }

    size_t n_thread = atoi(argv[1]);
    size_t size_mat = atoi(argv[2]);
    size_t repeat   = atoi(argv[3]);
    size_t num_nodes = size_mat * size_mat;

    struct timeval start_time, end_time;

    #ifdef DEBUG
    cout << "Calculate mat of " << size_mat << " with " << n_thread << " threads." << endl;
    #endif

    omp_set_dynamic(0);
    omp_set_num_threads(n_thread);
    int* mat = (int*) malloc(sizeof(int) * num_nodes);
    int* ans = (int*) malloc(sizeof(int) * num_nodes);
    GenMatrix(mat, size_mat);

    #ifdef DEBUG
    int* ref = (int*) malloc(sizeof(int) * num_nodes);
    memcpy(ref, mat, sizeof(int) * num_nodes);
    gettimeofday(&start_time, NULL);
    ST_APSP(ref, size_mat);
    gettimeofday(&end_time, NULL);
    int sequential_time = time_diff(start_time, end_time);
    #endif

    int parallel_time = 0;
    for (int t = 0; t < repeat; t++) {
        memcpy(ans, mat,  sizeof(int) * num_nodes);
        gettimeofday(&start_time, NULL);
        if (n_thread > 1) {
            calculate_shortest_path(ans, size_mat);
        } else {
            ST_APSP(ans, size_mat);
        }
        gettimeofday(&end_time, NULL);
        parallel_time += time_diff(start_time, end_time);
    }

    #ifdef DEBUG
    cout << "Sequential time: " << sequential_time << " usec." << endl;
    cout << "parallel time: " << parallel_time << " usec." << endl;
    if (CmpArray(ans, ref, num_nodes)) {
        cout << "Parallel answer is correct!" << endl;
    } else {
        cout << "Parallel answer is wrong!" << endl;
    }
    #else
    cout << n_thread << "\t" << size_mat << "\t" << parallel_time << endl;
    #endif
}
