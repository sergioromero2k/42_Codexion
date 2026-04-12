# Guía de defensa — Codexion

## Preguntas sobre el proyecto en general

**¿Qué es Codexion?**
Es una simulación de concurrencia en C basada en el problema de los Filósofos Cenando. Hay N coders sentados en círculo, cada uno necesita dos dongles USB para compilar, y solo hay N dongles en total. El reto es coordinar el acceso a los dongles sin deadlocks ni burnouts.

**¿Qué es un burnout?**
Si un coder no empieza a compilar dentro de `time_to_burnout` milisegundos desde su última compilación o desde el inicio, muere. La simulación para y se imprime "burned out".

**¿Cuándo termina la simulación?**
En dos casos — cuando un coder sufre burnout, o cuando todos los coders han compilado al menos `number_of_compiles_required` veces.

**¿Por qué es una variante del problema de los Filósofos?**
Los filósofos necesitan dos tenedores para comer, los coders necesitan dos dongles para compilar. El problema es el mismo — recursos compartidos limitados entre múltiples entidades concurrentes.

---

## Preguntas sobre threads

**¿Qué es un thread?**
Un hilo de ejecución independiente dentro del mismo proceso. Comparte memoria con los otros threads pero tiene su propia pila de ejecución.

**¿Cuántos threads tiene tu programa?**
N threads de coders + 1 thread monitor = N+1 threads en total.

**¿Cómo creas los threads?**
Con `pthread_create`. Cada coder es un thread que ejecuta `coder_routine`. El monitor ejecuta `monitor_routine`.

**¿Cómo esperas a que terminen?**
Con `pthread_join`. Sin join el main podría terminar antes que los threads.

---

## Preguntas sobre mutex

**¿Qué es un mutex?**
Un cerrojo de exclusión mutua. Solo un thread puede tenerlo a la vez. El resto espera hasta que se libere.

**¿Cuántos mutex tienes y para qué sirve cada uno?**
- `log_lock` → protege todos los printf
- `env->state_lock` → protege `simulation_over`
- `dongle->mutex` → protege cada dongle individual
- `coder->state_lock` → protege `last_compile_time` y `compile_count`

**¿Qué pasa si no usas mutex?**
Data race — dos threads acceden a la misma memoria simultáneamente, el resultado es impredecible o corrupto.

**¿Qué es un deadlock?**
Cuando dos threads se bloquean mutuamente esperando un recurso que el otro tiene. Por ejemplo, coder 1 tiene el dongle izquierdo y espera el derecho, coder 2 tiene el derecho y espera el izquierdo — ambos esperan para siempre.

**¿Cómo preveniste el deadlock?**
Los coders con ID par hacen `usleep` al inicio. Esto rompe la simetría — no todos intentan coger su dongle izquierdo al mismo tiempo, garantizando que siempre haya al menos un coder que pueda avanzar.

---

## Preguntas sobre condition variables

**¿Qué es `pthread_cond_wait`?**
Libera el mutex y duerme el thread hasta que otro thread le avise con signal o broadcast. Cuando se despierta, vuelve a bloquear el mutex automáticamente.

**¿Por qué usas `while` y no `if` con `cond_wait`?**
Por los spurious wakeups — el sistema puede despertar un thread sin razón. Con `while` el thread siempre recomprueba la condición al despertar. Con `if` podría continuar aunque la condición no se cumpla.

**¿Cuándo usas `cond_timedwait`?**
Cuando el dongle está en cooldown. En vez de esperar indefinidamente, el coder calcula exactamente cuándo expira el cooldown y duerme hasta ese momento preciso.

**¿Cuándo usas `broadcast` y cuándo `signal`?**
`broadcast` — cuando sueltas un dongle, porque varios coders pueden estar esperando y todos deben recomprobar.
`signal` — solo despertaría a uno, que puede no ser el más prioritario.

---

## Preguntas sobre el scheduler

**¿Qué es FIFO?**
First In First Out — el dongle se da al coder que lo pidió primero. La prioridad es el timestamp de llegada.

**¿Qué es EDF?**
Earliest Deadline First — el dongle se da al coder con el deadline más cercano, es decir, el que está más cerca del burnout. La prioridad es `last_compile_time + time_to_burnout`.

**¿Dónde cambia el comportamiento entre FIFO y EDF?**
Solo en `get_priority` — el valor que se mete como prioridad en el heap al hacer push. El resto del código es idéntico.

**¿Por qué EDF es mejor que FIFO?**
Porque prioriza a los más urgentes, reduciendo los burnouts. Con FIFO un coder puede estar a punto de morir pero esperar detrás de otros que llegaron antes.

---

## Preguntas sobre el heap

**¿Por qué implementaste un heap?**
Para gestionar la cola de espera de cada dongle de forma eficiente. El heap siempre da el elemento más prioritario en O(1) sin recorrer toda la cola.

**¿Qué es un min-heap?**
Un árbol binario donde el padre siempre es menor que sus hijos. El mínimo siempre está en la raíz — índice 0 del array.

**¿Cómo funciona `sift_up`?**
Después de insertar un nodo al final, lo sube comparándolo con su padre y swapeando si es menor, hasta llegar a la raíz o encontrar un padre menor.

**¿Cómo funciona `sift_down`?**
Después de extraer la raíz, pone el último elemento en su lugar y lo baja comparándolo con el menor de sus hijos, hasta no tener hijos o ser menor que ambos.

**¿Qué guarda cada nodo del heap?**
`coder_id` — quién es el coder, y `priority` — su valor de ordenación (timestamp para FIFO, deadline para EDF).

---

## Preguntas sobre el monitor

**¿Qué hace el monitor?**
Es un thread separado que comprueba dos condiciones cada 500 microsegundos — si algún coder ha superado `time_to_burnout`, y si todos han compilado suficiente.

**¿Por qué 500 microsegundos?**
El subject exige detectar el burnout en ≤10ms. Con `usleep(500)` el monitor comprueba cada 0.5ms, dando un margen de 20 comprobaciones por cada 10ms.

**¿Cómo para la simulación limpiamente?**
El monitor pone `simulation_over = 1` y hace broadcast en todos los dongles. Los coders bloqueados en `cond_wait` se despiertan, comprueban `sim_is_over` y salen de su bucle.

---

## Preguntas sobre el tiempo

**¿Qué es `gettimeofday`?**
Función del sistema que devuelve el tiempo actual en segundos (`tv_sec`) y microsegundos (`tv_usec`) desde el 1 de enero de 1970.

**¿Por qué conviertes a milisegundos?**
Porque todos los parámetros del programa están en milisegundos. Es más sencillo trabajar en una sola unidad.

**¿Qué es `get_timestamp`?**
Devuelve los milisegundos transcurridos desde el inicio de la simulación — `get_time_in_ms() - start_time`.

**¿Para qué necesitas `ms_to_timespec`?**
`pthread_cond_timedwait` necesita un `struct timespec` con tiempo absoluto en segundos y nanosegundos. `ms_to_timespec` convierte los milisegundos del cooldown al formato que entiende `cond_timedwait`.

---

## Casos límite — posibles preguntas trampa

**¿Qué pasa con 1 coder?**
Con un solo dongle nunca puede tener dos, así que nunca puede compilar. El monitor detecta el burnout y termina.

**¿Qué pasa si `dongle_cooldown` es 0?**
No hay período de enfriamiento. El dongle está disponible inmediatamente después de soltarse. Funciona correctamente.

**¿Qué pasa si `number_of_compiles_required` es 0?**
La simulación debería terminar inmediatamente porque todos ya han compilado 0 veces. Comprueba que tu `check_all_compiled` lo gestiona correctamente.

**¿Hay memory leaks?**
No — Valgrind confirma 0 leaks. Todos los mutex se destruyen en `cleanup`, todos los heaps se liberan con `pqueue_free`, y los arrays de coders y dongles se liberan con `free`.