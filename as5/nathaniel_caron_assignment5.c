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
 * Solution:
 * With the use of semaphores this program allows all cars to cross the bridge.
 * To avoid STARVATION:
 * If 5 cars in a row in the same direction advance on bridge, then cars in the other direction will be allowed to advance.
 */

// Structure to represent a car thread
typedef struct Car {
    char *car;
    char *direction;
    int arrival;
    int duration;
    int duration_left;
    int done;
    int exited;
} Car;

sem_t *car_sem;
sem_t time_sem;
sem_t bridge_sem;

Car *cars;
int car_count = 0;
int cars_on_bridge = 0;
int cars_since_direction_change = 1;
char *current_direction;
char *next_direction;
int earliest_arrival = 0;
int current_time = 0;
int thread_num = 0;

// Function templates
void *car_thread_execute(void *car_ptr);
void initProcesses(Car **cars, size_t *size);
void reallocateProcesses(Car **cars, size_t *size);
int getProcesses(Car **cars, size_t *cars_size);

int main(int argc, char **argv) {
    int i = 0;
    current_direction = (char*)calloc(4, sizeof(current_direction));
    next_direction = (char*)calloc(4, sizeof(next_direction));

    // Initialize cars, VARIABLE SIZE
    size_t initial_size;
    initProcesses(&cars, &initial_size);

    // Get the cars from the input file
    car_count = getProcesses(&cars, &initial_size);
    for (i = 0; i < car_count; i++) {
        cars[i].done = 0;
        cars[i].exited = 0;
    }

    // Determine the earliest arrival time and determine initial direction
    earliest_arrival = cars[0].arrival;
    strcpy(current_direction, cars[0].direction);
    strcpy(next_direction, cars[0].direction);
    for (i = 0; i < car_count; i++) {
        if (cars[i].arrival < earliest_arrival) {
            earliest_arrival = cars[i].arrival;
            strcpy(current_direction, cars[i].direction);
            strcpy(next_direction, cars[i].direction);
        }
    }

    current_time = earliest_arrival;

    car_sem = (sem_t *)calloc(car_count, sizeof(car_sem));

    // Initialize semaphores
    sem_init(&time_sem, 0, 0);
    sem_init(&bridge_sem, 0, 1);
    for (i = 0; i < car_count; i++) {
        sem_init(&car_sem[i], 0, 0);
    }

    // Print initial direction
    if (strcmp(current_direction, "N") == 0) {
        printf("Direction: North\n");
    } else {
        printf("Direction: South\n");
    }

    // Create and all threads
    pthread_t threads[car_count];
    for (i = 0; i < car_count; i++) {
        pthread_create(&threads[i], NULL, &car_thread_execute, &cars[i]);
    }

    // printf("\nCurrent time: %d, current direction: %s, next direction: %s\n", current_time, current_direction, next_direction);

    int is_done = 0;
    while (is_done == 0) {
        // Check if all cars have exited the bridge
        is_done = 1;
        for (i = 0; i < car_count; i++) {
            if (cars[i].duration_left > 0) {
                is_done = 0;
                break;
            }
            if (i == car_count-1) {
                // printf("--- All cars have exited the bridge ---\n");
            }
        }

        sem_wait(&time_sem);
        current_time++;

        // Avoid STARVATION: Allow cars in other direction to advance
        // if (cars_since_direction_change == 5) {
        //         if (strcmp(next_direction, "N") == 0) {
        //             strcpy(next_direction, "S");
        //         } else {
        //             strcpy(next_direction, "N");
        //         }
        //         cars_since_direction_change = 0;
        // }

        // Reset all car threads
        for (i = 0; i < car_count; i++) {
            if (cars[i].exited != 1) {
                cars[i].done = 0;
            }
        }
        for (i = 0; i < car_count; i++) {
            sem_post(&car_sem[i]);
        }
    }

    printf("Main got here 1\n");

    // Join all threads
    // for (i = 0; i < car_count; i++) {
    //     pthread_join(threads[i], NULL);
    // }

    printf("Main got here 2\n");

    // Destroy all semaphores
    // sem_destroy(&time_sem);
    // sem_destroy(&bridge_sem);
    // for (i = 0; i < car_count; i++) {
    //     sem_destroy(&car_sem[i]);
    // }
}

void *car_thread_execute(void *car_ptr) {
    int i;
    Car *car = (Car *)car_ptr;
    sem_t *thread_sem;

    sem_wait(&bridge_sem);
        thread_sem = &car_sem[thread_num];
        thread_num++;
    sem_post(&bridge_sem);

    int done = 0;
    while (done == 0) {
        // Wait if car has not entered the bridge
        while ((current_time < car->arrival) ||
               (strcmp(current_direction, car->direction) != 0 && cars_on_bridge != 0)) {
                // printf("%s still waiting\n", car->car);
                // printf("time: %d, arrival: %d\n", current_time, car->arrival);
                // printf("current direction: %s, cars on bridge: %d\n", current_direction, cars_on_bridge);
                // printf("next direction: %s, duration: %d, duration left: %d\n", current_direction, car->duration, car->duration_left);

            sem_wait(&bridge_sem);
                car->done = 1;
                int alldone = 1;
                for (i = 0; i < car_count; i++) {
                    if (cars[i].done != 1) {
                        alldone = 0;
                    }
                }
                if (alldone == 1) {
                    sem_post(&time_sem);
                }
            sem_post(&bridge_sem);
            // Wait
            sem_wait(thread_sem);
        }

        // Car enters the bridge
        sem_wait(&bridge_sem);
            // Change the current direction
            if (strcmp(current_direction, car->direction) != 0) {
                strcpy(current_direction, car->direction);
                if (strcmp(current_direction, "N") == 0) {
                    printf("Direction: North\n");
                } else {
                    printf("Direction: South\n");
                }
            }
            if (car->duration == car->duration_left) {
                cars_on_bridge++;
                cars_since_direction_change++;
                printf("%s\n", car->car);
            }
        sem_post(&bridge_sem);

        // Advance on bridge
        car->duration_left--;
        car->done = 1;

        // Check if all threads are done
        sem_wait(&bridge_sem);
            int alldone = 1;
            for (i = 0; i < car_count; i++) {
                if (cars[i].done != 1) {
                    alldone = 0;
                }
            }
            if (alldone == 1) {
                sem_post(&time_sem);
            }
        sem_post(&bridge_sem);

        // Wait
        sem_wait(thread_sem);

        // Car exits the bridge
        sem_wait(&bridge_sem);
            if (car->duration_left <= 0) {
                cars_on_bridge--;
                car->exited = 1;
                car->done = 1;
                // printf("%s exits the bridge, number of cars on bridge: %d\n", car->car, cars_on_bridge);
                // Check if all threads are done
                int alldone = 1;
                for (i = 0; i < car_count; i++) {
                    if (cars[i].done != 1) {
                        alldone = 0;
                    }
                }
                if (alldone == 1) {
                    sem_post(&time_sem);
                }
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
                (*cars)[cars_count].duration_left = atoi(token);
            }
            token_num++;
        }
        cars_count++;
    }

    return cars_count;
}