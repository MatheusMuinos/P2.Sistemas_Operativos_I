//Codigo de Matheus Muiños y Anton Jaureguizar

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t hijo1, hijo2;
    int estado;

    /* Hijo1 (zombie) - queda información en la tabla de procesos,
    que no se eliminará hasta que se ejecute un wait() en el proceso padre*/
    hijo1 = fork();
    if (hijo1 == 0) {
        printf("Hijo1 (PID=%d) termina pronto\n", getpid());
        sleep(1);
        exit(5);
    }

    // Hijo2
    hijo2 = fork();
    if (hijo2 == 0) {
        sleep(5); // Esperamos para que se ejecuten hijo1 y padre, quedando hijo2 huerfano
        printf("Hijo2 (PID=%d) ejecutándose y hará execvp\n", getpid());
        fflush(stdout);
   

        // Ejecutamos ls inmediatamente
        char *args[] = {"ls", NULL};
        execvp("ls", args); /*Si funciona correctamente, se reemplaza el proceso actual
                            por el de ls y no vuelve a ejecutar las instrucciones siguientes*/

        // Si falla execvp
        printf("Si ves este texto, execvp falló\n");
        exit(10);
    }

    // Padre
    printf("Padre (PID=%d) esperando al primer hijo...\n", getpid());
    pid_t hijo_terminado = wait(&estado);
    if (WIFEXITED(estado)) { //devuelve true si el hijo terminó correctamente
        printf("Padre: hijo con PID %d terminó, exit=%d\n",
               hijo_terminado, WEXITSTATUS(estado)); //devuelve el valor pasado a exit().
    }

    printf("Padre (PID=%d) termina\n", getpid());
    sleep(3); // Esperamos un poco para que el hijo1 pueda terminar
    return 0;
}
