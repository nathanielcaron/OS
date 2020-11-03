#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Assignment 7 - Paging
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

int main(int argc, char **argv) {
    // Initialize variables
    int i = 0;
    int n = 0;

    // Read in Number of allocated frames from cmd line arguments
    if (argc > 1) {
        n = atoi(argv[i+1]);
        if (n < 0) {
            n = 0;
        }
    }

    // REMOVE THIS
    printf("n = %d\n", n);

    int pages_number = 1 << 20;
    unsigned int invalid_page_number = pages_number + 1;
    printf("Invalid page number = %d + 1 = %d\n", pages_number, invalid_page_number);

    // Initialize frames array
    unsigned int frame_table[n];
    for (i = 0; i < n; i++) {
        frame_table[i] = invalid_page_number;
    }

    // REMOVE THIS
    printf("Initialized frame_table array of size %d\n", n);

    unsigned int invalid_frame_number = n + 1;
    printf("Invalid frame number = %d + 1 = %d\n", n, invalid_frame_number);

    // Initialize pages array
    unsigned int page_table[pages_number];
    for (i = 0; i < pages_number; i++) {
        page_table[i] = invalid_frame_number;
    }

    // REMOVE THIS
    printf("Initialized page_table array of size %d\n", pages_number);

    unsigned int page_frame_size = 1 << 12;
    // REMOVE THIS
    printf("Divisor %d\n", page_frame_size);

    int page_faults = 0;

    unsigned int current = 0;
    unsigned int page_number = 0;
    unsigned int offset = 0;
    unsigned int current_frame_number = 0;
    unsigned int frame_number = 0;
    unsigned int previous_page_number = 0;
    
    int line_len = 1000;
    char line[1000] = {0};

    unsigned int physical_address = 0;

    printf("\n--- --- --- --- --- --- --- ---\n");

    while (fgets(line, line_len, stdin) != NULL) {
        // Tokenize each line
        sscanf(line, "%u", &current);
        printf("logical address = %u, ", current);
        page_number = current / page_frame_size; // Top 20 bits
        offset = current % page_frame_size; // Lower 12 bits
        printf("Page number: %u, ", page_number);
        printf("Offset: %u, ", offset);

        // Perform work
        // 1. Page is loaded in a valid frame
        //      -> Physical address = Frame number * page_frame_size + offset
        // 2. Page is not loaded in a valid frame (page fault)
        //      -> Store current frame number in page table, store page number in frame table, Physical address = frame number * page_frame_size + offset, page_faults++, current_frame_number++

        if (page_table[page_number] != invalid_frame_number) {
            // Page loaded in frame
            frame_number = page_table[page_number];
            physical_address = frame_number * page_frame_size + offset;
            printf("Physical address: %d\n", physical_address);
        } else {
            // Page fault
            frame_number = current_frame_number;
            if (frame_table[frame_number] != invalid_page_number) {
                // Frame has a page in it, must clear frame table
                previous_page_number = frame_table[frame_number];
                page_table[previous_page_number] = invalid_frame_number;
            }
            page_table[page_number] = frame_number;
            frame_table[frame_number] = page_number;
            physical_address = frame_number * page_frame_size + offset;
            printf("Physical address: %d\n", physical_address);
            current_frame_number = (current_frame_number + 1) % n;
            printf("current frame number = %d\n", current_frame_number);
            page_faults++;
            printf("Page fault!\n");
        }
    }

    printf("--- --- --- --- --- --- --- ---\n");

    // Print final report
    printf("\nPage faults: %d\n", page_faults);

    return EXIT_SUCCESS;
}
