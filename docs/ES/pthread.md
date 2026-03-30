# POSIX Threads — Guía de referencia

## 1. Mutex — `pthread_mutex_t`

Un mutex (mutual exclusion) es un cerrojo. Solo un thread puede tenerlo
a la vez. El resto espera hasta que se libere.

### Cuándo usarlo
Siempre que dos o más threads accedan a la misma variable — uno escribe,
otro lee, o ambos escriben. Sin mutex eso es una data race.

### Sintaxis completa
```c
// Declarar
pthread_mutex_t mi_mutex;

// Inicializar (antes de crear threads)
pthread_mutex_init(&mi_mutex, NULL);
// El segundo parámetro es atributos, NULL = comportamiento por defecto

// Bloquear — si otro thread lo tiene, espera aquí
pthread_mutex_lock(&mi_mutex);

// Desbloquear — libera el cerrojo
pthread_mutex_unlock(&mi_mutex);

// Destruir — siempre al final, después de pthread_join
pthread_mutex_destroy(&mi_mutex);
```

### Ejemplo real
```c
// Sin mutex — data race, valor impredecible
int contador = 0;

void *thread_fn(void *arg)
{
    contador++;  // dos threads haciendo esto a la vez = problema
    return NULL;
}

// Con mutex — seguro
pthread_mutex_t lock;
int contador = 0;

void *thread_fn(void *arg)
{
    pthread_mutex_lock(&lock);
    contador++;   // solo un thread aquí a la vez
    pthread_mutex_unlock(&lock);
    return NULL;
}
```

### Reglas de oro
- Lock y unlock siempre en el mismo thread
- Nunca lockear el mismo mutex dos veces en el mismo thread (deadlock)
- Siempre desbloquear antes de return o salida de función
- Mantener la sección crítica lo más corta posible

---

## 2. Condition Variable — `pthread_cond_t`

Una condition variable permite que un thread **espere** hasta que otro
thread le avise de que algo ha cambiado. Siempre se usa junto a un mutex.

### Cuándo usarlo
Cuando un thread necesita esperar a que se cumpla una condición antes
de continuar. Por ejemplo: esperar a que un dongle esté disponible.

### Sintaxis completa
```c
// Declarar
pthread_cond_t mi_cond;

// Inicializar
pthread_cond_init(&mi_cond, NULL);

// Esperar — SIEMPRE dentro de un mutex bloqueado
// Libera el mutex atómicamente y espera
// Cuando se despierta, vuelve a bloquear el mutex
pthread_cond_wait(&mi_cond, &mi_mutex);

// Esperar con timeout — como wait pero con límite de tiempo
struct timespec ts;
ts.tv_sec  = tiempo_en_segundos;
ts.tv_nsec = nanosegundos;
pthread_cond_timedwait(&mi_cond, &mi_mutex, &ts);

// Despertar a UN thread que esté esperando
pthread_cond_signal(&mi_cond);

// Despertar a TODOS los threads que estén esperando
pthread_cond_broadcast(&mi_cond);

// Destruir
pthread_cond_destroy(&mi_cond);
```

### Ejemplo real — productor/consumidor
```c
pthread_mutex_t lock;
pthread_cond_t  cond;
int recurso_disponible = 0;

// Thread que espera (consumidor)
void *consumidor(void *arg)
{
    pthread_mutex_lock(&lock);
    while (recurso_disponible == 0)          // while, nunca if
        pthread_cond_wait(&cond, &lock);     // espera y libera el mutex
    // aquí el recurso está disponible
    pthread_mutex_unlock(&lock);
    return NULL;
}

// Thread que avisa (productor)
void *productor(void *arg)
{
    pthread_mutex_lock(&lock);
    recurso_disponible = 1;
    pthread_cond_signal(&cond);    // despierta al consumidor
    pthread_mutex_unlock(&lock);
    return NULL;
}
```

### Por qué `while` y no `if`

Un thread puede despertarse sin que nadie le haya avisado (spurious wakeup).
Con `if` continuaría aunque la condición no se cumpla.
Con `while` vuelve a comprobar y si no se cumple, vuelve a esperar.
```c
// MAL — susceptible a spurious wakeups
if (recurso_disponible == 0)
    pthread_cond_wait(&cond, &lock);

// BIEN — siempre seguro
while (recurso_disponible == 0)
    pthread_cond_wait(&cond, &lock);
```

---

## 3. Threads — `pthread_t`

### Sintaxis completa
```c
// Declarar
pthread_t mi_thread;

// Crear — arranca la función en un thread nuevo
pthread_create(&mi_thread, NULL, mi_funcion, arg);
// parámetro 1: puntero al pthread_t
// parámetro 2: atributos, NULL = por defecto
// parámetro 3: función a ejecutar, firma: void *fn(void *)
// parámetro 4: argumento que recibe la función

// Esperar a que termine — bloquea hasta que el thread acabe
pthread_join(mi_thread, NULL);
// parámetro 2: si no es NULL, recoge el valor de return del thread
```

### Ejemplo real
```c
void *mi_funcion(void *arg)
{
    int *numero = (int *)arg;
    printf("Soy el thread, recibí: %d\n", *numero);
    return NULL;
}

int main(void)
{
    pthread_t thread;
    int       valor = 42;

    pthread_create(&thread, NULL, mi_funcion, &valor);
    pthread_join(thread, NULL);  // espera a que termine
    return 0;
}
```

### Reglas de oro
- Siempre hacer join a todos los threads que crees
- No acceder a variables locales de main desde threads si main puede terminar
- El argumento de pthread_create debe vivir mientras el thread lo use

---

## 4. Patrones frecuentes en Codexion

### Leer una variable compartida de forma segura
```c
int leer_flag(pthread_mutex_t *lock, int *flag)
{
    int valor;

    pthread_mutex_lock(lock);
    valor = *flag;
    pthread_mutex_unlock(lock);
    return valor;
}
```

### Escribir una variable compartida de forma segura
```c
void escribir_flag(pthread_mutex_t *lock, int *flag, int valor)
{
    pthread_mutex_lock(lock);
    *flag = valor;
    pthread_mutex_unlock(lock);
}
```

### Log thread-safe
```c
void log_action(pthread_mutex_t *log_lock, long long ts, int id, char *msg)
{
    pthread_mutex_lock(log_lock);
    printf("%lld %d %s\n", ts, id, msg);
    pthread_mutex_unlock(log_lock);
}
```

### Esperar con timeout (para cooldown)
```c
// Convertir ms a timespec (necesario para timedwait)
struct timespec ms_to_timespec(long ms)
{
    struct timespec ts;
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    ts.tv_sec  = tv.tv_sec + ms / 1000;
    ts.tv_nsec = (tv.tv_usec * 1000) + (ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000)
    {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }
    return ts;
}

// Uso
pthread_mutex_lock(&dongle->mutex);
while (dongle_no_disponible)
{
    struct timespec ts = ms_to_timespec(cooldown_ms);
    pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
}
pthread_mutex_unlock(&dongle->mutex);
```

---

## 5. Resumen visual
```
PROBLEMA                    SOLUCIÓN
─────────────────────────────────────────────────
Dos threads tocan           pthread_mutex_t
la misma variable

Un thread espera            pthread_cond_t
a que algo cambie           + pthread_mutex_t

Lanzar trabajo              pthread_create
en paralelo                 + pthread_join
```

---

## 6. Errores más comunes
```
ERROR                           CONSECUENCIA
──────────────────────────────────────────────────────
Lockear dos veces               Deadlock — el programa
el mismo mutex                  se congela para siempre

Olvidar unlock                  Deadlock — nadie más
antes de return                 puede entrar

Leer variable                   Data race — valor
sin mutex                       impredecible o corrupto

if en vez de while              Spurious wakeup —
con cond_wait                   continúa sin condición

No hacer join                   Thread huérfano —
a un thread                     leak de recursos
```