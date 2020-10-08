#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/**
 * Assignment 4 - Producer Consumer problem
 * CS 3413
 * Nathaniel Caron
 * 3598979
 * 
 * Solution:
 * Made use of MUTEXES along with CONDITION VARIABLES to control data acess and avoid busy waiting
 */

// Function templates
int randomTime();

// Global variables
int time_limit = 0;
time_t start_time = 0;
time_t current_time = 0;

int paddles = 0;
int canoes = 0;
int paddle_done = 0;
int canoe_done = 0;

// Condition variables to avoid busy waiting
pthread_cond_t paddle_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t canoe_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t shipper_cond = PTHREAD_COND_INITIALIZER;

// Mutexes to control data access
pthread_mutex_t paddle_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t canoe_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t shipper_mutex = PTHREAD_MUTEX_INITIALIZER;



// Function to be executed by the paddle thread
void *paddle_function(void *arg) {
    int random_wait_time = 0;
    current_time = time(NULL);

    while ((current_time - start_time) < time_limit) {
        pthread_mutex_lock(&paddle_mutex);
            // Use condition variable to avoid busy waiting
            while (paddles == 2) {
                pthread_cond_wait(&paddle_cond, &paddle_mutex);
            }

            random_wait_time = randomTime();
            sleep(random_wait_time);
            paddles++;
            printf("We have a paddle.\n");
        pthread_mutex_unlock(&paddle_mutex);

        // Signal the consumer when package is ready
        if (paddles == 2 && canoes == 1) {
            pthread_cond_signal(&shipper_cond);
        }

        current_time = time(NULL);
    }

    paddle_done = 1;
    pthread_cond_signal(&shipper_cond);
    return NULL;
}

// Function to be executed by the canoe thread
void *canoe_function(void *arg) {
    int random_wait_time = 0;
    current_time = time(NULL);

    while ((current_time - start_time) < time_limit) {
        pthread_mutex_lock(&canoe_mutex);
            // Use condition variable to avoid busy waiting
            while (canoes == 1) {
                pthread_cond_wait(&canoe_cond, &canoe_mutex);
            }

            random_wait_time = randomTime();
            sleep(random_wait_time);
            canoes++;
            printf("We have a canoe.\n");
        pthread_mutex_unlock(&canoe_mutex);

        // Signal the consumer when package is ready
        if (paddles == 2 && canoes == 1) {
            pthread_cond_signal(&shipper_cond);
        }

        current_time = time(NULL);
    }

    canoe_done = 1;
    pthread_cond_signal(&shipper_cond);
    return NULL;
}

// Function to be executed by the shipper thread
void *shipper_function(void *arg) {
    current_time = time(NULL);

    while ((current_time - start_time) < time_limit) {
        pthread_mutex_lock(&shipper_mutex);
            // Use condition variable to avoid busy waiting
            while (paddles < 2 || canoes < 1) {
                pthread_cond_wait(&shipper_cond, &shipper_mutex);
                if (paddle_done == 1 && canoe_done == 1) {
                    return NULL;
                }
            }

            pthread_mutex_lock(&paddle_mutex);
                pthread_mutex_lock(&canoe_mutex);
                    paddles -= 2;
                    canoes--;
                    printf("We now have a shipment!\n");

                    // Signal producers that they can proceed
                    pthread_cond_signal(&paddle_cond);
                    pthread_cond_signal(&canoe_cond);
                pthread_mutex_unlock(&canoe_mutex);
            pthread_mutex_unlock(&paddle_mutex);
        pthread_mutex_unlock(&shipper_mutex);

        current_time = time(NULL);
    }

    return NULL;
}

int main(int argc, char **argv) {
    srand(time(NULL));
    start_time = time(NULL);

    // Read in time limit as command line argument (Default to zero)
    if (argc > 1) {
        time_limit = atoi(argv[1]);
        if (time_limit < 1) {
            time_limit = 0;
        }
    }

    // Create all threads
    pthread_t thread_paddle;
    pthread_t thread_canoe;
    pthread_t thread_shipper;

    pthread_create(&thread_paddle, NULL, &paddle_function, NULL);
    pthread_create(&thread_canoe, NULL, &canoe_function, NULL);
    pthread_create(&thread_shipper, NULL, &shipper_function, NULL);

    // Join all threads
    pthread_join(thread_paddle, NULL);
    pthread_join(thread_canoe, NULL);
    pthread_join(thread_shipper, NULL);

    // Destroy all mutexes and condition variables
    pthread_mutex_destroy(&paddle_mutex);
    pthread_mutex_destroy(&canoe_mutex);
    pthread_mutex_destroy(&shipper_mutex);
    pthread_cond_destroy(&paddle_cond);
    pthread_cond_destroy(&canoe_cond);
    pthread_cond_destroy(&shipper_cond);

    return EXIT_SUCCESS;
}

// Function to get a random amount of seconds between 2 and 5
int randomTime() {
    return (rand() % (5 - 2 + 1)) + 2;
}
