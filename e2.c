#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t hijo1, hijo2;
    int estado;

    // Hijo1 (zombie)
    hijo1 = fork();
    if (hijo1 == 0) {
        printf("Hijo1 (PID=%d) termina pronto\n", getpid());
        sleep(1);
        exit(5);
    }

    // Hijo2
    hijo2 = fork();
    if (hijo2 == 0) {
        printf("Hijo2 (PID=%d) ejecutándose y hará execvp\n", getpid());
        fflush(stdout);

        // Ejecutamos ls inmediatamente
        char *args[] = {"ls", NULL};
        execvp("ls", args);

        // Si falla execvp
        printf("Si ves este texto, execvp falló\n");
        exit(10);
    }

    // Padre
    printf("Padre (PID=%d) esperando al primer hijo...\n", getpid());
    pid_t hijo_terminado = wait(&estado);
    if (WIFEXITED(estado)) {
        printf("Padre: hijo con PID %d terminó, exit=%d\n",
               hijo_terminado, WEXITSTATUS(estado));
    }

    printf("Padre (PID=%d) termina\n", getpid());

    // Ya no hacemos sleep largo → la salida de hijo2 se verá
    return 0;
}
