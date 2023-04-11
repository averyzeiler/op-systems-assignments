/*
    Assignment 3
    Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
    Due: April 14th, 2023
*/

// Refer to chapter 11 slides!

/*
    - Service a disk with 300 cylinders (0 to 299)
    - Service 20 requests from request.bin (requests = cylinder numbers, stored as 4-byte ints)
    - 2 command line arguments
        - First argument: initial position of disk head (int from 0-299)
        - Second argument: direction of head (LEFT or RIGHT)
*/

/*
    PROCEDURE
    1. Read command line arguments
    2. Read requests from requests.bin and store in an int array, then close file
    3. (not really a step) Write functions for each disk scheduling algorithm
    4. FCFS + SSTF algorithms will service requests in int array from step 2 + compute head movements
    5. Sort requests array in increasing order + store in another int array
        - Then, remaining 4 algorithms will service requests in sorted array + compute head movements
    ** WE CAN USE WHATEVER SORTING ALGORITHM WE WANT

    OUTPUT
    - Requests in order in which they are serviced by each algorithm
    - Total amount of head movement incurred by EACH algorithm
    ** THIS OUTPUT REPEATED FOR EACH ALGORITHM!
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// https://www.geeksforgeeks.org/enumeration-enum-c/
enum direction {LEFT = -1, RIGHT = 1};

#define NUM_REQUESTS 20
#define DISK_SIZE 300
#define DISK_MAX DISK_SIZE - 1
#define INT_SIZE 4
#define DEFAULT_INITIAL 150
#define DEFAULT_DIR RIGHT

// NOTE: all algorithms will return total amount of head movement

int FCFS (int* requests, int initial) {
    int sum = 0;
    int tmp = initial;
    printf("FCFS [FIRST COME FIRST SERVE]\n\n");
    for (int i = 0; i < NUM_REQUESTS; i++) {
        printf("%d -> ", tmp);
        sum += abs(requests[i] - tmp);
        tmp = requests[i];
    }
    printf("%d\n\n", tmp);
    return sum;
}

int SSTF (int* requests, int initial) {
    int sum = 0;
    int tmp = initial;
    int min = DISK_SIZE;
    int choose = 0;             // Will represent INDEX of next request to service
    printf("SSTF [SHORTEST SEEK TIME FIRST]\n\n");
    // NOTE: using O(n^2) algorithm because list size = 20, however would change this if there were more requests.
    for (int i = 0; i < NUM_REQUESTS; i++) {
        for (int j = 0; j < NUM_REQUESTS; j++) {
            // If new minimum head movement found, store values for movement time and index of request
            if (requests[j] != -1 && (abs(requests[j] - tmp) < min)) {
                min = abs(requests[j] - tmp);
                choose = j;
            }
        }
        sum += min;
        min = DISK_SIZE;        // Reset so can start whole process again
        tmp = requests[choose];
        printf("%d -> ", tmp);
        requests[choose] = -1;  // Remove serviced request from list
    }
    printf("%d\n\n", tmp);
    return sum;
}

// UTILITY FUNCTION: find index of element in arrray that is next to initial, given direction
int findStart (int* requests, int initial, enum direction dir) {
    int start_at;
    if (dir == LEFT) {
        // Ensure that we can still use SCAN if smaller element isn't found
        start_at = 0;
        for (int i = NUM_REQUESTS - 1; i >= 0; i--) {
            if (initial > requests[i]) {
                start_at = i;
                break;
            }
        }
    } else {
        // Ensure that we can still use SCAN if larger element isn't found
        start_at = NUM_REQUESTS - 1;
        for (int i = 0; i < NUM_REQUESTS; i++) {
            if (initial < requests[i]) {
                start_at = i;
                break;
            }
        }
    }
    return start_at;
}

int SCAN (int* requests, int initial, enum direction dir) {
    printf("SCAN ALGORITHM\n\n");
    int sum = 0;
    int start_at = findStart(requests, initial, dir);
    int temp = initial;
    int inc = (int) dir;    // To increment/decrement i
    int i = start_at;       // Index of traversal
    // Next, traverse array in direction given
    while (i >= 0 && i < NUM_REQUESTS) {
        printf("%d -> ", temp);
        //sum += abs(temp - requests[i]);
        temp = requests[i];
        i += inc;
    }
    printf("%d -> ", temp);
    // Move disk head so it starts at the next available request
    if (dir == LEFT) {
        //sum += temp;
        i = start_at + 1;
    } else {
        //sum += NUM_REQUESTS - temp - 1;
        i = start_at - 1;
    }
    temp = requests[i];
    // Next, reverse direction and continue until full list has been traversed
    while (i >= 0 && i < NUM_REQUESTS) {
        printf("%d -> ", temp);
        temp = requests[i];
        i -= inc;
    }
    // Finally, calculate sum
    if (dir == LEFT) {
        // Sum = |initial - 0| + |0 - final| = initial + final
        printf("%d\n", requests[NUM_REQUESTS - 1]);
        sum = initial + requests[NUM_REQUESTS - 1];
    } else {
        // Sum = |DISK_MAX - initial| + |DISK_MAX - final|
        printf("%d\n", requests[0]);
        sum = DISK_MAX * 2 - initial - requests[0];
    }
    return sum;
}

int CSCAN (int* requests, int initial, enum direction dir) {
    printf("C-SCAN ALGORITHM\n\n");
    int sum = 0;
    int start_at = findStart(requests, initial, dir);
    int temp = initial;
    int inc = (int) dir;    // To increment/decrement i
    int i = start_at;       // Index of traversal
    // Next, traverse array in direction given
    while (i >= 0 && i < NUM_REQUESTS) {
        printf("%d -> ", temp);
        //sum += abs(temp - requests[i]);
        temp = requests[i];
        i += inc;
    }
    printf("%d -> ", temp);
    // Move disk head so it starts at the next available request
    if (dir == LEFT) {
        i = NUM_REQUESTS - 1;
    } else {
        i = 0;
    }
    temp = requests[i];
    // Next, travel to opposite end of disk and continue in same direction until all requests serviced
    while (i != start_at) {
        printf("%d -> ", temp);
        //sum += abs(temp - requests[i]);
        temp = requests[i];
        i += inc;
    }
    // Finally, calculate sum
    if (dir == LEFT) {
        // sum = |initial - 0| + |0 - DISK_MAX| + |DISK_MAX - final| = initial + DISK_MAX * 2 - final
        sum = DISK_MAX * 2 + initial - requests[start_at + 1];
    } else {
        // sum = |initial - DISK_MAX| + |DISK_MAX - 0| + |0 - final| = DISK_MAX * 2 + final - initial
        sum = DISK_MAX * 2 + requests[start_at - 1] - initial;
    }
    return sum;
}

int LOOK (int* requests, int initial, enum direction dir) {
    printf("LOOK ALGORITHM\n\n");
    int sum = 0;
    int start_at = findStart(requests, initial, dir);
    int temp = initial;
    int inc = (int) dir;    // To increment/decrement i
    int i = start_at;       // Index of traversal
    // Next, traverse array in direction given
    while (i >= 0 && i < NUM_REQUESTS) {
        printf("%d -> ", temp);
        //sum += abs(temp - requests[i]);
        temp = requests[i];
        i += inc;
    }
    printf("%d -> ", temp);
    // Move disk head so it starts at the next available request
    if (dir == LEFT) {
        //sum += temp;
        i = start_at + 1;
    } else {
        //sum += NUM_REQUESTS - temp - 1;
        i = start_at - 1;
    }
    temp = requests[i];
    // Next, reverse direction and continue until full list has been traversed
    while (i >= 0 && i < NUM_REQUESTS) {
        printf("%d -> ", temp);
        temp = requests[i];
        i -= inc;
    }
    // Finally, calculate sum
    if (dir == LEFT) {
        // Sum = |initial - min| + |min - final| = initial + final - min * 2
        printf("%d\n", requests[NUM_REQUESTS - 1]);
        sum = initial + requests[NUM_REQUESTS - 1] - requests[0] * 2;
    } else {
        // Sum = |max - initial| + |max - final| = max * 2 - initial - final
        printf("%d\n", requests[0]);
        sum = requests[NUM_REQUESTS - 1] * 2 - initial - requests[0];
    }
    return sum;
}

int CLOOK (int* requests, int initial, enum direction dir) {
    printf("C-LOOK ALGORITHM\n\n");
    int sum = 0;
    int start_at = findStart(requests, initial, dir);
    int temp = initial;
    int inc = (int) dir;    // To increment/decrement i
    int i = start_at;       // Index of traversal
    // Next, traverse array in direction given
    while (i >= 0 && i < NUM_REQUESTS) {
        printf("%d -> ", temp);
        //sum += abs(temp - requests[i]);
        temp = requests[i];
        i += inc;
    }
    printf("%d -> ", temp);
    // Move disk head so it starts at the next available request
    if (dir == LEFT) {
        i = NUM_REQUESTS - 1;
    } else {
        i = 0;
    }
    temp = requests[i];
    // Next, travel to opposite end of disk and continue in same direction until all requests serviced
    while (i != start_at) {
        printf("%d -> ", temp);
        //sum += abs(temp - requests[i]);
        temp = requests[i];
        i += inc;
    }
    // Finally, calculate sum
    if (dir == LEFT) {
        // sum = |initial - min| + |min - max| + |max - final| = initial + max * 2 - final - min * 2
        sum = requests[NUM_REQUESTS - 1] * 2 + initial - requests[start_at + 1] - requests[0] * 2;
    } else {
        // sum = |initial - max| + |max - min| + |min - final| = max * 2 + final - initial - min * 2
        sum = requests[NUM_REQUESTS - 1] * 2 + requests[start_at - 1] - initial - requests[0] * 2;
    }
    return sum;
}

// sort in ASCENDING order
// will use SELECTION SORT algorithm: https://www.geeksforgeeks.org/selection-sort/
int* sortArray (int* unsorted) {
    int sorted[NUM_REQUESTS];
    int min_idx;
    for (int i = 0; i < NUM_REQUESTS; i++) {
        // Copy unsorted array into sorted array
        sorted[i] = unsorted[i];
    }
    for (int i = 0; i < NUM_REQUESTS; i++) {
        min_idx = i;
        for (int j = i + 1; j < NUM_REQUESTS; j++) {
            // Find index of minimum value in array
            if (sorted[j] < sorted[min_idx]) {
                min_idx = j;
            }
        }
        // Swap minimum element with element @ current index if not already done
        if (min_idx != i) {
            int tmp = sorted[min_idx];
            sorted[min_idx] = sorted[i];
            sorted[i] = tmp;
        }
    }
    return sorted;
}

int main (int argc, char *argv[]) {
    int initial = DEFAULT_INITIAL;
    enum direction dir = DEFAULT_DIR;
    // INTERPRET ARGUMENTS: can only use input arguments if argc >= 2 
    if (argc >= 2) {
        // Check if within acceptable range
        if ((atoi(argv[1]) >= 0) && (atoi(argv[1]) <= DISK_MAX)) {
            initial = atoi(argv[1]);
        }
    }
    if (argc >= 3) {
        // Only need to change if direction is left
        if (argv[3] == "LEFT") {
            dir = LEFT;
        }
    }
}