#include <pthread.h>
#include <stdio.h>

void *funcion(void* arg) {
    printf("Hola desde el hilo\n");
    return NULL;
}

int main() {
    pthread_t hilo;
    pthread_create(&hilo, NULL, funcion, NULL);
    pthread_join(hilo, NULL);

    return 0;
}
