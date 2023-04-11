/*
    Assignment 3
    Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
    Due: April 16th, 2023
*/

// Refer to chapter 11 slides!

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// https://www.geeksforgeeks.org/enumeration-enum-c/
enum direction {LEFT = -1, RIGHT = 1};

#define NUM_REQUESTS 20
#define DISK_SIZE 300
#define DISK_MAX DISK_SIZE - 1
#define INT_SIZE 4
#define DEFAULT_INITIAL 150
#define DEFAULT_DIR RIGHT
#define INT_SIZE 1
#define MEM_SIZE INT_SIZE * NUM_REQUESTS
#define BUFFER_SIZE 4

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
    int temp_arr[NUM_REQUESTS];
    int choose = 0;             // Will represent INDEX of next request to service
    printf("SSTF [SHORTEST SEEK TIME FIRST]\n\n");
    // First, make copy of array
    for (int i = 0; i < NUM_REQUESTS; i++) {
        temp_arr[i] = requests[i];
    }
    // NOTE: using O(n^2) algorithm because list size = 20, however would change this if there were more requests.
    for (int i = 0; i < NUM_REQUESTS; i++) {
        for (int j = 0; j < NUM_REQUESTS; j++) {
            // If new minimum head movement found, store values for movement time and index of request
            if (temp_arr[j] != -1 && (abs(temp_arr[j] - tmp) < min)) {
                min = abs(temp_arr[j] - tmp);
                choose = j;
            }
        }
        sum += min;
        min = DISK_SIZE;        // Reset so can start whole process again
        tmp = temp_arr[choose];
        printf("%d -> ", tmp);
        temp_arr[choose] = -1;  // Remove serviced request from list
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
            if (initial >= requests[i]) {
                start_at = i;
                break;
            }
        }
    } else {
        // Ensure that we can still use SCAN if larger element isn't found
        start_at = NUM_REQUESTS - 1;
        for (int i = 0; i < NUM_REQUESTS; i++) {
            if (initial <= requests[i]) {
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
    printf("%d -> ", temp);
    while (i >= 0 && i < NUM_REQUESTS) {
        if (temp != initial) { printf("%d -> ", temp); }
        temp = requests[i];
        i += inc;
    }
    printf("%d -> ", temp);
    // Move disk head so it starts at the next available request
    i = start_at - inc;
    // Next, reverse direction and continue until full list has been traversed
    while (i >= 0 && i < NUM_REQUESTS) {
        temp = requests[i];        
        if (temp != initial) { printf("%d -> ", temp); }
        i -= inc;
    }
    printf("END\n");
    // Finally, calculate sum
    if (dir == LEFT) {
        // Sum = |initial - 0| + |0 - final| = initial + final
        sum = initial + requests[NUM_REQUESTS - 1];
    } else {
        // Sum = |initial - DISK_MAX| + |DISK_MAX - final|
        sum = DISK_MAX + DISK_MAX - initial - requests[0];
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
    printf("%d -> ", temp);
    // Next, traverse array in direction given
    while (i >= 0 && i < NUM_REQUESTS) {
        if (temp != initial) { printf("%d -> ", temp); }
        temp = requests[i];
        i += inc;
    }
    printf("%d -> ", temp);
    // Move disk head so it starts at the next available request
    if (dir == LEFT) { i = NUM_REQUESTS - 1; } else { i = 0; }
    // Next, travel to opposite end of disk and continue in same direction until all requests serviced
    while (i != start_at) {
        temp = requests[i];        
        if (temp != initial) { printf("%d -> ", temp); }
        i += inc;
    }
    printf("END\n");
    // Finally, calculate sum
    if (dir == LEFT) {
        // sum = |initial - 0| + |0 - DISK_MAX| + |DISK_MAX - final| = initial + DISK_MAX * 2 - final
        sum = DISK_MAX + DISK_MAX + initial - requests[start_at + 1];
    } else {
        // sum = |initial - DISK_MAX| + |DISK_MAX - 0| + |0 - final| = DISK_MAX * 2 + final - initial
        sum = DISK_MAX + DISK_MAX + requests[start_at - 1] - initial;
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
    printf("%d -> ", temp);
    // Next, traverse array in direction given
    while (i >= 0 && i < NUM_REQUESTS) {
        if (temp != initial) { printf("%d -> ", temp); }
        temp = requests[i];
        i += inc;
    }
    printf("%d -> ", temp);
    // Move disk head so it starts at the next available request
    i = start_at - inc;
    // Next, reverse direction and continue until full list has been traversed
    while (i >= 0 && i < NUM_REQUESTS) {
        temp = requests[i];        
        if (temp != initial) { printf("%d -> ", temp); }
        i -= inc;
    }
    printf("END\n");
    // Finally, calculate sum
    if (dir == LEFT) {
        // Sum = |initial - min| + |min - final| = initial + final - min * 2
        sum = initial + requests[NUM_REQUESTS - 1] - requests[0] - requests[0];
    } else {
        // Sum = |max - initial| + |max - final| = max * 2 - initial - final
        sum = requests[NUM_REQUESTS - 1] + requests[NUM_REQUESTS - 1] - initial - requests[0];
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
    printf("%d -> ", temp);
    // Next, traverse array in direction given
    while (i >= 0 && i < NUM_REQUESTS) {
        if (temp != initial) { printf("%d -> ", temp); }
        //sum += abs(temp - requests[i]);
        temp = requests[i];
        i += inc;
    }
    printf("%d -> ", temp);
    // Move disk head so it starts at the next available request
    if (dir == LEFT) { i = NUM_REQUESTS - 1; } else { i = 0; }
    // Next, travel to opposite end of disk and continue in same direction until all requests serviced
    while (i != start_at) {
        temp = requests[i];
        if (temp != initial) { printf("%d -> ", temp); }
        i += inc;
    }
    printf("END\n");
    // Finally, calculate sum
    if (dir == LEFT) {
        // sum = |initial - min| + |min - max| + |max - final| = initial + max * 2 - final - min * 2
        sum = requests[NUM_REQUESTS - 1] + requests[NUM_REQUESTS - 1] + initial - requests[start_at + 1] - requests[0] - requests[0];
    } else {
        // sum = |initial - max| + |max - min| + |min - final| = max * 2 + final - initial - min * 2
        sum = requests[NUM_REQUESTS - 1] + requests[NUM_REQUESTS - 1] + requests[start_at - 1] - initial - requests[0] - requests[0];
    }
    return sum;
}

// sort in ASCENDING order
// will use SELECTION SORT algorithm: https://www.geeksforgeeks.org/selection-sort/
int* sortArray (int unsorted[]) {
    int *sorted = malloc(NUM_REQUESTS*sizeof(int));
    if (sorted == NULL) {
        printf("Memory allocation failed\n");
        return unsorted;
    }
    int min_idx;
    for (int i = 0; i < NUM_REQUESTS; i++) {
        // Copy unsorted array into sorted array
        sorted[i] = unsorted[i];
    }
    for (int i = 0; i < NUM_REQUESTS - 1; i++) {
        min_idx = i;
        for (int j = i + 1; j < NUM_REQUESTS; j++) {
            // Find index of minimum value in array
            if (sorted[j] < sorted[min_idx]) {
                min_idx = j;
            }
        }
        // Swap minimum element with element @ current index
        int tmp = sorted[i];
        sorted[i] = sorted[min_idx];
        sorted[min_idx] = tmp;
    }
    return sorted;
}

int main (int argc, char *argv[]) {
    int initial = DEFAULT_INITIAL;
    enum direction dir = DEFAULT_DIR;
    char tmp[] = "RIGHT";
    char buff[BUFFER_SIZE];
    // INTERPRET ARGUMENTS: can only use input arguments if argc >= 2 
    if (argc >= 2) {
        // Check if within acceptable range
        if ((atoi(argv[1]) >= 0) && (atoi(argv[1]) <= DISK_MAX)) {
            initial = atoi(argv[1]);
        }
    }
    if (argc >= 3) {
        // Only need to change if direction is left
        if (strcmp(argv[2], "LEFT") == 0) {
            dir = LEFT;
            strcpy(tmp, "LEFT");
        }
    }
    printf("Total requests = %d\nInitial head position = %d\nDirection of head = %s\n\n", NUM_REQUESTS, initial, tmp);
    // Now, open request.bin and read all requests
    int requests[NUM_REQUESTS];
    FILE *fptr = fopen("request.bin", "rb");
    int i = 0;
    while (fread(buff, BUFFER_SIZE, 1, fptr) == 1) {
        requests[i] = *((int*)buff);
        i++;
        if (i >= NUM_REQUESTS) {
            break;
        }
    }
    for (int i = 0; i < NUM_REQUESTS; i++) {
        printf("%d ", requests[i]);
    }
    printf("\n");
    fclose(fptr);
    // Now, perform sorting algorithms!
    int sums[6];
    sums[0] = FCFS(requests, initial);
    printf("FCFS - Total head movements = %d\n\n", sums[0]);
    sums[1] = SSTF(requests, initial);
    printf("SSTF - Total head movements = %d\n\n", sums[1]);
    int *sorted = sortArray(requests);
    if (sorted == NULL) {
        printf("EPIC FAIL!\n");
        return 1;
    }
    sums[2] = SCAN(sorted, initial, dir);
    printf("SCAN - Total head movements = %d\n\n", sums[2]);
    sums[3] = CSCAN(sorted, initial, dir);
    printf("C-SCAN - Total head movements = %d\n\n", sums[3]);
    sums[4] = LOOK(sorted, initial, dir);
    printf("LOOK - Total head movements = %d\n\n", sums[4]);
    sums[5] = CLOOK(sorted, initial, dir);
    printf("C-LOOK - Total head movements = %d\n\n", sums[5]);
    free(sorted);
    return 0;
}