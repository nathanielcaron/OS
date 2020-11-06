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

    unsigned int pages_number = 1 << 20; // 2^20 or 1,048,576
    unsigned int page_frame_size = 1 << 12; // 2^12 or 4096
    unsigned int invalid_page_number = pages_number + 1;
    unsigned int invalid_frame_number = n + 1;

    // Initialize frames array
    unsigned int frame_table[n];
    for (i = 0; i < n; i++) {
        frame_table[i] = invalid_page_number;
    }

    // Initialize page table array
    unsigned int page_table[pages_number];
    for (i = 0; i < pages_number; i++) {
        page_table[i] = invalid_frame_number;
    }

    int page_faults = 0;
    unsigned int current_logical_address = 0;
    unsigned int page_number = 0;
    unsigned int offset = 0;
    unsigned int current_frame_number = 0;
    unsigned int frame_number = 0;
    unsigned int previous_page_number = 0;
    unsigned int physical_address = 0;

    int result = scanf("%u", &current_logical_address);

    while(result == 1) {
        // Split logical address into page number and offset
        page_number = current_logical_address / page_frame_size; // Top 20 bits
        offset = current_logical_address % page_frame_size; // Lower 12 bits

        if (page_table[page_number] != invalid_frame_number) {
            // Page loaded in frame
            frame_number = page_table[page_number];
            // Calculated physical address
            physical_address = frame_number * page_frame_size + offset;
        } else {
            // Page fault
            frame_number = current_frame_number;

            if (frame_table[frame_number] != invalid_page_number) {
                // Frame has a page in it, must clear frame table and page table
                previous_page_number = frame_table[frame_number];
                page_table[previous_page_number] = invalid_frame_number;
            }

            page_table[page_number] = frame_number;
            frame_table[frame_number] = page_number;
            // Calculate physical address
            physical_address = frame_number * page_frame_size + offset;
            // Move to next frame (FIFO)
            current_frame_number = (current_frame_number + 1) % n;
            // Increment number of page faults
            page_faults++;
        }
        // logical address -> physical address
        printf("%u -> %u\n", current_logical_address, physical_address);
        // Read new logical address
        result = scanf("%u", &current_logical_address);
    }

    // Print final report
    printf("\nNumber of Page faults: %d\n\n", page_faults);

    return EXIT_SUCCESS;
}
