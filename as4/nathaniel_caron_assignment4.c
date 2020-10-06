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

void *paddle_function(void *arg) {
    int random_time = 0;
    current_time = time(NULL);
    while ((current_time - start_time) < time_limit) {
        if (paddles > 2) {
            random_time = randomTime();
            sleep(random_time);
            pthread_mutex_lock(&paddle_mutex);
                paddles++;
            pthread_mutex_unlock(&paddle_mutex);
            printf("We have a paddle.\n");
        }

        printf("paddles function with time %d\n", random_time);
        current_time = time(NULL);
    }
}

void *canoe_function(void *arg) {

    pthread_mutex_lock(&canoe_mutex);
    pthread_mutex_unlock(&canoe_mutex);
}

void *shipper_function(void *arg) {
    
    pthread_mutex_lock(&paddle_mutex);
    pthread_mutex_unlock(&paddle_mutex);
    pthread_mutex_lock(&canoe_mutex);
    pthread_mutex_unlock(&canoe_mutex);
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

    printf("time limit: %d\n", time_limit);

    sleep(1);

    current_time = time(NULL);

    printf("time difference: %ld\n", current_time - start_time);

    printf("Random time: %d s\n", randomTime());
    printf("Random time: %d s\n", randomTime());
    printf("Random time: %d s\n", randomTime());
    printf("Random time: %d s\n", randomTime());
    printf("Random time: %d s\n", randomTime());

    // Create and join all threads
    pthread_t thread_paddle;
    pthread_t thread_canoe;
    pthread_t thread_shipper;

    pthread_create(&thread_paddle, NULL, &paddle_function, NULL);
    pthread_create(&thread_canoe, NULL, &canoe_function, NULL);
    pthread_create(&thread_shipper, NULL, &shipper_function, NULL);

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
