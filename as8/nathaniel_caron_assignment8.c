#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/**
 * Assignment 8 - Page Replacement Algorithms
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

// Structure for Frame
typedef struct Frame {
    unsigned int page_number;
	bool dirty;
} Frame;

// Structure for Memory Block Node
typedef struct Page {
    unsigned int frame_number;
	bool swapped;
} Page;

// Function templates
void first_in_first_out(Page *page_directory[], Frame frame_table[]);
void least_recently_used(Page *page_directory[], Frame frame_table[]);
void optimal(Page *page_directory[], Frame frame_table[]);


// Global variables
int n = 0;
int minor_page_faults = 0;
int major_page_faults = 0;
int page_hits = 0;
int pages_in_swap = 0;

int main(int argc, char **argv) {
    int i = 0;
    int j = 0;
    char algorithm = 'f';

    // Read in frame number n and page replacement algorithm from cmd line arguments
    if (argc > 1) {
        for (i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-F") == 0) {
                algorithm = 'f';
            } else if (strcmp(argv[i], "-L") == 0) {
                algorithm = 'l';
            } else if (strcmp(argv[i], "-O") == 0) {
                algorithm = 'o';
            } else if (atoi(argv[i]) != 0) {
                n = atoi(argv[i]);
            }
        }
        if (n < 1) {
            n = 0;
        }
    }

    printf("algorithm: %c, n: %d\n", algorithm, n);

    unsigned int page_number = 1 << 20; // 2^20 or 1,048,576
    unsigned int page_directory_table_size = 1 << 10; // 2^10 or 1,024
    unsigned int page_frame_size = 1 << 12; // 2^12 or 4,096
    unsigned int invalid_page_number = page_number + 1;
    unsigned int invalid_frame_number = n + 1;
    printf("invalid page number: %d, invalid frame number: %d\n", invalid_page_number, invalid_frame_number);

    // Initialize frames array
    Frame frame_table[n];
    for (i = 0; i < n; i++) {
        frame_table[i].page_number = invalid_page_number;
        frame_table[i].dirty = false;
    }

    // Initialize page directory and page tables
    int count = 0;
    Page *page_directory[page_directory_table_size]; // page directory size 2^10
    for (i = 0; i < page_directory_table_size; i++) {
        Page *base_table_pointer = (Page *)malloc(page_directory_table_size * sizeof(Page));
        page_directory[i] = base_table_pointer;
        for (j = 0; j < page_directory_table_size; j++) {
            base_table_pointer[j].frame_number = invalid_frame_number;
            base_table_pointer[j].swapped = false;
        }
        count++;
    }
    printf("Initialized %d page tables\n", count);
    printf("Accessing page table 0 position 0 -> %d\n", (page_directory[0])[0].frame_number);
    printf("Accessing page table 1023 position 1023 -> %d\n\n", (page_directory[1023])[1023].frame_number);

    if (algorithm == 'f') {
        first_in_first_out(page_directory, frame_table);
    } else if (algorithm == 'l') {
        least_recently_used(page_directory, frame_table);
    } else if (algorithm == 'o') {
        optimal(page_directory, frame_table);
    }

    // Print Final Report
    printf("\nMinor Page Faults: %d\n", minor_page_faults);
    printf("Major Page Faults: %d\n", major_page_faults);
    printf("Page Hits: %d\n", page_hits);
    printf("Pages Currently in Swap: %d\n\n", pages_in_swap);

    return EXIT_SUCCESS;
}

void first_in_first_out(Page *page_directory[], Frame frame_table[]) {
    char operation;
    unsigned int current_logical_address = 0;
    
    int line_len = 1000;
    char line[1000] = {0};

    int current_page_directory = 0;
    int current_page_number = 0;
    int current_offset = 0;

    unsigned int page_number = 1 << 20; // 2^20 or 1,048,576
    unsigned int page_frame_size = 1 << 12; // 2^12 or 4,096
    unsigned int page_directory_table_size = 1 << 10; // 2^10 or 1,024
    unsigned int invalid_page_number = page_number + 1;
    unsigned int invalid_frame_number = n + 1;

    unsigned int current_frame_number = 0;
    unsigned int current_physical_address = 0;

    unsigned int frame_number_to_replace = 0;
    unsigned int page_directory_to_replace = 0;
    unsigned int page_number_to_replace = 0;

    while (fgets(line, line_len, stdin) != NULL) {
        int token_num = 0;

        // Tokenize line
        char *token;
        for (token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 0) {
                operation = token[0];
            } else if (token_num == 1) {
                sscanf(token, "%u", &current_logical_address);
            }
            token_num++;
        }

        printf("operation: %c, logical address: %u\n", operation, current_logical_address);

        current_page_directory = (current_logical_address / page_frame_size) / page_directory_table_size; // Top 10 bits
        current_page_number = (current_logical_address / page_frame_size) % page_directory_table_size; // Middle 10 bits
        current_offset = current_logical_address % page_frame_size; // Lower 12 bits

        printf("logical address: %u, page directory: %u, page table: %u, offset: %u\n", current_logical_address, current_page_directory, current_page_number, current_offset);

        if ((page_directory[current_page_directory])[current_page_number].frame_number != invalid_frame_number) {
            // Page hit
            current_frame_number = (page_directory[current_page_directory])[current_page_number].frame_number;
            page_hits++;
            // Calculated physical address
            current_physical_address = current_frame_number * page_frame_size + current_offset;
            if (operation == 'w') {
                // write to page
                frame_table[current_frame_number].dirty = true;
            }
        } else {
            // Page fault
            current_frame_number = frame_number_to_replace;

            if (frame_table[current_frame_number].page_number != invalid_page_number) {
                // Frame has a page in it, must clear frame table and page table
                page_directory_to_replace = frame_table[current_frame_number].page_number / page_directory_table_size;
                page_number_to_replace = frame_table[current_frame_number].page_number % page_directory_table_size;
                (page_directory[page_directory_to_replace])[page_number_to_replace].frame_number = invalid_frame_number;
                if (frame_table[current_frame_number].dirty == true) {
                    // Major page fault
                    major_page_faults++;
                    if ((page_directory[page_directory_to_replace])[page_number_to_replace].swapped == false) {
                        (page_directory[page_directory_to_replace])[page_number_to_replace].swapped = true;
                        pages_in_swap++;
                    }
                } else {
                    // Minor page fault
                    minor_page_faults++;
                }
            } else {
                // Minor page fault
                minor_page_faults++;
            }

            (page_directory[current_page_directory])[current_page_number].frame_number = current_frame_number;
            frame_table[current_frame_number].page_number = current_page_directory * page_directory_table_size + current_page_number;
            frame_table[current_frame_number].dirty = false;

            if (operation == 'w') {
                // write to page
                frame_table[current_frame_number].dirty = true;
            }
            // Calculate physical address
            current_physical_address = current_frame_number * page_frame_size + current_offset;
            // Move to next frame (FIFO)
            frame_number_to_replace = (frame_number_to_replace + 1) % n;
        }
        printf("dirty: %d\n", frame_table[current_frame_number].dirty);
        // logical address -> physical address
        printf("%u -> %u\n", current_logical_address, current_physical_address);
    }
}

void least_recently_used(Page *page_directory[], Frame frame_table[]) {
    char operation;
    unsigned int current_logical_address = 0;
    
    int line_len = 1000;
    char line[1000] = {0};

    while (fgets(line, line_len, stdin) != NULL) {
        int token_num = 0;

        // Tokenize line
        char *token;
        for (token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 0) {
                operation = token[0];
            } else if (token_num == 1) {
                sscanf(token, "%u", &current_logical_address);
            }
            token_num++;
        }

        printf("operation: %c, logical address: %u\n", operation, current_logical_address);

        // Perform memory operation
        if (operation == 'r') {

        } else if (operation == 'w') {

        }
    }
}

void optimal(Page *page_directory[], Frame frame_table[]) {

}
