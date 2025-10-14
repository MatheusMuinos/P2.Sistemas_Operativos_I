#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int global = 42;

int main() {
    int local = 100;
    int *dinamica = malloc(sizeof(int));
    *dinamica = 200;
    char buffer[100];

    // Abrimos un fichero antes del fork
    FILE *f = fopen("compartido.txt", "w+");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    printf("\nProceso %s\n", pid == 0 ? "HIJO" : "PADRE");
    printf("PID: %d\n\n", getpid());
    printf("Dirección de 'global':   %p, valor: %d\n", (void*)&global, global);
    printf("Dirección de 'local':    %p, valor: %d\n", (void*)&local, local);
    printf("Dirección de 'dinamica': %p, valor: %d\n", (void*)dinamica, *dinamica);

    // Ambos procesos escriben en el fichero compartido
    fprintf(f, "[%s] PID: %d\n", pid == 0 ? "HIJO" : "PADRE", getpid());
    fflush(f);

    // Ambos procesos intentan leer del dispositivo de entrada
    printf("[%s] ANTES de scanf, esperando entrada...\n", pid == 0 ? "HIJO" : "PADRE");
    printf("[%s] Introduce una palabra para comprobar competencia en stdin: ", pid == 0 ? "HIJO" : "PADRE");
    scanf("%99s", buffer);
    printf("[%s] DESPUÉS de scanf, leí: %s\n", pid == 0 ? "HIJO" : "PADRE", buffer);

    // Modificamos los valores en el hijo
    if (pid == 0) {
        global = 999;
        local = 888;
        *dinamica = 777;
        printf("\n---------HIJO modifica las variables---------\n");
        printf("Dirección de 'global':   %p, valor: %d\n", (void*)&global, global);
        printf("Dirección de 'local':    %p, valor: %d\n", (void*)&local, local);
        printf("Dirección de 'dinamica': %p, valor: %d\n", (void*)dinamica, *dinamica);
    } else {
        int status;
        wait(&status); // Espera a que el hijo termine y obtiene el código de salida
        printf("\n---------PADRE después de modificación en hijo---------\n");
        printf("Dirección de 'global':   %p, valor: %d\n", (void*)&global, global);
        printf("Dirección de 'local':    %p, valor: %d\n", (void*)&local, local);
        printf("Dirección de 'dinamica': %p, valor: %d\n", (void*)dinamica, *dinamica);
    }

    fclose(f);
    free(dinamica);
    return 0;
    
}