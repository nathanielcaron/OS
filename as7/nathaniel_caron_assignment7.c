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
    int i = 0;

    // Read Number of allocated frames (n) from cmd line arguments
    int n = 0;
    if (argc > 1) {
        n = atoi(argv[i+1]);
        if (n < 0) {
            n = 0;
        }
    }

    unsigned int pages_number = 1 << 20;
    unsigned int page_frame_size = 1 << 12;
    unsigned int invalid_page_number = pages_number + 1;
    unsigned int invalid_frame_number = n + 1;

    // Initialize frames array
    unsigned int frame_table[n];
    for (i = 0; i < n; i++) {
        frame_table[i] = invalid_page_number;
    }

    // Initialize pages array
    unsigned int page_table[pages_number];
    for (i = 0; i < pages_number; i++) {
        page_table[i] = invalid_frame_number;
    }

    // REMOVE THIS
    printf("Number of frames n: %d\n", n);
    printf("Initialized frame_table array of size: %d\n", n);
    printf("Initialized page_table array of size: %d\n", pages_number);
    printf("Invalid frame number: %d + 1 = %d\n", n, invalid_frame_number);
    printf("Invalid page number: %d + 1 = %d\n", pages_number, invalid_page_number);
    printf("Page size = Frame size: %d bytes\n", page_frame_size);

    int page_faults = 0;

    unsigned int current_logical_address = 0;
    unsigned int page_number = 0;
    unsigned int offset = 0;
    unsigned int current_frame_number = 0;
    unsigned int frame_number = 0;
    unsigned int previous_page_number = 0;

    int line_len = 1000;
    char line[1000] = {0};

    unsigned int physical_address = 0;

    printf("\n--- --- --- --- --- --- --- ---\n");

    int result = scanf("%u", &current_logical_address);

    while(result == 1) {
        printf("logical address = %u, ", current_logical_address);
        page_number = current_logical_address / page_frame_size; // Top 20 bits
        offset = current_logical_address % page_frame_size; // Lower 12 bits
        printf("Page number: %u, ", page_number);
        printf("Offset: %u, ", offset);

        if (page_table[page_number] != invalid_frame_number) {
            // Page loaded in frame
            frame_number = page_table[page_number];
            physical_address = frame_number * page_frame_size + offset;
            printf("physical address: %u\n", physical_address);
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
            printf("Physical address: %u\n", physical_address);
            current_frame_number = (current_frame_number + 1) % n;
            // printf("current_logical_address frame number = %d\n", current_frame_number);
            page_faults++;
            // printf("Page fault!\n");
        }
        // Read new logical address
        result = scanf("%u", &current_logical_address);
    }

    printf("--- --- --- --- --- --- --- ---\n");

    // Print final report
    printf("\nNumber of Page faults: %d\n", page_faults);

    return EXIT_SUCCESS;
}
