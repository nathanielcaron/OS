#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Assignment 2 - Multiprocessor, Earliest Deadline First (EDF)
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

// Structure to represent a process
typedef struct Process {
    char user[100];
    char process[100];
    int arrival;
    int duration;
    int deadline;
    int running;
} Process;

// Structure to represent the summary table
typedef struct Summary {
    char user[100];
    int finish_time;
} Summary;

// Function templates
// int isProcessExecuting(int processor_num, char (*processors)[100], char *process);
void initProcesses(Process **processes, size_t size);
void reallocateProcesses(Process **processes, size_t *size);
int getProcesses(Process **processes, size_t *processes_size);

int main(int argc, char **argv) {

    // For loop variables
    int i = 0;
    int j = 0;

    // Read in n command line argument
    /*
     *  TODO: Add more error handling for cmd line argument
     */
    int processor_count = 1;
    if (argc > 1) {
        processor_count = atoi(argv[1]);
    }
    if (processor_count < 1) {
        processor_count = 1;
    }
    // Remove this
    // printf("processor_count: %d\n", processor_count);
    // Define array of processors
    // char processors[processor_num][100];
    // for (i = 0; i < processor_num; i++) {
    //     strcpy(processors[i], "");
    // }

    // Initialize processes array
    Process *processes;
    size_t processes_size = 500;
    initProcesses(&processes, processes_size);

    int process_count = 0;
    int arrived_process_count = 0;
    
    // Get the processes from the input file
    process_count = getProcesses(&processes, &processes_size);

    int earliest_arrival = processes[0].arrival;

    // Define the summary table
    Summary summary_table[process_count];
    int user_count = 0;
    char current_user[50] = {0};
    int user_in_table = 0;
    for (i = 0; i < process_count; i++) {
        strcpy(current_user, processes[i].user);
        for (j = 0; j < user_count; j++) {
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

    int process_to_execute_index = 0;
    int time = earliest_arrival;

    // Print output header
    printf("Time");
    for (i = 0; i < processor_count; i++) {
        printf("\tCPU%d", i+1);
    }
    printf("\n");

    int is_done = 0;
    while (is_done == 0) {

        // Reset processors array
        // for (i = 0; i < processor_num; i++) {
        //     strcpy(processors[i], "");
        // }
        for (i = 0; i < process_count; i++) {
            processes[i].running = 0;
        }

        // Check if all processes are complete
        is_done = 1;
        for (i = 0; i < process_count; i++) {
            if (processes[i].duration > 0) {
                is_done = 0;
                break;
            }
        }

        // Determine which processes have arrived
        for (i = 0; i < process_count; i++) {
            if (processes[i].arrival == time) {
                arrived_process_count++;
            }
        }

        printf("%d", time);

        for (i = 0; i < processor_count; i++) {
            // Determine which of the arrived processes should be executed
            for (j = 0; j < arrived_process_count; j++) {
                if (processes[process_to_execute_index].duration == 0 || processes[process_to_execute_index].running == 1) {
                    process_to_execute_index = j;
                } else if (processes[j].duration != 0 && processes[j].running != 1 && processes[j].deadline < processes[process_to_execute_index].deadline) {
                    process_to_execute_index = j;
                } else if (processes[j].duration != 0 && processes[j].running != 1 && processes[j].deadline == processes[process_to_execute_index].deadline && processes[j].duration < processes[process_to_execute_index].duration) {
                    process_to_execute_index = j;
                }
            }

            // Execute the process
            if (processes[process_to_execute_index].duration > 0 && processes[process_to_execute_index].running != 1) {
                printf("\t%s", processes[process_to_execute_index].process);
                (processes[process_to_execute_index].duration)--;
                processes[process_to_execute_index].running = 1;

                // Check if process is done, if it is, enter in summary table
                if (processes[process_to_execute_index].duration == 0) {
                    strcpy(current_user, processes[process_to_execute_index].user);
                    for (j = 0; j < user_count; j++) {
                        if (strcmp(current_user, summary_table[j].user) == 0) {
                            summary_table[j].finish_time = time+1;
                        }
                    }
                }
            } else if (is_done == 1 && i == 0) {
                printf("\tIDLE");
            } else if (is_done == 0) {
                printf("\t-");
            }
        }

        printf("\n");

        time++;
    }

    // Print summary table
    printf("\nSummary\n");
    for (i = 0; i < user_count; i++) {
        printf("%s\t%d\n", summary_table[i].user, summary_table[i].finish_time);
    }

    return EXIT_SUCCESS;
}

// int isProcessExecuting(int processor_num, char (*processors)[100], char *process) {
//     int i = 0;
//     for (i = 0; i < processor_num; i++) {
//         if (strcmp(processors[i], process) == 0) {
//             return 0;
//         }
//     }
//     return 1;
// }

// Function to initialize the processes array
void initProcesses(Process **processes, size_t size) {
  *processes = (Process*)calloc(size, sizeof(Process));
}

// Function to allocate more space for the processes array
void reallocateProcesses(Process **processes, size_t *size) {
    *size *= 2;
    *processes = (Process*)realloc(*processes, (*size) * sizeof(Process));
}

// Function used to read in the processes from the input
int getProcesses(Process **processes, size_t *processes_size) {
    int max_len = 500;
    char line[500] = {0};

    size_t process_count = 0;

    // Ignore the first line (header)
    if (fgets(line, max_len, stdin) == NULL) {
        return 0;
    }

    while (fgets(line, max_len, stdin) != NULL) {
        int token_num = 0;

        if (process_count == *processes_size) {
            reallocateProcesses(processes, processes_size);
        }

        // Tokenize each line
        char *token;
        for (token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 0) {
                strcpy((*processes)[process_count].user, token);
            } else if (token_num == 1) {
                strcpy((*processes)[process_count].process, token);
            } else if (token_num == 2) {
                (*processes)[process_count].arrival = atoi(token);
            } else if (token_num == 3) {
                (*processes)[process_count].duration = atoi(token);
            } else if (token_num == 4) {
                (*processes)[process_count].deadline = atoi(token);
            }
            token_num++;
        }
        process_count++;
    }

    return process_count;
}