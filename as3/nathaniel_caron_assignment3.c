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

// Structure to represent a process
typedef struct Process {
    char *user;
    char *process;
    int arrival;
    int duration;
    int deadline;
    int running;
} Process;

// Structure to represent the summary table
typedef struct Summary {
    char *user;
    int finish_time;
} Summary;

// Structure for priority queue node
typedef struct Node {
	int process_position;
    Process *process;
	struct Node* next;
} Node;

// Function templates
void initProcesses(Process **processes, size_t *size);
void reallocateProcesses(Process **processes, size_t *size);
int getProcesses(Process **processes, size_t *processes_size);
Node* newNode(int process_position, Process *process);
void removeNode(Node **head);
int compareNodePriority(Node *node1, Node *node2);
void insertNode(Node **head, Node *node);


int process_count = 0;
int earliest_arrival = 0;
int processor_count = 1;
int missed_deadlines = 0;
int assigned_processes = 0;
int processed_processes = 0;
Node *head = NULL;
Summary *summary_table;
int user_count = 0;
pthread_mutex_t mutex_assign = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_process = PTHREAD_MUTEX_INITIALIZER;

void* execute(void* processes_ptr) {
    int i = 0;
    int time = earliest_arrival;
    Process *processes = (Process *)processes_ptr;

    int is_done = 0;

    while (is_done != 1) {

        // Check if all processes are complete
        is_done = 1;
        for (i = 0; i < process_count; i++) {
            if (processes[i].duration > 0) {
                is_done = 0;
                break;
            }
        }

        // Assign process
        pthread_mutex_lock(&mutex_assign);
            while (processed_processes != 0) {}
            if (assigned_processes == 0) {
                if (time == earliest_arrival) {
                    // Print output header
                    printf("Time");
                    for (i = 0; i < processor_count; i++) {
                        printf("\tCPU%d", i+1);
                    }
                    printf("\n");
                }
                // This is the first thread to assign a process
                // printf("First thread to assign a process\n");

                // Determine which processes have arrived
                for (i = 0; i < process_count; i++) {
                    if (processes[i].arrival == time) {
                        // printf("Inserting node into run queue\n");
                        Node *node = newNode(i, &processes[i]);
                        insertNode(&head, node);
                    }
                }

                // printf("\nRun Queue\n");
                // printf("--- --- --- ---\n");
                // if (head != NULL) {
                //     Node *current = head;
                //     while (current != NULL) {
                //         printf("%s %d -> ", current->process->process, current->process->duration);
                //         current = current->next;
                //     }
                // }
                // printf("\n--- --- --- ---\n");
            }

            Node *current_process = NULL;

            if (assigned_processes == 0) {
                printf("%d", time);
            }

            if (head != NULL) {
                current_process = newNode(head->process_position, head->process);
                removeNode(&head);
                printf("\t%s", current_process->process->process);
                // printf("Current Head: %s\n", head->process->process);
            } else {
                if (is_done == 1 && assigned_processes == 0) {
                    printf("\tIDLE");
                } else if (is_done == 0) {
                    printf("\t-");
                }
            }

            // printf("Thread %d time %d\n", assigned_processes+1, time);
            assigned_processes++;

            if (assigned_processes == processor_count) {
                printf("\n");
            }
        pthread_mutex_unlock(&mutex_assign);

        // Execute process
        pthread_mutex_lock(&mutex_process);
            while (assigned_processes != processor_count) {}
            if (current_process != NULL) {
                (current_process->process->duration)--;
                if (current_process->process->duration == 0) {
                    if (time+1 > current_process->process->deadline) {
                        missed_deadlines++;
                    }
                    for (i = 0; i < user_count; i++) {
                        if (strcmp(current_process->process->user, summary_table[i].user) == 0) {
                            summary_table[i].finish_time = time+1;
                        }
                    }
                } else {
                    insertNode(&head, current_process);
                }
            }
            processed_processes++;
            if (processed_processes == processor_count) {
                assigned_processes = 0;
                processed_processes = 0;
            }
        pthread_mutex_unlock(&mutex_process);

        time++;
        sleep(1);
    }

    return NULL;
}

int main(int argc, char **argv) {

    // For loop variables
    int i = 0;
    int j = 0;

    // Read in number of processors as command line argument (Default to single core)
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

    // Get the processes from the input file
    // Initialize processes, VARIABLE SIZE, see reallocateProcesses() function
    Process *processes;
    size_t initial_size;
    initProcesses(&processes, &initial_size);
    process_count = getProcesses(&processes, &initial_size);

    earliest_arrival = processes[0].arrival;

    // Define the summary table
    summary_table = (Summary *)calloc(process_count, sizeof(*summary_table));
    int user_in_table = 0;
    for (i = 0; i < process_count; i++) {
        for (j = 0; j < user_count; j++) {
            // User already in table
            if (strcmp(processes[i].user, summary_table[j].user) == 0){
                user_in_table = 1;
            }
        }
        if (user_in_table == 0) {
            // User not in table
            summary_table[user_count].user = strdup(processes[i].user);
            user_count++;
        }
        user_in_table = 0;
    }

    // Create and join all threads
    pthread_t threads[processor_count];
    for (i = 0; i < processor_count; i++) {
        pthread_create(&threads[i], NULL, &execute, processes);
    }
    for (i = 0; i < processor_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // for (i = 0; i < process_count; i++) {
    //     Node *node = newNode(i, &processes[i]);
    //     insertNode(&head, node);
    // }

    // if (head != NULL) {
    //     Node *current = head;
    //     printf("Current node assigned\n");
    //     while (current != NULL) {
    //         printf("%s %d -> ", current->process->process, current->process->duration);
    //         current = current->next;
    //     }
    //     printf("\n");
    // }

    // removeNode(&head);

    // if (head != NULL) {
    //     Node *current = head;
    //     printf("Current node assigned\n");
    //     while (current != NULL) {
    //         printf("%s %d -> ", current->process->process, current->process->duration);
    //         current = current->next;
    //     }
    //     printf("\n");
    // }

    // Node *current = newNode(head->process_position, head->process);
    // removeNode(&head);

    // if (head != NULL) {
    //     Node *current = head;
    //     printf("Current node assigned\n");
    //     while (current != NULL) {
    //         printf("%s %d -> ", current->process->process, current->process->duration);
    //         current = current->next;
    //     }
    //     printf("\n");
    // }

    // insertNode(&head, current);

    // if (head != NULL) {
    //     Node *current = head;
    //     printf("\n");
    //     while (current != NULL) {
    //         printf("%s %d -> ", current->process->process, current->process->duration);
    //         current = current->next;
    //     }
    //     printf("\n");
    // }

    printf("\nSummary\n");
    for (i = 0; i < user_count; i++) {
        printf("%s\t%d\n", summary_table[i].user, summary_table[i].finish_time);
    }

    printf("\n%d missed deadlines\n", missed_deadlines);

    pthread_mutex_destroy(&mutex_assign);
    pthread_mutex_destroy(&mutex_process);

    return EXIT_SUCCESS;
}

// Function to initialize the processes array (initial size expanded as needed)
void initProcesses(Process **processes, size_t *size) {
    *size = 500;
    *processes = (Process*)calloc(*size, sizeof(Process));
}

// Function to reallocate space for the processes (VARIABLE SIZE)
void reallocateProcesses(Process **processes, size_t *size) {
    *size *= 2;
    *processes = (Process*)realloc(*processes, (*size) * sizeof(Process));
}

// Function used to read in the processes from the input
int getProcesses(Process **processes, size_t *processes_size) {
    int line_len = 1000;
    char line[1000] = {0};

    size_t process_count = 0;

    // Ignore input header
    fgets(line, line_len, stdin);

    while (fgets(line, line_len, stdin) != NULL) {
        int token_num = 0;

        // Reallocate more space for processes if needed (VARIABLE SIZE)
        if (process_count == *processes_size) {
            reallocateProcesses(processes, processes_size);
        }

        // Tokenize each line
        char *token;
        for (token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 0) {
                (*processes)[process_count].user = strdup(token);
            } else if (token_num == 1) {
                (*processes)[process_count].process = strdup(token);
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

    // REMOVE THIS
    // printf("\nProcesses\n");
    // printf("--- --- --- ---\n");
    // printf("User\tProcess\tArrival\tDuration\tDeadline\n");
    // for (int i = 0; i < process_count; i++) {
    //     printf("%s\t%s\t%d\t%d\t%d\n", (*processes)[i].user, (*processes)[i].process, (*processes)[i].arrival, (*processes)[i].duration, (*processes)[i].deadline);
    // }
    // printf("--- --- --- ---\n");

    return process_count;
}

Node* newNode(int process_position, Process *process) {
    // printf("Creating the node\n");
	Node* node = (Node*)calloc(1, sizeof(Node));
	node->process_position = process_position;
    node->process = process;
	node->next = NULL;
	return node;
}

void removeNode(Node **head) {
    if ((*head) != NULL) {
        // printf("Removing node %s\n", (*head)->process->process);
        Node *temp = (*head);
        (*head) = (*head)->next;
        free(temp);
    }
}

int compareNodePriority(Node *node1, Node *node2) {
    if ((node2->process->deadline < node1->process->deadline) ||
        (node2->process->deadline == node1->process->deadline && node2->process->duration < node1->process->duration) ||
        (node2->process->deadline == node1->process->deadline && node2->process->duration == node1->process->duration && node2->process->arrival < node1->process->arrival) ||
        (node2->process->deadline == node1->process->deadline && node2->process->duration == node1->process->duration && node2->process->arrival < node1->process->arrival && node2->process_position < node1->process_position)) {
        return 2;
    } else {
        return 1;
    }
}

void insertNode(Node **head, Node *node) {
    // printf("Inserting the node\n");
    if ((*head) == NULL) {
        // Empty queue
        (*head) = node;
    } else {
        Node *start = (*head);
        if (compareNodePriority((*head), node) == 2) {
            // Insert before head
            node->next = (*head);
            (*head) = node;
        } else {
            // Insert in list
            while (start->next != NULL && compareNodePriority(start->next, node) == 1) {
                start = start->next;
            }
            node->next = start->next;
            start->next = node;
        }
    }
}