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
int allocate_process(Node *node, int process, int size);
int deallocate_process(int process);
Node* createNode(int process, int start, int end, int block_size);

int main(int argc, char **argv) {
    int total_memory = 0;
    int block_count = 0;

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

// First Fit Algorithm
int first_fit(Node *current, int process, int size) {
    int return_value;
    if (current->free == 1 && current->block_size >= size) {
        return_value = allocate_process(current, process, size);
        return return_value;
    }
    while (current->next != NULL) {
        current = current->next;
        if (current->free == 1 && current->block_size >= size) {
            return_value = allocate_process(current, process, size);
            return return_value;
        }
    }
    return -1;
}

// Best Fit Algorithm
int best_fit(Node *current, int process, int size) {
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
        return_value = allocate_process(best, process, size);
        return return_value;
    }
}

// Worst Fit Algorithm
int worst_fit(Node *current, int process, int size) {
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
        return_value = allocate_process(worst, process, size);
        return return_value;
    }
}

int allocate_process(Node *node, int process, int size) {
    return 0;
}

int deallocate_process(int process) {
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
