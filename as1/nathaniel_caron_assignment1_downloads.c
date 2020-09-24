#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Assignment 1 - Preemptive Shortest Job First (SJF)
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

// Structure to represent a process
typedef struct Process {
    char user[50];
    char process[50];
    int arrival;
    int duration;
} Process;

// Structure to represent the summary table
typedef struct Summary {
    char user[50];
    int finish_time;
} Summary;

// Function templates
void initProcesses(Process **processes, size_t size);
void reallocateProcesses(Process **processes, size_t *size);
int getProcesses(Process **processes, size_t *processes_size);

int main(int argc, char **argv) {

    // Initialize processes array
    Process *processes;
    size_t processes_size = 500;
    initProcesses(&processes, processes_size);

    int process_count = 0;
    int arrived_process_count = 0;

    // For loop variables
    int i = 0;
    int j = 0;
    
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

    printf("Time\tJob\n");

    int is_done = 0;
    while (is_done == 0) {

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

        // Determine which of the arrived processes should be executed
        for (i = 0; i < arrived_process_count; i++) {
            if (processes[process_to_execute_index].duration == 0) {
                process_to_execute_index = i;
            } else if (processes[i].duration != 0 && processes[i].duration < processes[process_to_execute_index].duration) {
                process_to_execute_index = i;
            }
        }

        // Execute the process
        if (processes[process_to_execute_index].duration > 0) {
            printf("%d\t%s\n", time, processes[process_to_execute_index].process);
            (processes[process_to_execute_index].duration)--;

            // Check if process is done, if it is, enter in summary table
            if (processes[process_to_execute_index].duration == 0) {
                strcpy(current_user, processes[process_to_execute_index].user);
                for (i = 0; i < user_count; i++) {
                    if (strcmp(current_user, summary_table[i].user) == 0) {
                        summary_table[i].finish_time = time+1;
                    }
                }
            }
        } else {
            printf("%d\tIDLE\n", time);
        }

        time++;
    }

    // Print summary table
    printf("\nSummary\n");
    for (i = 0; i < user_count; i++) {
        printf("%s\t%d\n", summary_table[i].user, summary_table[i].finish_time);
    }

    return EXIT_SUCCESS;
}

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
            } else {
                (*processes)[process_count].duration = atoi(token);
            }
            token_num++;
        }
        process_count++;
    }

    return process_count;
}
