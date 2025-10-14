//Codigo de Matheus Muiños y Anton Jaureguizar

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define FILE_NAME "resultados.txt"

// Tiempo actual en segundos (monotónico)
double tempo_atual() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Media de tan(sqrt(i)) en [inicio, fim)
double media_intervalo(int inicio, int fim) {
    double soma = 0.0;
    for (int i = inicio; i < fim; i++) {
        soma += tan(sqrt(i));
    }
    return soma / (fim - inicio);
}

int main(int argc, char *argv[]) {
    // Verifica argumentos
    if (argc != 3) {
        printf("Uso: %s N P\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);   // Total de elementos
    int P = atoi(argv[2]);   // Nº de procesos parciales
    int parte = N / P;       // Tamaño base de cada bloque
    int resto = N % P;       // Resto para el último

    // Inicializa archivo de resultados
    FILE *f = fopen(FILE_NAME, "w");
    if (!f) {
        perror("Erro ao abrir arquivo");
        return 1;
    }
    fprintf(f, "# Formato: tipo | proceso | média | tempo (s) | tamanho\n");
    fclose(f);

    // 1) Lanzar P hijos para intervalos parciales
    pid_t filhos[P];
    for (int i = 0; i < P; i++) {
        filhos[i] = fork();
        if (filhos[i] == 0) {
            int ini = i * parte;
            int fim = (i == P - 1) ? (ini + parte + resto) : (ini + parte);

            double inicio_t = tempo_atual();
            double media = media_intervalo(ini, fim);
            double fim_t = tempo_atual();
            double tempo = fim_t - inicio_t;

            FILE *f2 = fopen(FILE_NAME, "a"); // Append resultado parcial
            if (!f2) {
                perror("Erro ao abrir arquivo");
                exit(1);
            }
            fprintf(f2, "PARCIAL | %d | %.10f | %.5f | %d\n", i, media, tempo, fim - ini);
            fclose(f2);

            printf("Hijo %d: intervalo [%d,%d), media=%.10f, tempo=%.3fs\n",
                   i, ini, fim, media, tempo);
            exit(0); // Termina hijo
        }
    }

    // Espera todos los hijos parciales
    for (int i = 0; i < P; i++) waitpid(filhos[i], NULL, 0);

    // 2) Hijo agregador: calcula media global a partir de parciales
    pid_t pid_agregador = fork();
    if (pid_agregador == 0) {
        double soma = 0.0;
        int total = 0;
        double inicio_t = tempo_atual();

        FILE *f3 = fopen(FILE_NAME, "r");
        if (!f3) {
            perror("Erro ao abrir arquivo");
            exit(1);
        }
        // Ignora encabezado y parsea solo líneas que comienzan con "PARCIAL"
        char linha[256];
        int id, tam;
        double media, tempo;
        while (fgets(linha, sizeof(linha), f3)) {
            if (strncmp(linha, "PARCIAL", 7) == 0) {
                if (sscanf(linha, "PARCIAL | %d | %lf | %lf | %d", &id, &media, &tempo, &tam) == 4) {
                    soma += media * tam;
                    total += tam;
                }
            }
        }
        fclose(f3);

        double media_final = (total > 0) ? soma / total : 0.0;
        double fim_t = tempo_atual();
        double tempo_total = fim_t - inicio_t;

        FILE *f4 = fopen(FILE_NAME, "a");
        if (!f4) {
            perror("Erro ao abrir arquivo");
            exit(1);
        }
        fprintf(f4, "AGREGADO | %d | %.10f | %.5f\n", getpid(), media_final, tempo_total);
        fclose(f4);

        printf("Hijo P+1 (agregador): media geral = %.10f, tempo=%.3fs\n", media_final, tempo_total);
        exit(0);
    }

    // 3) Hijo secuencial: calcula media completa directa
    pid_t pid_completo = fork();
    if (pid_completo == 0) {
        double inicio_t = tempo_atual();
        double media_total = media_intervalo(0, N);
        double fim_t = tempo_atual();
        double tempo_total = fim_t - inicio_t;

        FILE *f5 = fopen(FILE_NAME, "a");
        if (!f5) {
            perror("Erro ao abrir arquivo");
            exit(1);
        }
        fprintf(f5, "COMPLETO | %d | %.10f | %.5f\n", getpid(), media_total, tempo_total);
        fclose(f5);

        printf("Hijo P+2 (completo): media total = %.10f, tempo=%.3fs\n", media_total, tempo_total);
        exit(0);
    }

    // Espera agregador y completo
    waitpid(pid_agregador, NULL, 0);
    waitpid(pid_completo, NULL, 0);

    // 4) Padre lee resultados finales y compara
    FILE *f6 = fopen(FILE_NAME, "r");
    if (!f6) {
        perror("Erro ao reabrir arquivo");
        return 1;
    }

    int id;
    double media, tempo;
    double media_agregada = 0.0, media_completa = 0.0;
    double tempo_agregado = 0.0, tempo_completo = 0.0;

    // Extrae líneas AGREGADO y COMPLETO (ignorando encabezado)
    char linha2[256];
    while (fgets(linha2, sizeof(linha2), f6)) {
        if (strncmp(linha2, "AGREGADO", 8) == 0) {
            if (sscanf(linha2, "AGREGADO | %d | %lf | %lf", &id, &media, &tempo) == 3) {
                media_agregada = media;
                tempo_agregado = tempo;
            }
        } else if (strncmp(linha2, "COMPLETO", 8) == 0) {
            if (sscanf(linha2, "COMPLETO | %d | %lf | %lf", &id, &media, &tempo) == 3) {
                media_completa = media;
                tempo_completo = tempo;
            }
        }
    }
    fclose(f6);

    // Salida resumen
    printf("\n=== RESULTADOS FINAIS ===\n");
    printf("Média paralela (agregada): %.10f\n", media_agregada);
    printf("Média sequencial (completa): %.10f\n", media_completa);
    printf("Diferença: %.10e\n", fabs(media_agregada - media_completa));
    printf("Tempo agregador: %.3fs\n", tempo_agregado);
    printf("Tempo completo: %.3fs\n", tempo_completo);

    // gcc -o e3 e3.c -lm
    // ./e3 1000000 4
    // (N=1.000.000, P=4)

    return 0;
}
