#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/**
 * Assignment 4 - Producer Consumer
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

// Function templates
int randomTime();

// Global variables
int time_limit = 0;
time_t start_time = 0;
time_t current_time = 0;
int paddles = 0;
int canoes = 0;
static pthread_mutex_t paddle_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t canoe_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to be executed by the paddle thread
void *paddle_function(void *arg) {
    int random_wait_time = 0;
    current_time = time(NULL);

    while ((current_time - start_time) < time_limit) {
        if (paddles < 2) {
            random_wait_time = randomTime();
            sleep(random_wait_time);
            pthread_mutex_lock(&paddle_mutex);
                paddles++;
                printf("We have a paddle.\n");
            pthread_mutex_unlock(&paddle_mutex);
        }
        current_time = time(NULL);
    }
}

// Function to be executed by the canoe thread
void *canoe_function(void *arg) {
    int random_wait_time = 0;
    current_time = time(NULL);

    while ((current_time - start_time) < time_limit) {
        if (canoes < 1) {
            random_wait_time = randomTime();
            sleep(random_wait_time);
            pthread_mutex_lock(&canoe_mutex);
                canoes++;
                printf("We have a canoe.\n");
            pthread_mutex_unlock(&canoe_mutex);
        }
        current_time = time(NULL);
    }
}

// Function to be executed by the shipper thread
void *shipper_function(void *arg) {
    current_time = time(NULL);

    while ((current_time - start_time) <= time_limit || (paddles == 2 && canoes == 1)) {
        if (paddles == 2 && canoes == 1) {
            pthread_mutex_lock(&paddle_mutex);
                pthread_mutex_lock(&canoe_mutex);
                    paddles -= 2;
                    canoes--;
                    printf("We now have a shipment!\n");
                pthread_mutex_unlock(&canoe_mutex);
            pthread_mutex_unlock(&paddle_mutex);
        }
        current_time = time(NULL);
    }
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

    pthread_mutex_destroy(&paddle_mutex);
    pthread_mutex_destroy(&canoe_mutex);

    return EXIT_SUCCESS;
}

// Function to get a random amount of seconds between 2 and 5
int randomTime() {
    return (rand() % (5 - 2 + 1)) + 2;
}
