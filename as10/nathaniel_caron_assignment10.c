#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/**
 * Assignment 10 - Disk Defragmentation
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

// Structure to represent a request
typedef struct Block {
    unsigned int next;
    bool file_start;
    bool checked;
} Block;

// Function templates
void getBlocks(Block *blocks);
void reallocateFiles(int **files, int *size);

// Global variables
int block_count = 0;
int free_block_count = 0;
int total_file_count = 0;
int blocks_to_move = 0;

int main(int argc, char **argv) {
    int i = 0;
    int j = 0;

    Block blocks[100000];
    getBlocks(blocks);

    // TODO: remove this
    printf("Blocks before defragmentation: \n");
    for (i = 0; i < block_count; i++) {
        printf("%d - %d - %d\n", i, blocks[i].next, blocks[i].file_start);
    }
    printf("\n");

    int file_count = 0;
    int files_size = 100;
    int *files = (int*)calloc(files_size, sizeof(int));

    for (i = 0; i < block_count; i++) {
        if (blocks[i].next != -2 && !blocks[i].checked) {
            // Block is not empty

            // Check if file start
            for (j = 0; j < block_count; j++) {
                if (blocks[j].next == i) {
                    // Not a file start
                    break;
                } else if (j == block_count-1) {
                    // file start
                    blocks[i].file_start = true;
                }
            }

            // Calculate how many blocks file is
            if (blocks[i].file_start == true) {
                // Reallocate more space for files if needed (VARIABLE SIZE)
                if (file_count == files_size) {
                    reallocateFiles(&files, &files_size);
                }

                int current = i;
                int previous = 0;
                bool done = false;
                files[file_count] = 0;
                printf("*%d* ", file_count);
                while (!done) {
                    printf("%d ((%d))-> ", current, files[file_count]);
                    if (blocks[current].next == -1) {
                        // End of file
                        files[file_count]++;
                        blocks[current].checked = true;
                        done = true;
                    } else if (blocks[current].next != -2) {
                        files[file_count]++;
                        previous = current;
                        current = blocks[current].next;
                        blocks[previous].checked = true;
                    }
                }
                file_count++;
                printf("\n");
            }
        }
    }

    printf("Blocks before defragmentation w/ file starts: \n");
    for (i = 0; i < block_count; i++) {
        printf("%d - %d - %d\n", i, blocks[i].next, blocks[i].file_start);
    }
    printf("\n");

    printf("Files: \n");
    for (i = 0; i < file_count; i++) {
        printf("%d - %d\n", i, files[i]);
    }
    printf("\n");

    int block_position = 0;
    int new_value = 0;
    for (i = 0; i < file_count; i++) {
        bool done = false;
        while (!done) {
            if (files[i] != 1) {
                new_value = block_position + 1;
            } else {
                // End of file
                new_value = -1;
                done = true;
            }
            if (blocks[block_position].next != new_value) {
                    blocks[block_position].next = new_value;
                    blocks_to_move++;
            }
            files[i]--;
            block_position++;
        }
    }

    // Set free blocks at end of disk
    int assigned_free_block_count = 0;
    for (i = block_position; i < block_count; i++) {
        if (blocks[i].next != -2) {
            blocks[i].next = -2;
            blocks_to_move++;
        }
        assigned_free_block_count++;
    }

    printf("Blocks after defragmentation: \n");
    for (i = 0; i < block_count; i++) {
        printf("%d - %d \n", i, blocks[i].next);
    }

    printf("\nfree block count %d\n", free_block_count);
    printf("assigned free block count %d\n", assigned_free_block_count);

    printf("\ntotal file count %d\n", total_file_count);
    printf("file count %d\n", file_count);

    printf("blocks to move %d\n", blocks_to_move);

    return EXIT_SUCCESS;
}

// Function to read in the blocks from std input
void getBlocks(Block *blocks) {
    int line_len = 1000;
    char line[1000] = {0};

    while (fgets(line, line_len, stdin) != NULL) {
        int token_num = 0;

        // Tokenize each line
        char *token;
        for (token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 1) {
                blocks[block_count].next = atoi(token);
            }
            token_num++;
        }
        blocks[block_count].file_start = false;
        blocks[block_count].checked = false;
        if (blocks[block_count].next == -2) {
            free_block_count++;
        } else if (blocks[block_count].next == -1) {
            total_file_count++;
        }
        block_count++;
    }
}

// Function to reallocate space for the files (VARIABLE SIZE)
void reallocateFiles(int **files, int *size) {
    *size *= 2;
    *files = (int*)realloc(*files, (*size) * sizeof(int));
}
