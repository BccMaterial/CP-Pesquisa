#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long fibonacci_seq(int n) {
    if (n <= 1) {
        return n;
    }

    return fibonacci_seq(n - 1) + fibonacci_seq(n - 2);
}

int main(int argc, char *argv[]) {
    int n = 40;

    if (argc > 1) {
        n = atoi(argv[1]);
    }

    clock_t inicio = clock();

    long long resultado = fibonacci_seq(n);

    clock_t fim = clock();

    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("Fibonacci sequencial de %d = %lld\n", n, resultado);
    printf("Tempo sequencial: %.6f segundos\n", tempo);

    return 0;
}