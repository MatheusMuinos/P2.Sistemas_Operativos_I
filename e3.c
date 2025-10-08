#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define FILE_NAME "resultados.txt"

// Função para medir tempo em segundos
double tempo_atual() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Função para calcular média de tan(sqrt(i)) de um intervalo
double media_intervalo(int inicio, int fim) {
    double soma = 0.0;
    for (int i = inicio; i < fim; i++) {
        soma += tan(sqrt(i));
    }
    return soma / (fim - inicio);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s N P\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int P = atoi(argv[2]);
    int parte = N / P;
    int resto = N % P;

    FILE *f = fopen(FILE_NAME, "w");
    if (!f) {
        perror("Erro ao abrir arquivo");
        return 1;
    }
    fprintf(f, "# Formato: tipo | processo | média | tempo (s)\n");
    fclose(f);

    // --- 1. Processos filhos para cálculo parcial ---
    for (int i = 0; i < P; i++) {
        if (fork() == 0) {
            int ini = i * parte;
            int fim = (i == P - 1) ? (ini + parte + resto) : (ini + parte);

            double inicio_t = tempo_atual();
            double media = media_intervalo(ini, fim);
            double fim_t = tempo_atual();

            double tempo = fim_t - inicio_t;

            FILE *f2 = fopen(FILE_NAME, "a");
            fprintf(f2, "PARCIAL | %d | %.10f | %.5f\n", i, media, tempo);
            fclose(f2);

            printf("Hijo %d: intervalo [%d,%d), media=%.10f, tempo=%.3fs\n", 
                    i, ini, fim, media, tempo);
            exit(0);
        }
    }

    // Esperar os P filhos
    for (int i = 0; i < P; i++) wait(NULL);

    // --- 2. Filho P+1: lê as parciais e calcula a média geral ---
    if (fork() == 0) {
        double soma = 0.0;
        int count = 0;
        double inicio_t = tempo_atual();

        FILE *f3 = fopen(FILE_NAME, "r");
        char tipo[20];
        int id;
        double media, tempo;

        while (fscanf(f3, "%s | %d | %lf | %lf\n", tipo, &id, &media, &tempo) == 4) {
            if (strcmp(tipo, "PARCIAL") == 0) {
                soma += media;
                count++;
            }
        }
        fclose(f3);

        double media_final = soma / count;
        double fim_t = tempo_atual();
        double tempo_total = fim_t - inicio_t;

        FILE *f4 = fopen(FILE_NAME, "a");
        fprintf(f4, "AGREGADO | %d | %.10f | %.5f\n", getpid(), media_final, tempo_total);
        fclose(f4);

        printf("Hijo P+1 (agregador): media geral = %.10f, tempo=%.3fs\n", media_final, tempo_total);
        exit(0);
    }

    // --- 3. Filho P+2: cálculo sequencial completo ---
    if (fork() == 0) {
        double inicio_t = tempo_atual();
        double media_total = media_intervalo(0, N);
        double fim_t = tempo_atual();
        double tempo_total = fim_t - inicio_t;

        FILE *f5 = fopen(FILE_NAME, "a");
        fprintf(f5, "COMPLETO | %d | %.10f | %.5f\n", getpid(), media_total, tempo_total);
        fclose(f5);

        printf("Hijo P+2 (completo): media total = %.10f, tempo=%.3fs\n", media_total, tempo_total);
        exit(0);
    }

    // Esperar os dois filhos extras
    for (int i = 0; i < 2; i++) wait(NULL);

    // --- 4. Pai lê resultados e compara ---
    FILE *f6 = fopen(FILE_NAME, "r");
    if (!f6) {
        perror("Erro ao reabrir arquivo");
        return 1;
    }

    char tipo[20];
    int id;
    double media, tempo;
    double media_agregada = 0.0, media_completa = 0.0;
    double tempo_agregado = 0.0, tempo_completo = 0.0;

    while (fscanf(f6, "%s | %d | %lf | %lf\n", tipo, &id, &media, &tempo) == 4) {
        if (strcmp(tipo, "AGREGADO") == 0) {
            media_agregada = media;
            tempo_agregado = tempo;
        } else if (strcmp(tipo, "COMPLETO") == 0) {
            media_completa = media;
            tempo_completo = tempo;
        }
    }
    fclose(f6);

    printf("\n=== RESULTADOS FINAIS ===\n");
    printf("Média paralela (agregada): %.10f\n", media_agregada);
    printf("Média sequencial (completa): %.10f\n", media_completa);
    printf("Diferença: %.10e\n", fabs(media_agregada - media_completa));
    printf("Tempo agregador: %.3fs\n", tempo_agregado);
    printf("Tempo completo: %.3fs\n", tempo_completo);

    return 0;
}
