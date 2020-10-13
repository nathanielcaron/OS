#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/**
 * Assignment 5 - Semaphores
 * CS 3413
 * Nathaniel Caron
 * 3598979
 * 
 */

// Structure to represent a car thread
typedef struct Car {
    char *car;
    char *direction;
    int arrival;
    int duration;
} Car;

sem_t bridge_sem;
char *current_direction = "NONE";
int earliest_arrival = 0;

// Function templates
void *car_thread_execute(void *car_ptr);
void initProcesses(Car **cars, size_t *size);
void reallocateProcesses(Car **cars, size_t *size);
int getProcesses(Car **cars, size_t *cars_size);

int main(int argc, char **argv) {
    // For loop variables
    int i = 0;
    int j = 0;

    // Initialize semaphore
    sem_init(&bridge_sem, 0, 0);

    // Initialize cars, VARIABLE SIZE
    Car *cars;
    size_t initial_size;
    initProcesses(&cars, &initial_size);

    // Get the cars from the input file
    int cars_count = getProcesses(&cars, &initial_size);

    // Determine the earliest arrival time
    earliest_arrival = cars[0].arrival;
    for (i = 0; i < cars_count; i++) {
        if (cars[i].arrival < earliest_arrival) {
            earliest_arrival = cars[i].arrival;
        }
    }

    printf("Earliest arrival: %d\n", earliest_arrival);

    // TODO: REMOVE THIS
    for (i = 0; i < cars_count; i++) {
        printf("%s\t%s\t%d\t%d\n", cars[i].car, cars[i].direction, cars[i].arrival, cars[i].duration);
    }

    // Create and join all threads
    pthread_t threads[cars_count];

    for (i = 0; i < cars_count; i++) {
        pthread_create(&threads[i], NULL, &car_thread_execute, &cars[i]);
    }

    for (i = 0; i < cars_count; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&bridge_sem);
}

void *car_thread_execute(void *car_ptr) {
    int time = earliest_arrival;
    int done = 0;
    Car *car = (Car *)car_ptr;

    printf("thread for car %s\n", car->car);

    while (done == 0) {
        if (time == car->arrival) {
            printf("%s has arrived at time %d\n", car->car, time);
            done = 1;
        }
        time++;
    }

    return NULL;
}

// Function to initialize the processes array (initial size expanded as needed)
void initProcesses(Car **cars, size_t *size) {
    *size = 500;
    *cars = (Car*)calloc(*size, sizeof(Car));
}

// Function to reallocate space for the processes (VARIABLE SIZE)
void reallocateProcesses(Car **cars, size_t *size) {
    *size *= 2;
    *cars = (Car*)realloc(*cars, (*size) * sizeof(Car));
}

// Function used to read in the processes from the input
int getProcesses(Car **cars, size_t *cars_size) {
    int line_len = 1000;
    char line[1000] = {0};

    size_t cars_count = 0;

    // Ignore input header
    fgets(line, line_len, stdin);

    while (fgets(line, line_len, stdin) != NULL) {
        int token_num = 0;

        // Reallocate more space for processes if needed (VARIABLE SIZE)
        if (cars_count == *cars_size) {
            reallocateProcesses(cars, cars_size);
        }

        // Tokenize each line
        char *token;
        for (token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 0) {
                (*cars)[cars_count].car = strdup(token);
            } else if (token_num == 1) {
                (*cars)[cars_count].direction = strdup(token);
            } else if (token_num == 2) {
                (*cars)[cars_count].arrival = atoi(token);
            } else if (token_num == 3) {
                (*cars)[cars_count].duration = atoi(token);
            }
            token_num++;
        }
        cars_count++;
    }

    return cars_count;
}
