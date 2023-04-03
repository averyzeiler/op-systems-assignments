/*
    Assignment 3
    Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
    Due: April 14th, 2023
*/

/*
    IMPLEMENT FOLLOWING ALGORITHMS:
    - FCFS [5 marks]: first-come-first-served, so in order of requests.bin
        - Head movement = abs(loc1 - loc2) + abs(loc2 - loc3) + ... + abs(loc[n-1] - locn)
    - SSTF [10 marks]: choose pending request closest to current head position
        - Looks like SJF, so means may cause starvation
        - Look for smallest difference between start + all requests, then replace start with the next serviced request + repeat
    - SCAN [10 marks]: move in direction of head until end of disk, then move back in other direction
        - Think of it like an ELEVATOR; goes all the way up then all the way down or vice versa
    - C-SCAN [5 marks]: move in direction of head until end of disk, then go back to BEGINNING and continue in same direction
        - However this means we MUST add the # of cylinders to the total bc it traverses all the way back
    - LOOK [5 marks]: starts in direction of head, then reverses once it reaches its furthest location
        - Difference from SCAN is that it does NOT go to end of disk!
    - C-LOOK [5 marks]: similar to C-SCAN, but also does not go ALL the way to extremity of disk, just to furthest request
    *** TOTAL 40 MARKS
    Refer to chapter 11 slides!
*/

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

#define NUM_REQUESTS 20
#define DISK_SIZE 300
#define INT_SIZE 4

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
            if (requests[j] != -1) {
                if (abs(requests[j] - tmp) < min) {
                    min = abs(requests[j] - tmp);
                    choose = j;
                }
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

int SCAN (int* requests, int initial) {

}

int CSCAN (int* requests, int initial) {

}

int LOOK (int* requests, int initial) {

}

int CLOOK (int* requests, int initial) {

}

int* sortArray (int* unsorted) {
    
}

int main (int argc, char *argv[]) {

}