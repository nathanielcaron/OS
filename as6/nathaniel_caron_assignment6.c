#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Assignment 6 - Memory Allocation Algorithms
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

// Structure for Memory Block Node
typedef struct Node {
    int process;
	int start;
    int end;
    int block_size;
    int free;
    struct Node* previous;
	struct Node* next;
} Node;

// Function templates
int first_fit(Node *current, int process, int size);
int best_fit(Node *current, int process, int size);
int worst_fit(Node *current, int process, int size);
int allocate_process(Node **node, int process, int size);
int deallocate_process(Node **current, int process);
Node* createNode(int process, int start, int end, int block_size);

int main(int argc, char **argv) {
    int total_memory = 0;
    int block_count = 0;
    
    int result = 0;

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

    // NODE TEST
    Node *node1 = createNode(1, 0, 500, 500);
    Node *node2 = createNode(2, 500, 750, 250);
    Node *node3 = createNode(3, 750, 1000, 250);

    node1->next = node2;
    node2->previous = node1;
    node2->next = node3;
    node3->previous = node2;

    Node *current = node1;

    printf("--- --- ---\n");
    while (current != NULL) {
        printf("Node %d, start %d, end %d, block size %d\n", current->process, current->start, current->end, current->block_size);
        current = current->next;
    }
    printf("--- --- ---\n");

    Node *head = createNode(-1, 0, total_memory, total_memory);
    printf("--- --- ---\n");
    printf("Node %d, start %d, end %d, block size %d\n", head->process, head->start, head->end, head->block_size);
    printf("--- --- ---\n");


    // Listen for user operations
    while (1) {
        printf("Entered while loop\n");
        getline(&line, &size, stdin);
        printf("Operation: %s\n", line);

        // Allocate memory
        if (line[0] == 'N') {
            printf("Allocating memory\n");
            line++;
            char *token;
            // Get process number
            token = strtok(line, " \t");
            int process = atoi(token);
            printf("process: %d\n", process);
            // Get process size
            token = token = strtok(NULL, " \t");
            int size = atoi(token);
            printf("size: %d\n", size);
            if (algorithm == 'f') {
                result = first_fit(head, process, size);
            } else if (algorithm == 'b') {
                result = best_fit(head, process, size);
            } else if (algorithm == 'w') {
                result = worst_fit(head, process, size);
            }
            // Could not allocate memory
            if (result == -1) {
                printf("Process %d failed to allocate %d memory\n", process, size);
            }

            printf("Result: %d\n", result);
            current = head;
            printf("--- --- ---\n");
            while (current != NULL) {
                printf("Node %d, start %d, end %d, block size %d, free %d\n", current->process, current->start, current->end, current->block_size, current->free);
                current = current->next;
            }
            printf("--- --- ---\n");
        }

        // Deallocate memory
        else if (line[0] == 'T') {
            printf("Deallocating memory\n");
            line++;
            char *token;
            // Get process number
            token = strtok(line, " \t");
            int process = atoi(token);
            printf("process: %d\n", process);
            result = deallocate_process(&head, process);
            // Could not deallocate memory
            if (result == -1) {
                printf("Process %d failed to free memory\n", process);
            }

            printf("Result: %d\n", result);
            current = head;
            printf("--- --- ---\n");
            while (current != NULL) {
                printf("Node %d, start %d, end %d, block size %d, free %d\n", current->process, current->start, current->end, current->block_size, current->free);
                current = current->next;
            }
            printf("--- --- ---\n");
        }

        // Stop program
        else if (strcmp(line, "S\n") == 0) {
            break;
        }
    }

    // Print report here

    return EXIT_SUCCESS;
}

// First Fit Algorithm
int first_fit(Node *current, int process, int size) {
    printf("Entered first fit\n");
    int return_value;
    if (current->free == 1 && current->block_size >= size) {
        return_value = allocate_process(&current, process, size);
        printf("Returned from allocate process\n");
        return return_value;
    }
    while (current->next != NULL) {
        current = current->next;
        if (current->free == 1 && current->block_size >= size) {
            return_value = allocate_process(&current, process, size);
            return return_value;
        }
    }
    return -1;
}

// Best Fit Algorithm
int best_fit(Node *current, int process, int size) {
    printf("Entered best fit\n");
    int return_value;
    Node *best = current;
    while (current->next != NULL) {
        current = current->next;
        // Find the smallest available block that fits the process
        if (current->free == 1 && current->block_size >= size && current->block_size < best->block_size) {
            best = current;
        }
    }

    if (best->free != 1) {
        return -1;
    } else {
        return_value = allocate_process(&best, process, size);
        return return_value;
    }
}

// Worst Fit Algorithm
int worst_fit(Node *current, int process, int size) {
    printf("Entered worst fit\n");
    int return_value;
    Node *worst = current;
    while (current->next != NULL) {
        current = current->next;
        // Find the largest available block that fits the process
        if (current->free == 1 && current->block_size >= size && current->block_size > worst->block_size) {
            worst = current;
        }
    }

    if (worst->free != 1) {
        return -1;
    } else {
        return_value = allocate_process(&worst, process, size);
        return return_value;
    }
}

int allocate_process(Node **node, int process, int size) {
    printf("Entered allocate process\n");
        if (size < (*node)->block_size) {
        int new_node_end = (*node)->end;
        (*node)->block_size = size;
        (*node)->end = (*node)->start + size;
        (*node)->free = 0;
        (*node)->process = process;
        int new_node_start = (*node)->end;
        int new_node_size = new_node_end - new_node_start;
        printf("new node start %d\n", new_node_start);
        printf("new node end %d\n", new_node_end);
        printf("new node size %d\n", new_node_size);
        Node *new_node = createNode(-1, new_node_start, new_node_end, new_node_size);
        printf("New node was allocated\n");
        if ((*node)->next != NULL) {
            (*node)->next->previous = new_node;
            new_node->next = (*node)->next;
        }
        new_node->previous = (*node);
        (*node)->next = new_node;
        printf("Process was allocated at address %d\n", (*node)->start);
        return (*node)->start;
    } else if (size == (*node)->block_size) {
        (*node)->free = 0;
        (*node)->process = process;
        printf("Process was allocated at address %d\n", (*node)->start);
        return (*node)->start;
    }
}

int deallocate_process(Node **node, int process) {
    Node *current = (*node);
    while (current != NULL) {
        if (current->process == process) {
            current->free = 1;
            current->process = -1;
            // Check if two free blocks need to be merged
            if (current->next != NULL) {
                if (current->next->free == 1) {
                    current->end = current->next->end;
                    current->block_size += current->next->block_size;
                    printf("current new end %d\n", current->end);
                    printf("current new size %d\n", current->block_size);
                    Node *temp = current->next->next;
                    free(current->next);
                    current->next = temp;
                    if (temp != NULL) {
                        temp->previous = current;
                    }
                }
            } else if (current->previous != NULL) {
                if (current->previous->free == 1) {
                    current->previous->end = current->end;
                    current->previous->block_size += current->block_size;
                    printf("previous new end %d\n", current->previous->end);
                    printf("previous new size %d\n", current->previous->block_size);
                    Node *temp = current->next;
                    current->previous->next = temp;
                    if (temp != NULL) {
                        temp->previous = current->previous;
                    }
                    printf("Freeing node %d\n", current->process);
                    free(current);
                }
            }
            return 1;
        }
        current = current->next;
    }

    return -1;
}

// Function to create a memory block node
Node* createNode(int process, int start, int end, int block_size) {
	Node* node = (Node*)malloc(sizeof(Node));
    node->process = process;
	node->start = start;
    node->end = end;
    node->block_size = block_size;
    node->free = 1;
    node->previous = NULL;
	node->next = NULL;
	return node;
}
