#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Assignment 6 - Memory Allocation Algorithms
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

int total_memory = 0;

int main(int argc, char **argv) {
    int i = 0;
    char algorithm = 'f';

    char *line = NULL;
    size_t size = 0;

    // Read in total memory and algorithm from cmd line arguments
    if (argc > 1) {
        for (i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-s") == 0) {
                total_memory = atoi(argv[i+1]);
            } else if (strcmp(argv[i], "-f") == 0) {
                algorithm = 'f';
            } else if (strcmp(argv[i], "-b") == 0) {
                algorithm = 'b';
            } else if (strcmp(argv[i], "-w") == 0) {
                algorithm = 'w';
            }
        }
        if (total_memory < 1) {
            total_memory = 0;
        }
    }

    // REMOVE THIS
    printf("total memory: %d\n", total_memory);
    printf("Algorithm: %c\n", algorithm);

    while (1) {
        printf("Entered while loop\n");
        getline(&line, &size, stdin);
        printf("Operation: %s\n", line);
        // Perform action

        // Allocate memory
        if (line[0] == 'N') {
            printf("Allocating memory\n");
        }

        // Deallocate memory
        if (line[0] == 'T') {
            printf("Deallocating memory\n");
        }

        // Stop program
        if (strcmp(line, "S\n") == 0) {
            break;
        }
    }

    // Print report here

    free(line);

    return EXIT_SUCCESS;
}