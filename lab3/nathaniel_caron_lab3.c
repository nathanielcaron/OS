#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/**
 * Lab 3 - Semaphores
 * CS 3413
 * Nathaniel Caron
 * 3598979
 * 
 * QUESTION:
 * Experiment with the program by changing the (relative) production rate and the consumption
 * rate (change sleep times n and m) to achieve a processes alternation. Explain the results
 * ANSWER:
 * When the consumption rate is faster than the production rate, the consumer is constantly waiting for an item to be produced.
 * When the two rates are equal, the process functions well but only a single item is in the buffer at a time.
 * When the production rate is faster than the consumption rate, the consumer will always have items to consume.
 * (Although some items may never be consumed unless the producer stops producing)
 * IMPORTANT: Could have set a time limit for the producer to stop producing, consume remaining items and terminate.
 */

int n = 0;
int m = 0;
int buffer[10] = {0};
int counter = 0;
sem_t sem_mutex;
sem_t sem_full;
sem_t sem_empty;

// Function to get a random number between 1 and 10
int randomNum() {
    return (rand() % (10 - 1 + 1)) + 1;
}

// Function to be executed by the child process (Consumer)
void *consume(void *arg) {
    int current_item = 0;
    
    // Consume
    while(1) {
        sem_wait(&sem_full);
        sem_wait(&sem_mutex);
            // Remove item from buffer
            counter--;
            current_item = buffer[counter];
        sem_post(&sem_mutex);
        sem_post(&sem_empty);

        // Consume item
        printf("Consumed %d\n", current_item);

        sleep(m);
    }
}

int main(int argc, char **argv) {
    // Read in n and m sleep values for producer and consumer
    printf("Enter a vlue for n: ");
    scanf("%d", &n);
    printf("Enter a vlue for m: ");
    scanf("%d", &m);
    printf("Parent will sleep for %d seconds, child for %d seconds\n", n, m);

    // Initialize semaphores
    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_full, 0, 0);
    sem_init(&sem_empty, 0, 10);

    // Create child process
    pthread_t child_process;
    pthread_create(&child_process, NULL, &consume, NULL);

    srand(time(0));
    int random_num = 0;

    // Produce
    while(1) {

        // Produce item
        random_num = randomNum();
        printf("Producing %d\n", random_num);

        sem_wait(&sem_empty);
        sem_wait(&sem_mutex);
            // Remove item from buffer
            buffer[counter] = random_num;
            counter++;
        sem_post(&sem_mutex);
        sem_post(&sem_full);

        sleep(n);
    }

    pthread_join(child_process, NULL);

    return EXIT_SUCCESS;
}