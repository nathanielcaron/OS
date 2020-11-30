#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/**
 * Assignment 9 - Disk Scheduling Algorithms FCFS, SSTF, C-SCAN, LOOK
 * CS 3413
 * Nathaniel Caron
 * 3598979
 */

// Structure to represent a request
typedef struct Request {
    unsigned int sector;
    unsigned int arrival;
    bool arrived;
    bool done;
} Request;

// Function templates
void checkForArrivedRequests(Request *requests);
int timeRequiredForRequest(int distance, bool reverse_direction);
void firstInFirstOut(Request *requests);
void shortestSeekTimeFirst(Request *requests);
void cScan(Request *requests);
void look(Request *requests);
void initRequests(Request **requests, size_t *size);
void reallocateRequests(Request **requests, size_t *size);
int getRequests(Request **requests, size_t *requests_size);

// Global variables
int i = 0;
int j = 0;
int n = 0;
char algorithm = 'f';
char movement_direction = 'a';
int head_position = 0;
int request_count = 0;
int current_time = 0;
int arrived_requests_index = 0;
int requests_done = 0;
int first_request_arrival = 0;
int total_head_movement = 0;
int total_service_time = 0;

int request_to_service = 0;
bool reverse_direction = false;
bool done = false;

int main(int argc, char **argv) {
    // Read in disk scheduling algorithm, initial head position, and initial movement direction from cmd line arguments
    if (argc > 1) {
        for (i = 0; i < argc; i++) {
            if (strcmp(argv[i], "F") == 0) {
                algorithm = 'f';
            } else if (strcmp(argv[i], "T") == 0) {
                algorithm = 't';
            } else if (strcmp(argv[i], "C") == 0) {
                algorithm = 'c';
            } else if (strcmp(argv[i], "L") == 0) {
                algorithm = 'l';
            } else if (strcmp(argv[i], "a") == 0) {
                movement_direction = 'a';
            } else if (strcmp(argv[i], "d") == 0) {
                movement_direction = 'd';
            } else if (atoi(argv[i]) != 0) {
                head_position = atoi(argv[i]);
            }
        }

        if (head_position < 0) {
            head_position = 0;
        } else if (head_position > 9999) {
            head_position = 9999;
        }
    }

    // TODO: remove this
    // printf("algorithm: %c, starting head position: %u, starting movement direction: %c\n\n", algorithm, head_position, movement_direction);

    Request *requests;
    size_t initial_size;
    initRequests(&requests, &initial_size);

    // Get the requests from stdin
    getRequests(&requests, &initial_size);

    // TODO: remove this
    // for (i = 0; i < request_count; i++) {
    //     printf("%d - sector: %d - arrival - %d - arrived %d - done %d\n", i, requests[i].sector, requests[i].arrival, requests[i].arrived, requests[i].done);
    // }
    // printf("Request count: %d\n\n", request_count);

    first_request_arrival = requests[0].arrival;

    // Service requests with selected algorithm
    if (algorithm == 'f') {
        firstInFirstOut(requests);
    } else if (algorithm == 't') {
        shortestSeekTimeFirst(requests);
    } else if (algorithm == 'c') {
        cScan(requests);
    } else if (algorithm == 'l') {
        look(requests);
    }

    // Print Final Report
    printf("\nTotal Head Movement: %d\n", total_head_movement);
    printf("Total Service Time: %d\n", total_service_time);

    return EXIT_SUCCESS;
}

void checkForArrivedRequests(Request *requests) {
    for (i = arrived_requests_index; i < request_count; i++) {
        if (requests[i].arrival <= current_time) {
            requests[i].arrived = true;
            arrived_requests_index++;
        }
    }
}

// Function to calculate the time required to process a request
int timeRequiredForRequest(int distance, bool reverse_direction) {
    // Uses the function from the aasignment description:
    // time = distance/10 + (reverse_direction) ? 5 : 0
    return distance / 10 + ((reverse_direction) ? 5 : 0);
}

void firstInFirstOut(Request *requests) {
    unsigned int current_sector = 0;
    int current_distance = 0;
    int current_time_required = 0;

    while (!done) {
        checkForArrivedRequests(requests);

        if (requests[request_to_service].arrived && !requests[request_to_service].done) {
            current_sector = requests[request_to_service].sector;
            if (head_position == current_sector) {
                // No movement required
                current_distance = 0;
                reverse_direction = false;
            } else if (head_position > current_sector) {
                // Must move down
                if (movement_direction == 'd') {
                    // No need to switch direction
                    reverse_direction = false;
                } else if (movement_direction == 'a') {
                    // Must switch direction
                    reverse_direction = true;
                    movement_direction = 'd';
                }
                current_distance = head_position - current_sector;
            } else if (head_position < current_sector) {
                // Must move up
                if (movement_direction == 'a') {
                    // No need to switch direction
                    reverse_direction = false;
                } else if (movement_direction == 'd') {
                    // Must switch direction
                    reverse_direction = true;
                    movement_direction = 'a';
                }
                current_distance = current_sector - head_position;
            }

            // Calculate time required and set new head position
            current_time_required = timeRequiredForRequest(current_distance, reverse_direction);
            head_position = current_sector;
            // Mark request as done
            requests[request_to_service].done = true;
            requests_done++;
            // Update time, total head movement, and move on to next request
            current_time += current_time_required;
            total_head_movement += current_distance;
            request_to_service++;
        } else {
            current_time++;
        }

        // TODO: remove this
        printf("--- time %d - total head movement %d - current time required %d - current distance required %d ---\n", current_time, total_head_movement, current_time_required, current_distance);
        for (i = 0; i < request_count; i++) {
            printf("%d - sector: %d - arrival - %d - arrived %d - done %d\n", i, requests[i].sector, requests[i].arrival, requests[i].arrived, requests[i].done);
        }

        // Check if all requests have been serviced
        if (requests_done == request_count) {
            done = true;
            total_service_time = current_time - first_request_arrival;
        }
    }
}

void shortestSeekTimeFirst(Request *requests) {
    int shortest_seek_time = 0;
    int shortest_seek_index = 0;
    int current_seek = 0;
    int current_seek_time = 0;

    unsigned int current_sector = 0;
    int current_distance = 0;
    int current_time_required = 0;

    while (!done) {
        checkForArrivedRequests(requests);

        // Determine the request to service
        shortest_seek_time = 10000;
        for (i = 0; i < arrived_requests_index; i++) {
            if (requests[i].arrived && !requests[i].done) {
                current_seek = abs(requests[i].sector - head_position);
                // Calculate seek time
                if (current_seek == 0) {
                    // No movement required
                    reverse_direction = false;
                } else if (head_position > requests[i].sector) {
                    // Must move down
                    if (movement_direction == 'd') {
                        // No need to switch direction
                        reverse_direction = false;
                    } else if (movement_direction == 'a') {
                        // Must switch direction
                        reverse_direction = true;
                    }
                } else if (head_position < requests[i].sector) {
                    // Must move up
                    if (movement_direction == 'a') {
                        // No need to switch direction
                        reverse_direction = false;
                    } else if (movement_direction == 'd') {
                        // Must switch direction
                        reverse_direction = true;
                    }
                }
                current_seek_time = timeRequiredForRequest(current_seek, reverse_direction);
                // Check if current seek time is less than shortest seek time
                if (current_seek_time < shortest_seek_time) {
                    shortest_seek_time = current_seek_time;
                    shortest_seek_index = i;
                }
            }
        }

        printf("*** Shortest seek: %d - request %d ***\n", shortest_seek_time, requests[shortest_seek_index].sector);

        request_to_service = shortest_seek_index;

        if (requests[request_to_service].arrived && !requests[request_to_service].done) {
            current_sector = requests[request_to_service].sector;
            if (head_position == current_sector) {
                // No movement required
                current_distance = 0;
                reverse_direction = false;
            } else if (head_position > current_sector) {
                // Must move down
                if (movement_direction == 'd') {
                    // No need to switch direction
                    reverse_direction = false;
                } else if (movement_direction == 'a') {
                    // Must switch direction
                    reverse_direction = true;
                    movement_direction = 'd';
                }
                current_distance = head_position - current_sector;
            } else if (head_position < current_sector) {
                // Must move up
                if (movement_direction == 'a') {
                    // No need to switch direction
                    reverse_direction = false;
                } else if (movement_direction == 'd') {
                    // Must switch direction
                    reverse_direction = true;
                    movement_direction = 'a';
                }
                current_distance = current_sector - head_position;
            }
            // Calculate time required and set new head position
            current_time_required = timeRequiredForRequest(current_distance, reverse_direction);
            head_position = current_sector;
            // Mark request as done
            requests[request_to_service].done = true;
            requests_done++;
            // Update time, total head movement, and move on to next request
            current_time += current_time_required;
            total_head_movement += current_distance;
            request_to_service++;
        } else {
            current_time++;
        }

        printf("--- time %d - total head movement %d - current time required %d ---\n", current_time, total_head_movement, current_time_required);

        for (i = 0; i < request_count; i++) {
            printf("%d - sector: %d - arrival - %d - arrived %d - done %d\n", i, requests[i].sector, requests[i].arrival, requests[i].arrived, requests[i].done);
        }

        // Check if all requests have been serviced
        if (requests_done == request_count) {
            done = true;
            total_service_time = current_time - first_request_arrival;
        }
    }
}

void cScan(Request *requests) {
    bool done_next_request = false;

    int closest_request = 0;
    int closest_request_index = 0;
    int current_request_distance = 0;

    unsigned int current_sector = 0;
    int current_distance = 0;
    int current_time_required = 0;

    while (!done) {
        checkForArrivedRequests(requests);

        current_distance = 0;

        if (arrived_requests_index > 0) {
            done_next_request = false;
            while (!done_next_request) {
                printf("Must find next request\n");
                // Determine the request to service
                if (movement_direction == 'a') {
                    closest_request = 10000;
                    closest_request_index = -1;
                    for (i = 0; i < arrived_requests_index; i++) {
                        if (requests[i].arrived && !requests[i].done && requests[i].sector >= head_position) {
                            current_request_distance = abs(requests[i].sector - head_position);
                            if (current_request_distance < closest_request) {
                                closest_request = current_request_distance;
                                closest_request_index = i;
                            }
                        }
                    }
                    if (closest_request_index == -1) {
                        // No request above current head position, must wrap around
                        // total_head_movement += (9999 - head_position) + 1;
                        current_distance = (9999 - head_position) + 1;
                        head_position = 0;
                    } else {
                        done_next_request = true;
                    }
                } else if (movement_direction == 'd') {
                    closest_request = 10000;
                    closest_request_index = -1;
                    for (i = 0; i < arrived_requests_index; i++) {
                        if (requests[i].arrived && !requests[i].done && requests[i].sector <= head_position) {
                            current_request_distance = abs(requests[i].sector - head_position);
                            if (current_request_distance < closest_request) {
                                closest_request = current_request_distance;
                                closest_request_index = i;
                            }
                        }
                    }
                    if (closest_request_index == -1) {
                        // No request above current head position, must wrap around
                        // total_head_movement += (head_position - 0) + 1;
                        current_distance = (head_position - 0) + 1;
                        head_position = 9999;
                    } else {
                        done_next_request = true;
                    }
                }
            }

            printf("*** Closest request: %d - request %d ***\n", closest_request, requests[closest_request_index].sector);
        }

        request_to_service = closest_request_index;

        if (requests[request_to_service].arrived && !requests[request_to_service].done) {
            current_sector = requests[request_to_service].sector;
            if (head_position == current_sector) {
                // No movement required
                current_distance += 0;
            } else if (head_position > current_sector) {
                // Must move down
                current_distance += head_position - current_sector;
            } else if (head_position < current_sector) {
                // Must move up
                current_distance += current_sector - head_position;
            }

            printf("Current distance: %d", current_distance);

            // Calculate time required and set new head position
            current_time_required = timeRequiredForRequest(current_distance, reverse_direction);
            head_position = current_sector;
            // Mark request as done
            requests[request_to_service].done = true;
            requests_done++;
            // Update time, total head movement, and move on to next request
            current_time += current_time_required;
            total_head_movement += current_distance;
            request_to_service++;
        } else {
            current_time++;
        }

        // TODO: remove this
        printf("--- time %d - total head movement %d - current time required %d ---\n", current_time, total_head_movement, current_time_required);
        for (i = 0; i < request_count; i++) {
            printf("%d - sector: %d - arrival - %d - arrived %d - done %d\n", i, requests[i].sector, requests[i].arrival, requests[i].arrived, requests[i].done);
        }

        // Check if all requests have been serviced
        if (requests_done == request_count) {
            done = true;
            total_service_time = current_time - first_request_arrival;
        }
    }
}

void look(Request *requests) {
    bool done_next_request = false;

    int closest_request = 0;
    int closest_request_index = 0;
    int current_request_distance = 0;

    unsigned int current_sector = 0;
    int current_distance = 0;
    int current_time_required = 0;

    while (!done) {
        checkForArrivedRequests(requests);

        reverse_direction = false;

        if (arrived_requests_index > requests_done) {
            done_next_request = false;
            while (!done_next_request) {
                printf("Must find next request\n");
                // Determine the request to service
                if (movement_direction == 'a') {
                    closest_request = 10000;
                    closest_request_index = -1;
                    for (i = 0; i < arrived_requests_index; i++) {
                        if (requests[i].arrived && !requests[i].done && requests[i].sector >= head_position) {
                            current_request_distance = abs(requests[i].sector - head_position);
                            if (current_request_distance < closest_request) {
                                closest_request = current_request_distance;
                                closest_request_index = i;
                            }
                        }
                    }
                    if (closest_request_index == -1) {
                        // No request above current head position, switch direction
                        movement_direction = 'd';
                        reverse_direction = true;
                    } else {
                        done_next_request = true;
                    }
                } else if (movement_direction == 'd') {
                    closest_request = 10000;
                    closest_request_index = -1;
                    for (i = 0; i < arrived_requests_index; i++) {
                        if (requests[i].arrived && !requests[i].done && requests[i].sector <= head_position) {
                            current_request_distance = abs(requests[i].sector - head_position);
                            if (current_request_distance < closest_request) {
                                closest_request = current_request_distance;
                                closest_request_index = i;
                            }
                        }
                    }
                    if (closest_request_index == -1) {
                        // No request above current head position, switch direction
                        movement_direction = 'a';
                        reverse_direction = true;
                    } else {
                        done_next_request = true;
                    }
                }
            }

            printf("*** Closest request: %d - request %d ***\n", closest_request, requests[closest_request_index].sector);
        }

        request_to_service = closest_request_index;

        if (requests[request_to_service].arrived && !requests[request_to_service].done) {
            current_sector = requests[request_to_service].sector;
            if (head_position == current_sector) {
                // No movement required
                current_distance = 0;
            } else if (head_position > current_sector) {
                // Must move down
                current_distance = head_position - current_sector;
            } else if (head_position < current_sector) {
                // Must move up
                current_distance = current_sector - head_position;
            }

            // Calculate time required and set new head position
            current_time_required = timeRequiredForRequest(current_distance, reverse_direction);
            head_position = current_sector;
            // Mark request as done
            requests[request_to_service].done = true;
            requests_done++;
            // Update time, total head movement, and move on to next request
            current_time += current_time_required;
            total_head_movement += current_distance;
            request_to_service++;
        } else {
            current_time++;
        }

        // TODO: remove this
        printf("--- time %d - total head movement %d - current time required %d ---\n", current_time, total_head_movement, current_time_required);
        for (i = 0; i < request_count; i++) {
            printf("%d - sector: %d - arrival - %d - arrived %d - done %d\n", i, requests[i].sector, requests[i].arrival, requests[i].arrived, requests[i].done);
        }

        // Check if all requests have been serviced
        if (requests_done == request_count) {
            done = true;
            total_service_time = current_time - first_request_arrival;
        }
    }
}

// Function to initialize the requests array (initial size expanded as needed)
void initRequests(Request **requests, size_t *size) {
    *size = 500;
    *requests = (Request*)malloc((*size)*sizeof(Request));
}

// Function to reallocate space for the requests (VARIABLE SIZE)
void reallocateRequests(Request **requests, size_t *size) {
    *size *= 2;
    *requests = (Request*)realloc(*requests, (*size) * sizeof(Request));
}

// Function to read in the requests from std input
int getRequests(Request **requests, size_t *requests_size) {
    int line_len = 1000;
    char line[1000] = {0};

    while (fgets(line, line_len, stdin) != NULL) {
        int token_num = 0;

        // Reallocate more space for requests if needed (VARIABLE SIZE)
        if (request_count == *requests_size) {
            reallocateRequests(requests, requests_size);
        }

        // Tokenize each line
        char *token;
        for (token = strtok(line, " \t"); token != NULL; token = strtok(NULL, " \t")) {
            if (token_num == 0) {
                (*requests)[request_count].sector = atoi(token);
            } else if (token_num == 1) {
                (*requests)[request_count].arrival = atoi(token);
            }
            token_num++;
        }
        (*requests)[request_count].arrived = false;
        (*requests)[request_count].done = false;
        request_count++;
    }

    return request_count;
}
