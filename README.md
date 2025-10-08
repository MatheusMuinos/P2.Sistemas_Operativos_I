# Sistemas Operativos I — Práctica 2

Este repositorio contiene tres programas en C (e1.c, e2.c, e3.c) para ilustrar procesos, memoria, concurrencia y temporización en Linux (Ubuntu 24.04).

## Requisitos
- GCC y herramientas básicas (ya disponibles en el contenedor)
- Linux/WSL/Dev Container

## Compilación rápida
```bash
# e1 y e2 no requieren librerías adicionales
gcc -e1.c -o e1
gcc -e2.c -o e2

# e3 usa funciones matemáticas → enlazar con -lm (al final)
gcc e3.c -o e3 -lm
```

## Ejecución rápida
```bash
./e1
./e2
./e3 <N> <P>     # Ej.: ./e3 1000000 4
```

---

## e1.c
Propósito:
- Demostrar fork(), espacios de memoria (global, local, dinámica) y competencia por stdin/archivo abierto antes del fork.

Qué hace:
- Crea un hijo con fork().
- Ambos procesos imprimen PID, direcciones y valores de variables global/local/dinámica.
- Ambos escriben en un fichero abierto antes del fork (compartido.txt).
- Ambos realizan scanf compitiendo por stdin.
- El hijo modifica los valores; el padre espera al hijo y vuelve a imprimir para ver que no le afectan los cambios del hijo.

Cómo ejecutar:
```bash
./e1
```

Resultados esperados:
- Se ven dos bloques de impresión (PADRE/HIJO) con PIDs distintos.
- Direcciones de memoria similares por mapeo, pero espacios independientes; los cambios del hijo no afectan al padre.
- Ambos procesos piden entrada por stdin; la primera palabra la consumirá uno y la siguiente el otro.

Observaciones:
- El uso de scanf en ambos procesos puede bloquear/competir; es parte del objetivo.
- El fichero compartido se abre antes de fork() y ambos procesos escriben en él.

---

## e2.c
Propósito:
- Ilustrar creación de procesos múltiples, finalización, espera de hijos y reemplazo de imagen con execvp.

Qué hace:
- Crea dos hijos.
- Hijo1 imprime y finaliza con exit(5).
- Hijo2 imprime y ejecuta execvp("ls"), reemplazándose por el listado del directorio.
- El padre espera a un hijo y muestra su código de salida, luego termina.

Cómo ejecutar:
```bash
./e2
```

Resultados esperados:
- Mensaje del Hijo1 y fin con código 5 (el padre lo reporta).
- Hijo2 ejecuta ls; su salida aparece aunque el padre termine antes (posible huérfano adoptado por init).

Observaciones:
- No se hace wait explícito de Hijo2; se pretende ilustrar orfandad/exec.

---

## e3.c
Propósito:
- Calcular la media en doble precisión de tan(sqrt(i)) para los primeros N números naturales de dos formas y comparar:
  1) En paralelo con P hijos (bloques) + un hijo agregador (P+1).
  2) En un único hijo que calcula todo (P+2).
- Guardar resultados y tiempos en un fichero auxiliar (resultados.txt).

Qué hace:
- Recibe N y P por línea de comandos.
- Crea P hijos; cada uno calcula un intervalo y escribe una línea “PARCIAL” con media y tiempo en resultados.txt.
- Tras esperar a los P hijos, crea el hijo agregador (P+1) que lee el fichero y calcula la media “AGREGADO”.
- En paralelo a lo anterior, crea el hijo (P+2) que calcula la media completa “COMPLETO”.
- El padre lee “AGREGADO” y “COMPLETO”, muestra la diferencia y los tiempos.

Cómo ejecutar:
```bash
./e3 1000000 4
```

Resultados esperados:
- P líneas de hijos parciales con intervalos y tiempos.
- Una línea del agregador con media agregada.
- Una línea del cálculo completo.
- El padre imprime la diferencia entre ambas medias y los tiempos.

Observaciones importantes:
- Compilar con -lm (al final): gcc ... e3.c -o e3 -lm.
- El fichero empieza con una cabecera (“# Formato ...”). El agregador usa fscanf con un formato estricto y dejará de leer en cuanto encuentre esa primera línea, resultando en división por cero. Soluciones:
  - Eliminar la cabecera inicial, o
  - Cambiar la lectura a fgets + sscanf y saltar líneas que no empaten, o
  - Leer con fscanf ignorando la cabecera.
- El agregador promedia medias parciales por igual. Si N%P != 0, los bloques no tienen el mismo tamaño y eso sesga el resultado. Para exactitud, ponderar por el número de elementos de cada bloque.
- “Primeros N naturales”: en el código se empieza en i=0. Si se desea iniciar en 1, ajustar los bucles.
- Escrituras concurrentes en resultados.txt no usan bloqueo; en práctica suelen ser líneas cortas y funcionan, pero para total robustez usar bloqueo de fichero (fcntl) o escribir desde el padre.

---
```<!-- filepath: /workspaces/P2.Sistemas-Operativos-I/README.md -->
# Sistemas Operativos I — Práctica 2

Este repositorio contiene tres programas en C (e1.c, e2.c, e3.c) para ilustrar procesos, memoria, concurrencia y temporización en Linux (Ubuntu 24.04).

## Requisitos
- GCC y herramientas básicas (ya disponibles en el contenedor)
- Linux/WSL/Dev Container

## Compilación rápida
```bash
# e1 y e2 no requieren librerías adicionales
gcc -O2 -Wall -Wextra -pedantic e1.c -o e1
gcc -O2 -Wall -Wextra -pedantic e2.c -o e2

# e3 usa funciones matemáticas → enlazar con -lm (al final)
gcc -O2 -Wall -Wextra -pedantic e3.c -o e3 -lm
```

## Ejecución rápida
```bash
./e1
./e2
./e3 <N> <P>     # Ej.: ./e3 1000000 4
```

---

## e1.c
Propósito:
- Demostrar fork(), espacios de memoria (global, local, dinámica) y competencia por stdin/archivo abierto antes del fork.

Qué hace:
- Crea un hijo con fork().
- Ambos procesos imprimen PID, direcciones y valores de variables global/local/dinámica.
- Ambos escriben en un fichero abierto antes del fork (compartido.txt).
- Ambos realizan scanf compitiendo por stdin.
- El hijo modifica los valores; el padre espera al hijo y vuelve a imprimir para ver que no le afectan los cambios del hijo.

Cómo ejecutar:
```bash
./e1
```

Resultados esperados:
- Se ven dos bloques de impresión (PADRE/HIJO) con PIDs distintos.
- Direcciones de memoria similares por mapeo, pero espacios independientes; los cambios del hijo no afectan al padre.
- Ambos procesos piden entrada por stdin; la primera palabra la consumirá uno y la siguiente el otro.

Observaciones:
- El uso de scanf en ambos procesos puede bloquear/competir; es parte del objetivo.
- El fichero compartido se abre antes de fork() y ambos procesos escriben en él.

---

## e2.c
Propósito:
- Ilustrar creación de procesos múltiples, finalización, espera de hijos y reemplazo de imagen con execvp.

Qué hace:
- Crea dos hijos.
- Hijo1 imprime y finaliza con exit(5).
- Hijo2 imprime y ejecuta execvp("ls"), reemplazándose por el listado del directorio.
- El padre espera a un hijo y muestra su código de salida, luego termina.

Cómo ejecutar:
```bash
./e2
```

Resultados esperados:
- Mensaje del Hijo1 y fin con código 5 (el padre lo reporta).
- Hijo2 ejecuta ls; su salida aparece aunque el padre termine antes (posible huérfano adoptado por init).

Observaciones:
- No se hace wait explícito de Hijo2; se pretende ilustrar orfandad/exec.

---

## e3.c
Propósito:
- Calcular la media en doble precisión de tan(sqrt(i)) para los primeros N números naturales de dos formas y comparar:
  1) En paralelo con P hijos (bloques) + un hijo agregador (P+1).
  2) En un único hijo que calcula todo (P+2).
- Guardar resultados y tiempos en un fichero auxiliar (resultados.txt).

Qué hace:
- Recibe N y P por línea de comandos.
- Crea P hijos; cada uno calcula un intervalo y escribe una línea “PARCIAL” con media y tiempo en resultados.txt.
- Tras esperar a los P hijos, crea el hijo agregador (P+1) que lee el fichero y calcula la media “AGREGADO”.
- En paralelo a lo anterior, crea el hijo (P+2) que calcula la media completa “COMPLETO”.
- El padre lee “AGREGADO” y “COMPLETO”, muestra la diferencia y los tiempos.

Cómo ejecutar:
```bash
./e3 1000000 4
```

Resultados esperados:
- P líneas de hijos parciales con intervalos y tiempos.
- Una línea del agregador con media agregada.
- Una línea del cálculo completo.
- El padre imprime la diferencia entre ambas medias y los tiempos.

Observaciones importantes:
- Compilar con -lm (al final): gcc ... e3.c -o e3 -lm.
- El fichero empieza con una cabecera (“# Formato ...”). El agregador usa fscanf con un formato estricto y dejará de leer en cuanto encuentre esa primera línea, resultando en división por cero. Soluciones:
  - Eliminar la cabecera inicial, o
  - Cambiar la lectura a fgets + sscanf y saltar líneas que no empaten, o
  - Leer con fscanf ignorando la cabecera.
- El agregador promedia medias parciales por igual. Si N%P != 0, los bloques no tienen el mismo tamaño y eso sesga el resultado. Para exactitud, ponderar por el número de elementos de cada bloque.
- “Primeros N naturales”: en el código se empieza en i=0. Si se desea iniciar en 1, ajustar los bucles.
- Escrituras concurrentes en resultados.txt no usan bloqueo; en práctica suelen ser líneas cortas y funcionan, pero para total robustez usar bloqueo de fichero (fcntl) o escribir desde el padre.

---