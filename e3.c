#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>

double promedio(int inicio, int fin) {
    double suma = 0;
    for (int i = inicio; i < fin; i++) {
        suma += tan(sqrt(i));
    }
    return suma / (fin - inicio);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s N P\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]); // cantidad de numeros
    int P = atoi(argv[2]); // cantidad de procesos

    int parte = N / P;
    int resto = N % P;

    for (int i = 0; i < P; i++) {
        if (fork() == 0) {
            int ini = i * parte;
            int fin = (i+1) * parte;
            if (i == P-1) fin += resto; // el ultimo hace el resto
            double prom = promedio(ini, fin);
            printf("HIJO %d: promedio=%.3f\n", i, prom);
            exit(0);
        }
    }

    // Padre espera todos
    for (int i = 0; i < P; i++) wait(NULL);

    // Calculo secuencial para comparar
    double promTotal = promedio(0, N);
    printf("PADRE: promedio secuencial=%.3f\n", promTotal);

    return 0;
}
