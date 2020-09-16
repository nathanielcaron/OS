#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Assignment 1 - Preemptive Shortest Job First
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
} Process;

// A structure to represent the summary table
typedef struct Summary {
    char user[50];
    int finish_time;
} Summary;

// Function used to read in the processes from the input
int getProcesses(struct Process processes[250]) {
    int max_len = 250;
    char line[250] = {0};
    int process_count = 0;

    // Ignore the first line (header)
    if (fgets(line, max_len, stdin) == NULL) {
        return 0;
    }

    while (fgets(line, max_len, stdin) != NULL) {
        int token_num = 0;

        // Tokenize each line
        for (char *token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 0) {
                strcpy(processes[process_count].user, token);
            } else if (token_num == 1) {
                processes[process_count].process = *token;
            } else if (token_num == 2) {
                processes[process_count].arrival = atoi(token);
            } else {
                processes[process_count].duration = atoi(token);
            }
            token_num++;
        }
        process_count++;
    }

    return process_count;
}

int main(int argc, char **argv) {

    Process processes[250];
    int process_count = 0;
    int arrived_process_count = 0;
    
    // Get the processes from the input file
    process_count = getProcesses(processes);

    if (process_count > 0) {
        // Determine the total processing time
        int total_time = 0;
        int first_arrival_index = 0;
        for (int i = 0; i < process_count; i++) {
            if (processes[i].arrival < processes[first_arrival_index].duration) {
                first_arrival_index = i;
            }
            total_time += processes[i].duration;
        }
        total_time += processes[first_arrival_index].arrival;

        // Define the summary table
        Summary summary_table[process_count];
        int user_count = 0;
        char current_user[50] = {0};
        int user_in_table = 0;
        for (int i = 0; i < process_count; i++) {
            strcpy(current_user, processes[i].user);
            for (int j = 0; j < user_count; j++) {
                // User already in table
                if (strcmp(current_user, summary_table[j].user) == 0){
                    user_in_table = 1;
                }
            }
            if (user_in_table == 0) {
                // User not in table
                strcpy(summary_table[user_count].user, current_user);
                user_count++;
            }
            user_in_table = 0;
        }

        int earliest_arrival = processes[first_arrival_index].arrival;
        int process_to_execute_index = 0;

        printf("Time\tJob\n");
        for (int time = earliest_arrival; time <= total_time; time++) {
            if (time == total_time) {
                printf("%d\tIDLE\n", time);
            } else {
                printf("%d\t", time);

                // Determine which processes are arrived
                for (int i = 0; i < process_count; i++) {
                    if (processes[i].arrival == time) {
                        arrived_process_count++;
                    }
                }

                // Determine which of the arrived processes should be executed
                for (int i = 0; i < arrived_process_count; i++) {
                    if (processes[process_to_execute_index].duration == 0) {
                        process_to_execute_index = i;
                    }
                    else if (processes[i].duration != 0 && processes[i].duration < processes[process_to_execute_index].duration) {
                        process_to_execute_index = i;
                    }
                }

                // Execute the process
                if (processes[process_to_execute_index].duration > 0) {
                    printf("%c\n", processes[process_to_execute_index].process);
                    (processes[process_to_execute_index].duration)--;

                    // Check if process is done, if it is, enter in summary table
                    if (processes[process_to_execute_index].duration == 0) {
                        strcpy(current_user, processes[process_to_execute_index].user);
                        for (int i = 0; i < user_count; i++) {
                            if (strcmp(current_user, summary_table[i].user) == 0) {
                                summary_table[i].finish_time = time+1;
                            }
                        }
                    }
                }
            }
        }

        // Print summary table
        printf("\nSummary\n");
        for (int i = 0; i < user_count; i++) {
            printf("%s\t%d\n", summary_table[i].user, summary_table[i].finish_time);
        }
    }

    return EXIT_SUCCESS;
}