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

/**
 * getProcesses() is the function used to read in the processes from the input
 * The parameter is where the processes will be stored
 */
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
    int process_count = 0;
    int arrived_prosess_count = 0;

    char current_user[50] = {0};
    
    // Get the processes from the input file
    process_count = getProcesses(processes);

    // Define second ouput table and determine the total processing time
    struct Table2 table2[process_count];
    int user_count = 0;
    int total_time = processes[0].arrival;
    for (int i = 0; i < process_count; i++) {
        table2[i].finish_time = -1;
        total_time += processes[i].duration;
    }

    int earliest_arrival = processes[0].arrival;

    printf("\nTimes:\n");
    printf("earliest time: %d\n", earliest_arrival);
    printf("total time: %d\n", total_time);

    // This is where the output is produced
    printf("\nOutput:\n");

    int process_to_execute_index = 0;

    for (int time = earliest_arrival; time <= total_time; time++) {
        if (time == total_time) {
            printf("%d\tIDLE\n", time);
        } else {
            printf("%d\t", time);

            // Determine which processes are arrived
            for (int i = 0; i < process_count; i++) {
                if (processes[i].arrival == time) {
                    arrived_prosess_count++;
                }
            }
            printf("Arrived processes: %d\t", arrived_prosess_count);

            // Determine which of the arrived processes should be executed
            for (int i = 0; i < arrived_prosess_count; i++) {
                if (processes[process_to_execute_index].duration == 0) {
                    process_to_execute_index = i;
                }
                else if (processes[i].duration != 0 && processes[i].duration < processes[process_to_execute_index].duration) {
                    process_to_execute_index = i;
                }
            }

            // Execute the process
            if (processes[process_to_execute_index].duration > 0) {
                printf("%c\t%d\n", processes[process_to_execute_index].process, processes[process_to_execute_index].duration);
                (processes[process_to_execute_index].duration)--;
                // Process is done
                if (processes[process_to_execute_index].duration == 0) {
                    strcpy(current_user, processes[process_to_execute_index].user);
                    for (int i = 0; i < user_count; i++) {
                        // User already in table
                        if (strcmp(current_user, table2[i].user) == 0){
                            table2[i].finish_time = time+1;
                        }
                    }
                    // User not in table
                    strcpy(table2[user_count].user, current_user);
                    table2[user_count].finish_time = time+1;
                    user_count++;
                }
            }
        }
    }

    printf("\nSummary\n");
    // Print second table
    for (int i = 0; i < user_count; i++) {
        printf("%s\t%d\n", table2[i].user, table2[i].finish_time);
    }

    return EXIT_SUCCESS;
}

// Have arrived processes array with arrived processes counter
// Add process to arrived processes array when current time >= arrival time
// Execute the arrived process with the lowest duration (break duration ties with arrival time)
// If same duration and same arrival time, then use order from input

// Cases to consider:
// Sample input / ouput provided
// two processes with same duration and same arrival time
// single process in input file
// No process
// More complex input file

// For second ouput table, ignore entries with finish_time = -1
// When process hits zero, look at user and enter it in table2 along with current time

// TODO: Fix duplicate in summary table
//       Fix order in summary table