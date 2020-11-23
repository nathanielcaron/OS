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
    int count;
} Frame;

// Structure for Page
typedef struct Page {
    unsigned int frame_number;
	bool swapped;
} Page;

// Structure for input
typedef struct Input {
    unsigned int logical_address;
    char operation;
} Input;

// Function templates
void first_in_first_out(Page *page_directory[], Frame frame_table[]);
void least_recently_used(Page *page_directory[], Frame frame_table[]);
void optimal(Page *page_directory[], Frame frame_table[]);

// Global variables
int i = 0;
int j = 0;
int n = 0;
int minor_page_faults = 0;
int major_page_faults = 0;
int page_hits = 0;
int pages_in_swap = 0;

unsigned int page_number = 1 << 20; // 2^20 or 1,048,576
unsigned int page_directory_table_size = 1 << 10; // 2^10 or 1,024
unsigned int page_frame_size = 1 << 12; // 2^12 or 4,096
unsigned int invalid_page_number;
unsigned int invalid_frame_number;

unsigned int current_page = 0;
unsigned int current_page_directory = 0;
unsigned int current_page_number = 0;
unsigned int current_offset = 0;
unsigned int current_frame_number = 0;
unsigned int current_physical_address = 0;

unsigned int frame_number_to_replace = 0;
unsigned int page_directory_to_replace = 0;
unsigned int page_number_to_replace = 0;

int main(int argc, char **argv) {
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

    invalid_page_number = page_number + 1;
    invalid_frame_number = n + 1;

    // Initialize frames array
    Frame frame_table[n];
    for (i = 0; i < n; i++) {
        frame_table[i].page_number = invalid_page_number;
        frame_table[i].dirty = false;
        frame_table[i].count = 0;
    }

    // Initialize page directory (Don't allocate memory for page tables yet, see algorithms for dynamic page allocation)
    Page *page_directory[page_directory_table_size];
    for (i = 0; i < page_directory_table_size; i++) {
        page_directory[i] = NULL;
    }

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

// Function for First In First Out Page Replacement Algorithm
void first_in_first_out(Page *page_directory[], Frame frame_table[]) {
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

        current_page_directory = (current_logical_address / page_frame_size) / page_directory_table_size; // Top 10 bits
        current_page_number = (current_logical_address / page_frame_size) % page_directory_table_size; // Middle 10 bits
        current_offset = current_logical_address % page_frame_size; // Lower 12 bits

        // Dynamically allocate page tables as needed
        if (page_directory[current_page_directory] == NULL) {
            Page *base_table_pointer = (Page *)malloc(page_directory_table_size * sizeof(Page));
            page_directory[current_page_directory] = base_table_pointer;
            for (i = 0; i < page_directory_table_size; i++) {
                base_table_pointer[i].frame_number = invalid_frame_number;
                base_table_pointer[i].swapped = false;
            }
        }

        if ((page_directory[current_page_directory])[current_page_number].frame_number != invalid_frame_number) {
            // PAGE HIT
            current_frame_number = (page_directory[current_page_directory])[current_page_number].frame_number;
            page_hits++;
            // Calculate physical address
            current_physical_address = current_frame_number * page_frame_size + current_offset;
            if (operation == 'w') {
                // write to page
                frame_table[current_frame_number].dirty = true;
            }
        } else {
            // PAGE FAULT
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
        // logical address -> physical address
        printf("%u -> %u\n", current_logical_address, current_physical_address);
    }
}

// Function for Least Recently Used Page Replacement Algorithm
void least_recently_used(Page *page_directory[], Frame frame_table[]) {
    int i = 0;
    char operation;
    unsigned int current_logical_address = 0;
    
    int line_len = 1000;
    char line[1000] = {0};

    int input_index = 0;
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

        current_page_directory = (current_logical_address / page_frame_size) / page_directory_table_size; // Top 10 bits
        current_page_number = (current_logical_address / page_frame_size) % page_directory_table_size; // Middle 10 bits
        current_offset = current_logical_address % page_frame_size; // Lower 12 bits

        // Dynamically allocate page tables as needed
        if (page_directory[current_page_directory] == NULL) {
            Page *base_table_pointer = (Page *)malloc(page_directory_table_size * sizeof(Page));
            page_directory[current_page_directory] = base_table_pointer;
            for (i = 0; i < page_directory_table_size; i++) {
                base_table_pointer[i].frame_number = invalid_frame_number;
                base_table_pointer[i].swapped = false;
            }
        }

        if ((page_directory[current_page_directory])[current_page_number].frame_number != invalid_frame_number) {
            // PAGE HIT
            current_frame_number = (page_directory[current_page_directory])[current_page_number].frame_number;
            page_hits++;
            // Time since use = 0
            frame_table[current_frame_number].count = 0;
            // Calculate physical address
            current_physical_address = current_frame_number * page_frame_size + current_offset;
            if (operation == 'w') {
                // write to page
                frame_table[current_frame_number].dirty = true;
            }
        } else {
            // PAGE FAULT
            
            current_frame_number = frame_number_to_replace;

            if (input_index < n) {
                // Have free frame
                current_frame_number = input_index;
            }

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
            // Time since use = 0
            frame_table[current_frame_number].count = 0;

            if (operation == 'w') {
                // write to page
                frame_table[current_frame_number].dirty = true;
            }
            // Calculate physical address
            current_physical_address = current_frame_number * page_frame_size + current_offset;
        }

        // Find the least recently used page frame
        for (i = 0; i < n; i++) {
            frame_table[i].count++;
            if (frame_table[i].count > frame_table[frame_number_to_replace].count) {
                frame_number_to_replace = i;
            }
        }

        // logical address -> physical address
        printf("%u -> %u\n", current_logical_address, current_physical_address);
        input_index++;
    }
}

// Function for Optimal Page Replacement Algorithm
void optimal(Page *page_directory[], Frame frame_table[]) {
    int i = 0;
    int j = 0;
    int order_to_come = 0;
    size_t input_size = 1000;
    Input *input = (Input*)calloc(input_size, sizeof(Input));
    size_t input_count = 0;

    // Read input
    int line_len = 1000;
    char line[1000] = {0};
    while (fgets(line, line_len, stdin) != NULL) {
        int token_num = 0;

        // Reallocate more space for input if needed (VARIABLE SIZE)
        if (input_count == input_size) {
            input_size *= 2;
            input = (Input*)realloc(input, input_size * sizeof(Input));
        }

        // Tokenize each line
        char *token;
        for (token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 0) {
                input[input_count].operation = token[0];
            } else if (token_num == 1) {
                sscanf(token, "%u", &input[input_count].logical_address);
            }
            token_num++;
        }
        input_count++;
    }

    char current_operation;
    unsigned int current_logical_address = 0;

    unsigned int future_page_directory = 0;
    unsigned int future_page_number = 0;
    unsigned int future_frame_number = 0;

    int input_index;
    for (input_index = 0; input_index < input_count; input_index++) {
        // Get current operation and logical address
        current_operation = input[input_index].operation;
        current_logical_address = input[input_index].logical_address;

        current_page = current_logical_address / page_frame_size;
        current_page_directory = (current_logical_address / page_frame_size) / page_directory_table_size; // Top 10 bits
        current_page_number = (current_logical_address / page_frame_size) % page_directory_table_size; // Middle 10 bits
        current_offset = current_logical_address % page_frame_size; // Lower 12 bits

        // Dynamically allocate page tables as needed
        if (page_directory[current_page_directory] == NULL) {
            Page *base_table_pointer = (Page *)malloc(page_directory_table_size * sizeof(Page));
            page_directory[current_page_directory] = base_table_pointer;
            for (i = 0; i < page_directory_table_size; i++) {
                base_table_pointer[i].frame_number = invalid_frame_number;
                base_table_pointer[i].swapped = false;
            }
        }

        if ((page_directory[current_page_directory])[current_page_number].frame_number != invalid_frame_number) {
            // PAGE HIT
            current_frame_number = (page_directory[current_page_directory])[current_page_number].frame_number;
            page_hits++;
            if (current_operation == 'w') {
                // write to page
                frame_table[current_frame_number].dirty = true;
            }
            // Find next use of page in frame
            for (i = input_index+1; i < input_count; i++) {
                if (frame_table[current_frame_number].page_number == (input[i].logical_address / page_frame_size)) {
                    frame_table[current_frame_number].count = i;
                    break;
                } else if (i == input_count-1) {
                    frame_table[current_frame_number].count = i+1;
                }
            }
        } else {
            // PAGE FAULT
            if (input_index < n) {
                // Have free frame
                current_frame_number = frame_number_to_replace;
                // Minor page fault
                minor_page_faults++;
                if (frame_number_to_replace < n-1) {
                    frame_number_to_replace++;
                }
            } else {
                // Find OPTIMAL PAGE to replace (page that does not get used for the longest time)
                frame_number_to_replace = 0;
                for (i = 0; i < n; i++) {
                    if (frame_table[i].count > frame_table[frame_number_to_replace].count) {
                        frame_number_to_replace = i;
                    }
                }

                current_frame_number = frame_number_to_replace;

                if (frame_table[current_frame_number].page_number != invalid_page_number) {
                    // Frame has a page in it, must clear frame table and page table
                    page_directory_to_replace = frame_table[current_frame_number].page_number / page_directory_table_size;
                    page_number_to_replace = frame_table[current_frame_number].page_number % page_directory_table_size;

                    // Dynamically allocate page tables as needed
                    if (page_directory[page_directory_to_replace] == NULL) {
                        Page *base_table_pointer = (Page *)malloc(page_directory_table_size * sizeof(Page));
                        page_directory[page_directory_to_replace] = base_table_pointer;
                        for (i = 0; i < page_directory_table_size; i++) {
                            base_table_pointer[i].frame_number = invalid_frame_number;
                            base_table_pointer[i].swapped = false;
                        }
                    }

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
                }
            }

            (page_directory[current_page_directory])[current_page_number].frame_number = current_frame_number;
            frame_table[current_frame_number].page_number = current_page_directory * page_directory_table_size + current_page_number;
            frame_table[current_frame_number].dirty = false;

            if (current_operation == 'w') {
                // write to page
                frame_table[current_frame_number].dirty = true;
            }
            // Find next use of page inserted in frame
            for (i = input_index+1; i < input_count; i++) {
                if (frame_table[current_frame_number].page_number == (input[i].logical_address / page_frame_size)) {
                    frame_table[current_frame_number].count = i;
                    break;
                } else if (i == input_count-1) {
                    frame_table[current_frame_number].count = i+1;
                }
            }
        }
        // Calculate physical address
        current_physical_address = current_frame_number * page_frame_size + current_offset;
        // logical address -> physical address
        printf("%u -> %u\n", current_logical_address, current_physical_address);
    }
}
