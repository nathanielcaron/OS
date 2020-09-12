#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Assignment 1
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

// A structure to represent a process
typedef struct Process {
    char user[50];
    char process;
    int arrival;
    int duration;
}Process;

// A structure to represent the second output table
typedef struct Table2 {
    char user[50];
    int finish_time;
} Table2;

int getProcesses(struct Process processes[250]) {
    int max_len = 250;
    char line[250] = {0};
    int process_count = 0;

    // Ignore the first line (header)
    if (fgets(line, max_len, stdin) == NULL) {
        printf("No input provided!\n");
        return EXIT_FAILURE;
    }

    while (fgets(line, max_len, stdin) != NULL) {

        int token_num = 0;

        // Tokenize each line
        for (char *token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 0) {
                strcpy(processes[process_count].user, token);
            }
            else if (token_num == 1) {
                processes[process_count].process = *token;
            }
            else if (token_num == 2) {
                processes[process_count].arrival = atoi(token);
            }
            else {
                processes[process_count].duration = atoi(token);
            }
            token_num++;
        }
        process_count++;
    }

    printf("\nProcesses:\n");
    for (int i = 0; i < process_count; i++) {
        printf("%s\t%c\t%d\t%d\n", processes[i].user, processes[i].process, processes[i].arrival, processes[i].duration);
    }

    return process_count;
}

int main(int argc, char **argv) {

    struct Process processes[250];
    
    // Get the processes from the input file
    int process_count = getProcesses(processes);

    // Define table 2
    // struct Table2 table2[process_count];

    // Determine the total processing time
    int total_time = processes[0].arrival;
    for (int i = 0; i < process_count; i++) {
        total_time += processes[i].duration;
    }

    int earliest_arrival = processes[0].arrival;

    printf("\nTimes:\n");
    printf("earliest time: %d\n", earliest_arrival);
    printf("total time: %d\n", total_time);

    printf("\nOutput:\n");
    for (int i = earliest_arrival; i <= total_time; i++) {
        if (i == total_time) {
            printf("%d\tIDLE\n", i);
        } else {
            printf("%d\tA\n", i);
        }
    }

    return EXIT_SUCCESS;
}