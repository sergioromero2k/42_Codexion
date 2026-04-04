*Este proyecto ha sido creado como parte del currículo de 42 por serromer.*
# CODEXION

## Descripción

Codexion es una simulación de concurrencia escrita en C que modela un espacio de trabajo compartido donde los programadores compiten por recursos hardware limitados. Cada programador debe adquirir simultáneamente dos dongles USB raros para compilar su código cuántico antes de sufrir un burnout por inactividad. Este proyecto es una variante compleja del clásico problema de los Filósofos Cenando, introduciendo restricciones adicionales como el enfriamiento de recursos y políticas de planificación dinámicas (FIFO y EDF) para prevenir bloqueos mutuos y garantizar una distribución justa de recursos.

## Instrucciones

### Compilación
```bash
# Desde la raíz del repositorio
make -C coders

# O desde dentro de coders/
cd coders && make
```

### Ejecución
```bash
./coders/codexion numero_de_coders tiempo_burnout tiempo_compilar \
                  tiempo_debug tiempo_refactor compilaciones_requeridas \
                  cooldown_dongle planificador
```

| Argumento | Descripción |
|---|---|
| `numero_de_coders` | Número de programadores y dongles |
| `tiempo_burnout` | Máximo ms sin compilar antes del burnout |
| `tiempo_compilar` | Duración de la compilación en ms |
| `tiempo_debug` | Duración del debugging en ms |
| `tiempo_refactor` | Duración del refactoring en ms |
| `compilaciones_requeridas` | Compilaciones necesarias por coder para terminar |
| `cooldown_dongle` | Ms que un dongle no está disponible tras soltarse |
| `planificador` | `fifo` o `edf` |

### Ejemplo
```bash
./coders/codexion 5 500 100 100 100 7 50 edf
```

## Recursos

### Referencias

- [Documentación POSIX Threads](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [Problema de los Filósofos Cenando](https://es.wikipedia.org/wiki/Problema_de_la_cena_de_los_fil%C3%B3sofos)
- [Guía de programación POSIX Threads](https://hpc-tutorials.llnl.gov/posix/)
- [Planificación EDF](https://es.wikipedia.org/wiki/Earliest_deadline_first)

### Uso de IA

Las herramientas de IA se utilizaron para aclarar conceptos complejos de concurrencia, entender las primitivas de pthread y depurar carreras de datos identificadas durante las pruebas. La lógica de sincronización, la gestión de threads, la implementación de la cola de prioridad y todas las estructuras de datos compatibles con C89 fueron implementadas por el desarrollador para garantizar una comprensión completa durante la evaluación entre pares.

## Casos de bloqueo gestionados

### Prevención de deadlock

Para prevenir la espera circular — una condición clave de Coffman — los coders con ID par esperan brevemente al inicio antes de intentar adquirir los dongles. Esto rompe la simetría que hace que todos los coders intenten coger su dongle izquierdo simultáneamente, garantizando que al menos un coder siempre pueda acceder a ambos recursos y progresar.

### Prevención de inanición

El planificador EDF (Earliest Deadline First) prioriza a los coders más cercanos al burnout. Cada dongle mantiene una cola de prioridad (min-heap) de coders esperando, ordenados por su deadline (`last_compile_time + time_to_burnout`). El coder con el deadline más cercano siempre es atendido primero, evitando la espera indefinida.

### Gestión del cooldown

Cada estructura `t_dongle` almacena un timestamp `cooldown_until` en milisegundos. Cuando un coder suelta un dongle, `cooldown_until` se establece en `tiempo_actual + dongle_cooldown`. Cualquier coder que intente adquirir el dongle antes de este timestamp usa `pthread_cond_timedwait` para dormir exactamente hasta que expire el cooldown, evitando la espera activa.

### Detección de burnout

Un thread monitor dedicado se ejecuta independientemente, comprobando cada 500 microsegundos si `ahora - last_compile_time >= time_to_burnout` para cada coder. Al detectarlo, el mensaje de burnout se imprime inmediatamente dentro del margen requerido de 10ms, `simulation_over` se establece en 1, y todos los coders en espera son despertados mediante `pthread_cond_broadcast`.

### Serialización de logs

Un mutex global `log_lock` envuelve cada llamada a `printf` en todos los threads. Esto garantiza que los mensajes de cambio de estado se impriman atómicamente — ningún mensaje puede entrelazarse con otro en la misma línea de salida, independientemente de cuántos threads estén ejecutándose simultáneamente.

## Mecanismos de sincronización de threads

### Uso de mutex

Cuatro mutex distintos protegen diferentes recursos compartidos:

- **`log_lock`**: Mutex global que envuelve cada `printf`. Evita la salida entrelazada entre los threads de los coders y el monitor.

- **`env->state_lock`**: Protege `simulation_over`, el flag global de parada. El monitor lo escribe mediante `set_sim_over()` y todos los coders lo leen mediante `sim_is_over()`. Sin este mutex, un coder podría leer un valor parcialmente escrito y continuar ejecutándose después de que se detecte el burnout.

- **`dongle->mutex`**: Uno por dongle. Protege `in_use`, `cooldown_until` y la cola de prioridad `waiters`. Evita que dos coders adquieran simultáneamente el mismo dongle.

- **`coder->state_lock`**: Uno por coder. Protege `last_compile_time` y `compile_count`. El monitor lee estos valores mientras el thread del coder los actualiza — este mutex evita que el monitor lea un valor intermedio corrupto.

### Variables de condición

Cada dongle tiene un `pthread_cond_t cond` para gestionar su cola de espera eficientemente sin espera activa:

- **`pthread_cond_wait`**: Se usa en `take_one_dongle` cuando el dongle está ocupado o aún no es el turno del coder según el planificador.
- **`pthread_cond_timedwait`**: Se usa cuando el dongle está en cooldown — el coder duerme hasta el momento exacto en que expira el cooldown.
- **`pthread_cond_broadcast`**: Se llama en `drop_dongles` tras soltar un dongle, y en `handle_burnout` para despertar a todos los coders en espera y que puedan detectar `simulation_over` y salir limpiamente.

### Thread monitor

Un thread monitor dedicado ejecuta `monitor_routine`, comprobando dos condiciones de parada cada 500 microsegundos:

- **Burnout**: `check_health()` calcula `ahora - last_compile_time` para cada coder usando `coder->state_lock`. Si supera `time_to_burnout`, `handle_burnout()` imprime el mensaje de burnout, establece `simulation_over` y hace broadcast a todos los dongles.
- **Completado**: `check_all_compiled()` verifica que el `compile_count` de cada coder sea `>= number_of_compiles_required`. Si es cierto, se llama a `set_sim_over()` y la simulación termina silenciosamente.

### Extra

```
coders/                   # Obligatorio: Archivos fuente principales 
├── Makefile              # Compila con -Wall -Wextra -Werror -pthread
├── codexion.h            # Cabecera global con estructuras y prototipos
├── main.c                # Punto de entrada y gestión de argumentos 
├── parsing.c             # Validación de los argumentos obligatorios 
├── init.c                # Inicialización de mutexes, hilos y datos
├── routine.c             # Ciclo de vida del coder (compilar, depurar, refactorizar)
├── actions.c             # Adquisición de dongles y cambios de estado
├── actions_utils.c       # Ayudas para sincronización y temporización 
├── monitor.c             # Hilo de monitoreo para detección de burnout 
├── heap.c                # Implementación de cola de prioridad para FIFO/EDF 
├── heap_utils.c          # Funciones de utilidad del min-heap para el scheduler 
└── utils.c               # Utilidades generales (tiempo, limpieza, strings)
docs/                     # Documentación y planificación del proyecto
├── en.subject_codexion.pdf
├── codexion_plan.html
├── ES/                   # Notas y traducciones al español
└── tests/                # Programas de prueba para módulos internos
    └── test_heap.c       # Verificación de la lógica de la cola de prioridad
README.md                 # Este archivo 
```