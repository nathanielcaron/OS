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

    // Initialize frames array
    unsigned int frames[n];
    for (i = 0; i < n; i++) {
        frames[i] = 0;
    }

    // REMOVE THIS
    printf("Initialized frames array of size %d\n", n);

    // Initialize pages array
    int pages_number = 1 << 20;
    unsigned int pages[pages_number];
    for (i = 0; i < pages_number; i++) {
        pages[i] = 0;
    }

    // REMOVE THIS
    printf("Initialized pages array of size %d\n", pages_number);

    unsigned int divisor = 1 << 12;
    // REMOVE THIS
    printf("Divisor %d\n", divisor);

    // Remove this
    // Bit manipulation test
    // {
    //     unsigned int value = 242; // 1111 0010
    //     unsigned int divisor = 16; // 2^4
    //     unsigned int top_bits = value / divisor; // 1111
    //     unsigned int bottom_bits = value % divisor; // 0010
    //     unsigned int value_without_bottom_bits = top_bits * divisor; // 1111 0000 or 240
    //     printf("value = %d, divisor = %d, top_bits = %d, bottom_bits = %d, value_without_bottom_bits = %d\n", value, divisor, top_bits, bottom_bits, value_without_bottom_bits);
    //     printf("value without bottom bits + bottom bits = %d + %d = %d = value\n", value_without_bottom_bits, bottom_bits, value);
    // }

    unsigned int current = 0;
    unsigned int page_number = 0;
    unsigned int offset = 0;
    
    int line_len = 1000;
    char line[1000] = {0};

    while (fgets(line, line_len, stdin) != NULL) {
        // Tokenize each line
        sscanf(line, "%u", &current);
        printf("logical address = %u, ", current);
        page_number = current / divisor; // Top 20 bits
        offset = current % divisor; // Lower 12 bits
        printf("Page number: %u, ", page_number);
        printf("Offset: %u\n", offset);
    }
    return EXIT_SUCCESS;
}
