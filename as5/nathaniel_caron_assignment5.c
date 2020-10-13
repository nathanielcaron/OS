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

sem_t car_waiting_sem;
sem_t car_arrived_sem;
sem_t time_sem;
sem_t bridge_sem;

int car_count = 0;

int cars_done = 0;

int cars_waiting = 0;
int cars_arrived = 0;

char *current_direction;
int earliest_arrival = 0;
int current_time = 0;

// Function templates
void *car_thread_execute(void *car_ptr);
void initProcesses(Car **cars, size_t *size);
void reallocateProcesses(Car **cars, size_t *size);
int getProcesses(Car **cars, size_t *cars_size);

int main(int argc, char **argv) {
    current_direction = (char*)calloc(4, sizeof(char));

    int i = 0;

    // TODO: Remove this
    int n_num = 0;
    int s_num = 0;

    // Initialize cars, VARIABLE SIZE
    Car *cars;
    size_t initial_size;
    initProcesses(&cars, &initial_size);

    // Get the cars from the input file
    car_count = getProcesses(&cars, &initial_size);

    cars_waiting = car_count;

    // Determine the earliest arrival time
    earliest_arrival = cars[0].arrival;
    strcpy(current_direction, cars[0].direction);
    for (i = 0; i < car_count; i++) {
        if (cars[i].arrival < earliest_arrival) {
            earliest_arrival = cars[i].arrival;
            strcpy(current_direction, cars[i].direction);
        }
        if (strcmp(cars[i].direction, "N") == 0) {
            n_num++;
        } else {
            s_num++;
        }
    }

    current_time = earliest_arrival;

    // TODO: REMOVE THIS
    printf("Earliest arrival: %d\n", earliest_arrival);
    printf("Current direction: %s\n", current_direction);
    printf("Number of north cars: %d\n", n_num);
    printf("Number of south cars: %d\n\n", s_num);
    for (i = 0; i < car_count; i++) {
        printf("%s\t%s\t%d\t%d\n", cars[i].car, cars[i].direction, cars[i].arrival, cars[i].duration);
    }
    printf("\n");

    // Initialize semaphores
    sem_init(&car_waiting_sem, 0, 0);
    sem_init(&car_arrived_sem, 0, 0);
    sem_init(&time_sem, 0, 0);
    sem_init(&bridge_sem, 0, 1);


    // Create and join all threads
    pthread_t threads[car_count];

    for (i = 0; i < car_count; i++) {
        pthread_create(&threads[i], NULL, &car_thread_execute, &cars[i]);
    }

    if (strcmp(current_direction, "N") == 0) {
        printf("Direction: North\n");
    } else {
        printf("Direction: South\n");
    }

    int is_done = 0;
    while (is_done == 0) {
        printf("Time: %d, Cars arrived: %d\n", current_time, cars_arrived);

        // Remove this
        if (current_time == 4) {
            printf("\n --- Done -- \n");
            for (i = 0; i < car_count; i++) {
                printf("%s\t%s\t%d\t%d\n", cars[i].car, cars[i].direction, cars[i].arrival, cars[i].duration);
            }
            printf(" --- Done ---\n");
            return 0;
        }

        // Check if all cars have exited the bridge
        is_done = 1;
        for (i = 0; i < car_count; i++) {
            if (cars[i].duration > 0) {
                is_done = 0;
                break;
            }
        }

        sem_wait(&time_sem);
        current_time++;
    
        for (i = 0; i < cars_waiting; i++) {
            sem_post(&car_waiting_sem);
        }

        for (i = 0; i < cars_arrived; i++) {
            sem_post(&car_arrived_sem);
        }
    }

    for (i = 0; i < car_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // TODO Update with current semaphores
    sem_destroy(&car_waiting_sem);
    sem_destroy(&car_arrived_sem);
    sem_destroy(&time_sem);
    sem_destroy(&bridge_sem);
}

void *car_thread_execute(void *car_ptr) {
    int done = 0;
    Car *car = (Car *)car_ptr;
    // TODO Remove this
    printf("thread for car %s\n", car->car);

    while (done == 0) {
        while (current_time < car->arrival || (strcmp(current_direction, car->direction) != 0 && strcmp(current_direction, "NONE") != 0)) {
            sem_wait(&bridge_sem);
                cars_done++;
                printf("Thread %s done\n", car->car);
                if (cars_done == car_count) {
                    printf("\n --- All car threads are done ---\n");
                    sem_post(&time_sem);
                    cars_done = 0;
                }
            sem_post(&bridge_sem);
            // Wait
            sem_wait(&car_waiting_sem);
        }

        sem_wait(&bridge_sem);
            if (current_time == car->arrival) {
                cars_waiting--;
                cars_arrived++;
            }
        sem_post(&bridge_sem);

        // Change the current direction
        sem_wait(&bridge_sem);
            if (strcmp(current_direction, car->direction) != 0) {
                strcpy(current_direction, car->direction);
                if (strcmp(current_direction, "N") == 0) {
                    printf("Direction: North\n");
                } else {
                    printf("Direction: South\n");
                }
            }
        sem_post(&bridge_sem);

        car->duration--;

        sem_wait(&bridge_sem);
            cars_done++;
            printf("Thread %s done\n", car->car);
            if (cars_done == car_count) {
                printf("\n --- All car threads are done ---\n");
                sem_post(&time_sem);
                cars_done = 0;
            }
        sem_post(&bridge_sem);

        sem_wait(&car_arrived_sem);

        sem_wait(&bridge_sem);
            if (car->duration <= 0) {
                printf("%s\n", car->car);
                cars_arrived--;
                car_count--;
                printf("Cars arrived: %d\n", cars_arrived);
                printf("Car count: %d\n", car_count);
                if (cars_arrived == 0) {
                    strcpy(current_direction, "NONE");
                }
                printf("Current direction: %s\n", current_direction);
                done = 1;
            }
        sem_post(&bridge_sem);
    }

    return NULL;
}

// Function to initialize the cars array (initial size expanded as needed)
void initProcesses(Car **cars, size_t *size) {
    *size = 500;
    *cars = (Car*)calloc(*size, sizeof(Car));
}

// Function to reallocate space for the cars (VARIABLE SIZE)
void reallocateProcesses(Car **cars, size_t *size) {
    *size *= 2;
    *cars = (Car*)realloc(*cars, (*size) * sizeof(Car));
}

// Function used to read in the cars from the input
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
