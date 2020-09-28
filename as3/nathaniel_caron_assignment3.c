#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/**
 * Assignment 3 - Pthread Earliest Deadline First (EDF)
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

static int missed_deadlines = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* execute(void* processor_number_ptr) {
    int *processor_number = processor_number_ptr;

    int time = 0;

    pthread_mutex_lock(&mutex);
    missed_deadlines++;
    pthread_mutex_unlock(&mutex);

    // Test for timing
    for (time = 0; time < 3; time++) {
        printf("Thread %d time %d\n", *processor_number, time);
        sleep(1);
    }

    return NULL;
}

int main(int argc, char **argv) {

    // For loop variables
    int i = 0;

    // Read in number of processors as command line argument (Default to single core)
    int processor_count = 1;
    if (argc > 1) {
        processor_count = atoi(argv[1]);
        if (processor_count < 1) {
            processor_count = 1;
        }
    }

    // Allocate memory for processor numbers
    int *processor_numbers = malloc(processor_count * sizeof(*processor_numbers));
    for (i = 0; i < processor_count; i++) {
        processor_numbers[i] = i+1;
    }

    // Read processes from stdin

    // Create and join all threads
    pthread_t threads[processor_count];
    for (i = 0; i < processor_count; i++) {
        pthread_create(&threads[i], NULL, &execute, &processor_numbers[i]);
    }
    for (i = 0; i < processor_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Sum: %d\n", missed_deadlines);

    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}