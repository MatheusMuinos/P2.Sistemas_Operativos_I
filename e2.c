#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t hijo1 = fork();

    if (hijo1 == 0) { // Primer hijo
        printf("HIJO1: termino rapido\n");
        exit(42); // código de salida
    }

    pid_t hijo2 = fork();

    if (hijo2 == 0) { // Segundo hijo
        printf("HIJO2: dormire 5s y luego exec\n");
        sleep(5);
        execlp("echo", "echo", "Hola desde exec!", NULL);
        perror("exec fallo");
        exit(1);
    }

    // Padre espera al primer hijo
    int status;
    waitpid(hijo1, &status, 0);
    printf("PADRE: HIJO1 salio con codigo %d\n", WEXITSTATUS(status));

    printf("PADRE termina, HIJO2 puede volverse huerfano.\n");
    return 0;
}
