#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/**
 * Lab 2 - Pipes
 * CS 3413
 * Nathaniel Caron
 * 3598979
 * 
 * Solution:
 * Read integers from stdin and write the values to pipes from parent thread
 * From the child / worker threads, read the values from the pipes and perform calculations
 */

// Global variables
int average = 0;
int minimum = 0;
int maximum = 0;

int pipe_average[2];
int pipe_minimum[2];
int pipe_maximum[2];

// Function to be executed by average thread
void* find_average(void *arg) {
    int sum = 0;
    int count = 0;
    int current;

    // Read all integer values from pipe
    while(read(pipe_average[0], &current, sizeof(current)) != 0) {
        sum += current;
        count++;
    }

    if (count != 0) {
        average = sum / count;
    }
    return NULL;
}

// Function to be executed by minimum thread
void* determine_minimum(void *arg) {
    int current;
    int current_min = 0;

    // Read all integer values from pipe
    while(read(pipe_minimum[0], &current, sizeof(current)) != 0) {
        if (current_min == 0) {
            current_min = current;
        } else if (current < current_min) {
            current_min = current;
        }
    }
    
    minimum = current_min;
    return NULL;
}

// Function to be executed by maximum thread
void* determine_maximum(void *arg) {
    int current;
    int current_max = 0;

    // Read all integer values from pipe
    while(read(pipe_maximum[0], &current, sizeof(current)) != 0) {
        if (current_max == 0) {
            current_max = current;
        } else if (current > current_max) {
            current_max = current;
        }
    }

    maximum = current_max;
    return NULL;
}

int main() {
    // Create pipes
    if (pipe(pipe_average) != -1 && pipe(pipe_minimum) != -1 && pipe(pipe_maximum) != -1) {
        // Read in values from stdin until 0
        int current_num = 0;
        scanf("%d", &current_num);
        while(current_num != 0) {
            write(pipe_average[1], &current_num, sizeof(current_num));
            write(pipe_minimum[1], &current_num, sizeof(current_num));
            write(pipe_maximum[1], &current_num, sizeof(current_num));

            scanf("%d", &current_num);
        }

        // Close the writing end of pipes
        close(pipe_average[1]);
        close(pipe_minimum[1]);
        close(pipe_maximum[1]);

        // Create and join all threads
        pthread_t thread_average;
        pthread_t thread_minimum;
        pthread_t thread_maximum;

        pthread_create(&thread_average, NULL, &find_average, NULL);
        pthread_create(&thread_minimum, NULL, &determine_minimum, NULL);
        pthread_create(&thread_maximum, NULL, &determine_maximum, NULL);

        pthread_join(thread_average, NULL);
        pthread_join(thread_minimum, NULL);
        pthread_join(thread_maximum, NULL);

        // Close the read end of pipes
        close(pipe_average[0]);
        close(pipe_minimum[0]);
        close(pipe_maximum[0]);

        // Print output from parent thread
        printf("\nThe average value is %d\n", average);
        printf("The minimum value is %d\n", minimum);
        printf("The maximum value is %d\n", maximum);

        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}