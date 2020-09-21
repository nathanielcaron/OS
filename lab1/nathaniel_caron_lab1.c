#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

static double global_value = 1.0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* multiply(void *arg) {
    int i = 0;
    for (i = 0; i < 100; i++) {
        pthread_mutex_lock(&mutex);
        global_value *= 2;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* divide(void *arg) {
    int i = 0;
    for (i = 0; i < 100; i++) {
        pthread_mutex_lock(&mutex);
        global_value /= 2;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t thread_multiply;
    pthread_t thread_divide;

    pthread_create(&thread_multiply, NULL, &multiply, NULL);
    pthread_create(&thread_divide, NULL, &divide, NULL);

    pthread_join(thread_multiply, NULL);
    pthread_join(thread_multiply, NULL);

    pthread_mutex_destroy(&mutex);

    printf("%f\n", global_value);
    return EXIT_SUCCESS;
}