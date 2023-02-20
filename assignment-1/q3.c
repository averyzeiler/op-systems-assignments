/*
 Assignment 1
 Question 3
 Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
 Due: February 19th, 2023
*/

// Include necessary libraries
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <semaphore.h>
#include <stdbool.h>

// Used in case if no arguments are given when main() is called
// 5 students, 3 times they can access the TA
#define DEFAULT_NUM_STU 6
#define DEFAULT_ACCESS_AMNT 1

// GLOBAL VARIABLE DECLARATIONS
sem_t sem_ta;                           // Synchronize TA sleeping
sem_t sem_stu;                          // Synchronize students going in for help
sem_t sem_done;                         // Synchronize end of helping
pthread_mutex_t lock_ta;                // Used to lock chair list
int num_threads = DEFAULT_NUM_STU;      // Number of STUDENTS; real # of threads is num_threads + 1
int chairs[4];                          // Buffer for students to sit in; contains student numbers; INCLUDES student in TA office!
int waiting = 0;                        // Number of students waiting; INCLUDES student in TA office!
int next_idx = 0;                       // Position of next student to be helped
int next_chair = 0;                     // Next available chair; can be 0, 1, or 2
int need_help = DEFAULT_NUM_STU;        // Used to terminate program once students are done being helped
int num_turns = DEFAULT_ACCESS_AMNT;    // Number of times each student can access TA

// Student struct makes it easier to iterate through student thread creation
struct Student {
    int id;
    pthread_t tid;
};

// UTILITY FUNCTION: determines if a character can be converted to an integer
bool isInt (char num[]) {
    for (int i = 0; num[i] != 0; i++) {
        if (!isdigit(num[i]))
            return false;
    }
    return true;
}

// UTILITY FUNCTION: sets global control variables to initialize simulation
// Also note that the minimum number of students is 5!!
void setParams (int argc, char *argv[]) {
    // CASE 1: No arguments were inputted
    if (argc == 1) {
        printf("No arguments inputted, initializing to default values.\n");
    // CASE 2: Only number of students were inputted
    } else if (argc == 2) {
        printf("Only number of students inputted, initializing access to default value.\n");
        // Test if parameter is valid
        if (isInt(argv[1]) && atoi(argv[1]) >= DEFAULT_NUM_STU) {
            num_threads = atoi(argv[1]);
        } else {
            printf("Invalid input for number of students. Initializing to default.\n");
        }
    // CASE 3: All parameters were inputted
    } else {
        printf("Enough parameters were inputted.\n");
        // Test if parameters are valid
        if (isInt(argv[1]) && atoi(argv[1]) >= DEFAULT_NUM_STU) {
            num_threads = atoi(argv[1]);
        } else {
            printf("Invalid input for number of students. Initializing to default.\n");
        }
        if (isInt(argv[2])) {
            num_turns = atoi(argv[2]);
        } else {
            printf("Invalid input for access. Initializing to default.\n");
        }
    }
}

void * student (void *params) {
    int num = *(int*)params;        // To identify which student is seeking help
    int access = num_turns;         // Limits how many times students can access TA
    int work;                       // Random work time
    printf("Student number %d thread created.\n", num);
    // This part repeats until students are finished being helped!
    while (access > 0) {
        work = rand() % 10 + 1; // Generate random work time between 1 and 10
        printf("Student %d is now working for %d seconds.\n", num, work);
        sleep(work);
        printf("Student %d is now seeking help.\n", num);
        if (waiting >= 4) {
            printf("No seats available. Student %d will continue working.\n", num);
            continue;       // Begin working again and don't try to access TA
        } else if (waiting == 0) {
            // No students waiting. Student will enter office directly and notify sleeping TA
            printf("Student %d is entering office as no others are waiting.\n", num);
            sem_post(&sem_ta);
        }
        // MUTEX LOCK: modifying chair queue
        // No other student can modify chair queue until done!
        pthread_mutex_lock(&lock_ta);
        // Increase # students waiting + add to queue
        waiting++;
        chairs[next_chair] = num;
        if (waiting != 1)
            printf("Student %d sits in chair. Students waiting = %d\n", num, (waiting - 1));
        next_chair = (next_chair + 1) % 4;  // Avoid overflows
        pthread_mutex_unlock(&lock_ta);
        // MUTEX UNLOCK: other students can now modify chair queue
        // Student waits until their number is called by TA
        do {
            sem_wait(&sem_stu);
        } while (chairs[next_idx] != num);
        sem_wait(&sem_done);
        // CRUCIAL TO TERMINATE LOOP: decrement access
        access--;
    }
    printf("Student %d has finished working and is heading home.\n", num);
    pthread_exit(0);
}

void * teachingAssistant () {
    printf("Teaching assistant thread created.\n");
    int help;           // Random help time
    // TA sleeps until first student knocks at door
    printf("TA starts asleep.\n");
    sem_wait(&sem_ta);
    printf("TA is now waking up to help first student in session.\n");
    // This part repeats until all students are finished being helped!
    while (need_help > 0) {
        sem_post(&sem_stu);     // TA notifies student to enter office
        help = rand() % 10 + 1; // Generate random help time between 1 and 10
        printf("Helping student %d for %d seconds. ", chairs[next_idx], help);
        printf("Students waiting = %d.\n", (waiting - 1));
        // TA takes time to help student, then notifies student they are done
        sleep(help);
        // MUTEX LOCK: modifying chair queue
        // No student can modify chair queue until done!
        pthread_mutex_lock(&lock_ta);
        printf("Student %d is done being helped. NEXT!\n", chairs[next_idx]);
        // Next student index moves to next spot in queue as THIS student gets helped
        chairs[next_idx] = 0;
        waiting--;
        next_idx = (next_idx + 1) % 4;
        pthread_mutex_unlock(&lock_ta);
        // MUTEX UNLOCK: now students can now modify chair queue
        sem_post(&sem_done);
        if (waiting == 0) {
            // TA only sleeps if no more students are waiting
            printf("TA is going back to sleep.\n");
            sem_wait(&sem_ta);
            printf("TA is waking up again.\n");
        }
    }
    pthread_exit(0);
}

// Arguments given (also outlined in README):
    // Number of students (index 1)
    // Number of times an individual student can access the TA (index 2)
// Because of this, argc = 3 and argv has a size of 3
int main (int argc, char *argv[]){
    
    // FIRSTLY, set parameters based on command line input
    setParams(argc,argv);
    // Initialize thread IDs and attributes
    struct Student students[num_threads];
    pthread_t tid_ta;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    need_help = num_threads;            // This terminates the TA thread

    // Initialize semaphore for student availability
    if (sem_init(&sem_stu, 0, 0) != 0) {
        // Error in initializing semaphore
        printf("Error in initializing student semaphore.\n");
        return -1;
    }

    // Initialize semaphore for TA availability
    if (sem_init(&sem_ta, 0, 0) != 0) {
        // Error in initializing semaphore
        printf("Error in initializing TA semaphore.\n");
        return -2;
    }

    // Initialize semaphore for TA availability
    if (sem_init(&sem_done, 0, 0) != 0) {
        // Error in initializing semaphore
        printf("Error in initializing TA semaphore.\n");
        return -2;
    }

    // Initialize mutex to lock TA resource
    if (pthread_mutex_init(&lock_ta, NULL) != 0) {
        // Error in initializing mutex lock
        printf("Error in initializing TA mutex lock.\n");
        return -3;
    }
    
    // Create TA thread
    if (pthread_create(&tid_ta, &attr, teachingAssistant, NULL) != 0) {
        // Error in creating TA thread
        printf("Error in creating TA thread.\n");
        return -(num_threads + 2);
    }

    // Create all student threads
    for (int i = 0; i < num_threads; i++) {
        students[i].id = i + 1;
        if (pthread_create(&students[i].tid, &attr, student, (void*) &students[i].id) != 0) {
            // Error in creating student thread
            printf("Error in creating student thread %d.\n", students[i].id);
            return -(i + 4);
        }
    }
    
    // Now, the simulation runs until all students are done working!
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(students[i].tid, NULL) != 0) {
            printf("Error in joining student thread %d.\n", i);
            return i + 1;
        }
        printf("Student %d thread has been joined.\n", (i+1));
        need_help--;
    }
    sem_post(&sem_ta);
    if (pthread_join(tid_ta,NULL) != 0) {
        printf("Error in joining TA thread.\n");
        return num_threads + 1;
    }
    // At this point, simulation has terminated successfully.
    printf("TA is finished working and is heading home.\n");
    sem_destroy(&sem_stu);
    sem_destroy(&sem_ta);
    sem_destroy(&sem_done);
    return 0;
}
