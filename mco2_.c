#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

extern void asm_kernel(double* X1, double* X2, double* Y1, double* Y2, double* Z, int n);

void c_kernel(int n, double* X1, double* X2, double* Y1, double* Y2, double* Z) {
    for (int i = 0; i < n; i++) {
        Z[i] = sqrt(pow(X2[i] - X1[i], 2) + pow(Y2[i] - Y1[i], 2));
    }
}

long long get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

void print_divider() {
    printf("\n-------------------------------------------------\n");
}

int main() {
    int n = 4;
    int count = n;
    double X1[] = {1.5, 4.0, 3.5, 2.0};
    double X2[] = {3.0, 2.5, 2.5, 1.0};
    double Y1[] = {4.0, 3.0, 3.5, 3.0};
    double Y2[] = {2.0, 2.5, 1.0, 1.5};
    double* Z = (double*)malloc(n * sizeof(double));
    double* Z_asm = (double*)malloc(n * sizeof(double));
    int num_runs = 30;
    int correct = 1;

    printf("Testing with n = %zu\n", n);
    
    //Getting avg execution time of C Kernel 
    long long c_total_time_ns = 0;
    for (int i = 0; i < num_runs; i++) {
        long long start_time = get_time_ns();
        c_kernel(n, X1, X2, Y1, Y2, Z);
        long long end_time = get_time_ns();
        c_total_time_ns += (end_time - start_time);
    }
    long long c_avg_time_ns = c_total_time_ns / num_runs;

    //C Kernel Results
    if(n > 10) count = 10;
    printf("C Result:\nZ -> ");
    for (int i = 0; i < count ; i++) {
        printf("%.9lf ", Z[i]);
    }
    printf("\n");

    //Getting avg execution time of x86-64 Kernel
    long long asm_total_time_ns = 0;
    for (int i = 0; i < num_runs; i++) {
        long long start_time = get_time_ns();
        asm_kernel(X1, X2, Y1, Y2, Z_asm, n);
        long long end_time = get_time_ns();
        asm_total_time_ns += (end_time - start_time);
    }
    long long asm_avg_time_ns = asm_total_time_ns / num_runs;

    //x86-64 Kernel Results
    printf("Assembly Result:\nZ -> ");
    for (int i = 0; i < count; i++) {
        printf("%.9lf ", Z_asm[i]);
    }
    printf("\n");

    printf("\nAverage execution time (C kernel): %lld ns\n", c_avg_time_ns);
    printf("Average execution time (Assembly kernel): %lld ns\n", asm_avg_time_ns);
    
    //Check if correct (C version is the "sanity check answer key")
    double epsilon = 1e-9;
    for (int i = 0; i < n; i++) {
        if (fabs(Z[i] - Z_asm[i]) > epsilon) {
            printf("\nMismatch at index %d: C = %.9lf, ASM = %.9lf\n", i, Z[i], Z_asm[i]);
            correct = 0;
            break;
        }
    }    
    
    if (correct) {printf("\nThe x86-64 kernel output is correct.\n");}
    else {printf("\nThe x86-64 kernel output is incorrect.\n");}

    free(Z);
    free(Z_asm);

    //TESTS WITH n = [2^20, 2^24, and 2^28]
    int num_sizes = 5; //# of tests
    size_t sizes[] = {1 << 20, 1 << 24, 1 << 25, 1 << 26, 1 << 27};

    for (int s = 0; s < num_sizes; s++) {

        print_divider();
        
        size_t n = sizes[s];
        printf("Testing with n = %zu\n", n);

        double* X1 = (double*)malloc(n * sizeof(double));
        double* X2 = (double*)malloc(n * sizeof(double));
        double* Y1 = (double*)malloc(n * sizeof(double));
        double* Y2 = (double*)malloc(n * sizeof(double));
        double* Z = (double*)malloc(n * sizeof(double));
        double* Z_asm = (double*)malloc(n * sizeof(double));

        if (!X1 || !X2 || !Y1 || !Y2 || !Z || !Z_asm) {
            fprintf(stderr, "Memory allocation failed for size n = %zu\n", n);
            exit(EXIT_FAILURE);
        }

        //initialize input arrays with random values
        for (size_t i = 0; i < n; i++) {
            X1[i] = rand() / (double)RAND_MAX;
            X2[i] = rand() / (double)RAND_MAX;
            Y1[i] = rand() / (double)RAND_MAX;
            Y2[i] = rand() / (double)RAND_MAX;
        }

        long long c_total_time_ns = 0;
        for (int i = 0; i < num_runs; i++) {
            long long start_time = get_time_ns();
            c_kernel(n, X1, X2, Y1, Y2, Z);
            long long end_time = get_time_ns();
            c_total_time_ns += (end_time - start_time);
        }
        long long c_avg_time_ns = c_total_time_ns / num_runs;

        long long asm_total_time_ns = 0;
        for (int i = 0; i < num_runs; i++) {
            long long start_time = get_time_ns();
            asm_kernel(X1, X2, Y1, Y2, Z_asm, n);
            long long end_time = get_time_ns();
            asm_total_time_ns += (end_time - start_time);
        }
        long long asm_avg_time_ns = asm_total_time_ns / num_runs;

        printf("C Result:\nZ -> ");
        for (int i = 0; i < 10 ; i++) {
            printf("%.9lf ", Z[i]);
        }
        printf("\n");

        printf("Assembly Result:\nZ -> ");
        for (int i = 0; i < 10; i++) {
            printf("%.9lf ", Z_asm[i]);
        }
        printf("\n");

        printf("\nAverage execution times:\n");
        printf("C kernel: %lld ns (%.9f s)\nAssembly kernel: %lld ns (%.9f s)\n",
       c_avg_time_ns, c_avg_time_ns / 1e9, asm_avg_time_ns, asm_avg_time_ns / 1e9);


        //correctness check for n=[2^20, 2^24, 2^30]
        correct = 1;
        for (int i = 0; i < n; i++) {
            if (fabs(Z[i] - Z_asm[i]) > epsilon) {
                printf("\nMismatch at index %d: C = %.9lf, ASM = %.9lf\n", i, Z[i], Z_asm[i]);
                correct = 0;
                break;
            }
        }    
        
        if (correct) {printf("\nThe x86-64 kernel output is correct.\n");}

        //free allocated memory
        free(X1);
        free(X2);
        free(Y1);
        free(Y2);
        free(Z);
        free(Z_asm);
    }
    

    return 0;
}