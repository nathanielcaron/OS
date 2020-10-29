#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Assignment 6 - Memory Allocation Algorithms
 * CS 3413
 * Nathaniel Caron
 * 3598979
 * 
 * Solution:
 * Uses a linked list to represent memory blocks
 * IMPORTANT:
 * Final Smallest Fragmented Memory Size and Final Largest Fragmented Memory Size --> Considers all memory blocks (Free or not)
 * Total Processes Created, Total Allocated Memory, Total Processes Terminated, Total Freed Memory --> For entire program lifecycle
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

// Final Report Variables
int total_processes_created = 0;
int total_allocated_memory = 0;
int total_processes_terminated = 0;
int total_freed_memory = 0;
int final_memory_available = 0;
int final_smallest_fragment = 0;
int final_largest_fragment = 0;

int main(int argc, char **argv) {
    // Initialize variables
    int i = 0;
    int total_memory = 0;
    int result = 0;
    char algorithm = 'f';
    char *line = NULL;
    size_t size = 0;

    // Read in Total Memory and Memory Allocation Algorithm from cmd line arguments
    if (argc > 1) {
        for (i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-s") == 0) {
                total_memory = atoi(argv[i+1]);
            } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "-F") == 0) {
                algorithm = 'f';
            } else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "-B") == 0) {
                algorithm = 'b';
            } else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "-W") == 0) {
                algorithm = 'w';
            }
        }
        if (total_memory < 1) {
            total_memory = 0;
        }
    }

    Node *head = createNode(-1, 1, total_memory, total_memory);
    Node *current = head;

    // Listen for user operations
    while (1) {
        getline(&line, &size, stdin);

        if (strlen(line) > 2) {

            // Allocate memory
            if (line[0] == 'N') {
                line++;
                char *token;
                // Get process number
                token = strtok(line, " \t");
                int process = atoi(token);
                // Get process size
                token = token = strtok(NULL, " \t");
                int size = atoi(token);
                if (algorithm == 'f') {
                    result = first_fit(head, process, size);
                } else if (algorithm == 'b') {
                    result = best_fit(head, process, size);
                } else if (algorithm == 'w') {
                    result = worst_fit(head, process, size);
                }
                if (result == 0) {
                    // Could not allocate memory
                    printf("Process %d failed to allocate %d memory\n", process, size);
                }
            }

            // Deallocate memory
            else if (line[0] == 'T') {
                line++;
                char *token;
                // Get process number
                token = strtok(line, " \t");
                int process = atoi(token);
                result = deallocate_process(&head, process);
                if (result == 0) {
                    // Could not deallocate memory
                    printf("Process %d failed to free memory\n", process);
                }
            }

        } else if (line[0] == 'S') {
            // Stop program
            break;
        }
    }

    // Print Final Report
    current = head;
    final_smallest_fragment = current->block_size;
    final_largest_fragment = current->block_size;
    while (current != NULL) {
        if (current->free == 1) {
            // Calculate the final memory available
            final_memory_available += current->block_size;
        }
        if (current->block_size < final_smallest_fragment) {
            // Find the smallest memory fragment size
            final_smallest_fragment = current->block_size;
        }
        if (current->block_size > final_largest_fragment) {
            // Find the largest memory fragment size
            final_largest_fragment = current->block_size;
        }
        current = current->next;
    }
    printf("\nTotal Processes Created: %d\n", total_processes_created);
    printf("Total Allocated Memory: %d\n", total_allocated_memory);
    printf("Total Processes Terminated: %d\n", total_processes_terminated);
    printf("Total Freed Memory: %d\n", total_freed_memory);
    printf("Final Memory Available: %d\n", final_memory_available);
    printf("Final Smallest Fragmented Memory Size: %d\n", final_smallest_fragment);
    printf("Final Largest Fragmented Memory Size: %d\n\n", final_largest_fragment);

    return EXIT_SUCCESS;
}

// First Fit Algorithm
int first_fit(Node *current, int process, int size) {
    // Find the first free memory block that fits the process
    while (current != NULL) {
        if (current->free == 1 && current->block_size >= size) {
            return allocate_process(&current, process, size);
        }
        current = current->next;
    }
    // Return 0 when allocation fails
    return 0;
}

// Best Fit Algorithm
int best_fit(Node *current, int process, int size) {
    Node *best = current;
    while (current->next != NULL) {
        current = current->next;
        if (best->free != 1) {
            if (current->free == 1) {
                best = current;
            }
        } else {
            // Find the smallest free memory block that fits the process (to create the smallest fragment)
            if (current->free == 1 && current->block_size >= size && current->block_size < best->block_size) {
                best = current;
            }
        }
    }

    if (best->free == 1 && best->block_size >= size) {
        return allocate_process(&best, process, size);
    } else {
        // Return 0 when allocation fails
        return 0;
    }
}

// Worst Fit Algorithm
int worst_fit(Node *current, int process, int size) {
    Node *worst = current;
    while (current->next != NULL) {
        current = current->next;
        if (worst->free != 1) {
            if (current->free == 1) {
                worst = current;
            }
        } else {
            // Find the largest free memory block that fits the process (to create the largest fragment)
            if (current->free == 1 && current->block_size >= size && current->block_size > worst->block_size) {
                worst = current;
            }
        }
    }

    if (worst->free == 1 && worst->block_size >= size) {
        return allocate_process(&worst, process, size);
    } else {
        // Return 0 when allocation fails
        return 0;
    }
}

// Function to allocate memory for a process (Called by the Memory Allocation Algorithms)
int allocate_process(Node **node, int process, int size) {
    if (size < (*node)->block_size) {
        // New process is smaller than the size of the free memory block
        int new_node_end = (*node)->end;
        (*node)->block_size = size;
        (*node)->end = (*node)->start + size - 1;
        (*node)->free = 0;
        (*node)->process = process;
        int new_node_start = (*node)->end + 1;
        int new_node_size = new_node_end - new_node_start + 1;
        Node *new_node = createNode(-1, new_node_start, new_node_end, new_node_size);
        if ((*node)->next != NULL) {
            (*node)->next->previous = new_node;
            new_node->next = (*node)->next;
        }
        new_node->previous = (*node);
        (*node)->next = new_node;
    } else if (size == (*node)->block_size) {
        // New process is exactly the size of the free memory block
        (*node)->free = 0;
        (*node)->process = process;
    }
    // Increment Final Report Variables
    total_processes_created++;
    total_allocated_memory += size;
    return (*node)->start;
}

// Function to deallocate memory for a process
int deallocate_process(Node **node, int process) {
    Node *current = (*node);
    while (current != NULL) {
        if (current->process == process) {
            current->free = 1;
            current->process = -1;
            // Increment Final Report Variables
            total_processes_terminated++;
            total_freed_memory += current->block_size;

            // Check if two free blocks of memory need to be merged into one free block
            if (current->next != NULL) {
                if (current->next->free == 1) {
                    // Next memory block was also free
                    current->end = current->next->end;
                    current->block_size += current->next->block_size;
                    Node *temp = current->next->next;
                    free(current->next);
                    current->next = temp;
                    if (temp != NULL) {
                        temp->previous = current;
                    }
                }
            } else if (current->previous != NULL) {
                if (current->previous->free == 1) {
                    // Previous memory block was also free
                    current->previous->end = current->end;
                    current->previous->block_size += current->block_size;
                    Node *temp = current->next;
                    current->previous->next = temp;
                    if (temp != NULL) {
                        temp->previous = current->previous;
                    }
                    free(current);
                }
            }
            return 1;
        }
        current = current->next;
    }
    return 0;
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
