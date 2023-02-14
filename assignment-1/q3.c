/*
 Assignment 1
 Question 3
 Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
 Due: February 15th, 2023
*/

// Include necessary libraries
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <semaphore.h>
#include <stdbool.h>

// Used in case if no arguments are given when main() is called
// 6 students, 3 times they can access the TA
#define DEFAULT_NUM_STU = 6
#define DEFAULT_ACCESS_AMNT = 3

// stu_num used mostly for debugging, access_amnt for terminating program
struct stu {
    int stu_num;
    int access_amnt;
};

sem_t sem_stu;              // Indicates if students need help
sem_t sem_ta;               // Indicates if TA is available
pthread_mutex_t lock_ta;    // Used to lock TA resource

int num_threads;                    // Number of STUDENTS; real # of threads is num_threads + 1
int chairs[3];                      // Buffer for students to sit in; contains student numbers
int students_waiting = 0;           // Number of students waiting
int next_student = 0;               // Position of next student to be helped
int next_chair = 0;                 // Next available chair; can be 0, 1, or 2
bool ta_asleep = false;             // TA begins awake!
int need_help = DEFAULT_NUM_STU;    // Used to terminate program once students are done being helped

// UTILITY FUNCTION: determines if a character can be converted to an integer
bool isInt (char num[]) {
    for (int i = 0; num[i] != 0; i++) {
        if (!isdigit(num[i]))
            return false;
    }
    return true;
}

// UTILITY FUNCTION: check if a student is waiting in a chair
bool isWaiting(int id) {
    for (int i = 0; i < 3; i++) {
        if (chairs[i] == id)
            return true;
    }
    return false;
}

void student (void params) {
    int num = ((struct stu*)params)->stu_num + 1;
    int access = ((struct stu*)params)->access_amnt;
    int work;
    printf("Student number %d thread created.\n", num);
    // This part repeats until students are finished being helped!
    while (access > 0) {
        // CASE 1: Student is currently waiting in a chair
        if (isWaiting(num)) 
            continue;
        // Student is NOT waiting in a chair
        work = rand() % 10; // Generate random work time between 0 and 9
        printf("Student %d is now working for %d seconds.\n", num, work);
        sleep(work);
        pthread_mutex_lock(&lock_ta);
        printf("Student %d is now seeking help.\n", num);
        // Student is now seeking help
        if (students_waiting < 3) {
            // CASE 2A: Student is able to wait in the hallway
            students_waiting++;
            chairs[next_chair] = num;
            printf("Student %d is now waiting in the hallway. Students waiting = %d.\n", num, students_waiting);
            next_chair = (next_chair + 1) % 3;  // Avoid overflows
            pthread_mutex_unlock(&lock_ta);
            // Now, wake TA if asleep
            sem_post(&sem_stu);
            sem_wait(&sem_ta);
            // CRUCIAL TO TERMINATE LOOP: decrement access
            access--;
        } else {
            // CASE 2B: No chairs are available
            pthread_mutex_unlock(&lock_ta);
            printf("No chairs available. Student %d will try again later.\n", num);
        } 
    }
    return;
}

void teachingAssistant () {
    printf("Teaching assistant thread created.\n");
    int help;
    // This part repeats until all students are finished being helped!
    while (need_help > 0) {
        if (students_waiting > 0) {
            // CASE 1: Students are waiting to be helped
            // TA awakes, pauses student interruptions + locks TA resource
            ta_asleep = false;
            sem_wait(&sem_stu);
            pthread_mutex_lock(&lock_ta);
            // Time to help student!
            help = rand() % 10; // Generate random help time between 0 and 9
            printf("Helping %d student for %d seconds.\n", chairs[next_student], help);
            printf("There are still %d students waiting.\n", (students_waiting - 1));
            // Move to next student in line
            chairs[next_student] = 0;
            students_waiting--;
            next_student = (next_student + 1) % 3;
            // TA takes time to help student, then notifies their availability!
            sleep(help);
            pthread_mutex_unlock(&lock_ta);
            sem_post(&sem_ta);
        } else if (!ta_asleep) {
            // CASE 2: Students are waiting to be helped
            printf("No students waiting. TA is now asleep.\n");
            ta_asleep = true;
        }
    }
    return;
}

// Arguments given:
    // Number of students (index 1)
    // Number of times an individual student can access the TA (index 2)
// Because of this, argc = 3 and argv has a size of 3
int main (int argc, char *argv[]){

    // Initialize number of students + number of times student can access TA
    struct stu *tmp = (struct stu *)malloc(sizeof(struct stu));
    num_threads = DEFAULT_NUM_STU;
    tmp->access_amnt = DEFAULT_ACCESS_AMNT;
    // CASE 1: No arguments were inputted
    if (argc == 1) {
        printf("No arguments inputted, initializing to default values.\n");
    // CASE 2: Only number of students were inputted
    } else if (argc == 2) {
        printf("Only number of students inputted, initializing access to default value.\n");
        // Test if parameter is valid
        if (isInt(argv[1])) {
            num_threads = atoi(argv[1]);
        } else {
            printf("Invalid input for number of students. Initializing to default.\n");
        }
    // CASE 3: All parameters were inputted
    } else {
        printf("Enough parameters were inputted.\n");
        // Test if parameters are valid
        if (isInt(argv[1])) {
            num_threads = atoi(argv[1]);
        } else {
            printf("Invalid input for number of students. Initializing to default.\n");
        }
        if (isInt(argv[2])) {
            tmp->access_amnt = atoi(argv[2]);
        } else {
            printf("Invalid input for access. Initializing to default.\n");
        }
    }

    // Initialize thread IDs and attributes
    pthread_t tid_s[num_threads];
    pthread_t tid_ta;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    need_help = num_threads;
    
    // Initialize semaphore for student availability
    if (sem_init(&sem_stu, 0, 0) != 0) {
        // Error in initializing semaphore
        printf("Error in initializing student semaphore.\n");
        return -1;
    }

    // Initialize semaphore for TA availability
    if (sem_init(&sem_ta, 0, 1) != 0) {
        // Error in initializing semaphore
        printf("Error in initializing TA semaphore.\n");
        return -2;
    }

    // Initialize mutex to lock TA resource
    if (pthread_mutex_init(&lock_ta, NULL) != 0) {
        // Error in initializing mutex lock
        printf("Error in initializing mutex lock.\n");
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
        tmp->stu_num = i;
        if (pthread_create(&tid_s[i], &attr, student, (void *)tmp) != 0) {
            // Error in creating student thread
            printf("Error in creating student thread %d.\n", i);
            return -(i + 4);
        }
    }

    // Now, the simulation runs until all students are done working!
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(tid_s[i], NULL) != 0) {
            printf("Error in joining student thread %d.\n", i);
            return i + 1;
        }
        need_help--;
    }
    if (pthread_join(tid_ta,NULL) != 0) {
        printf("Error in joining TA thread.\n");
        return num_threads + 1;
    }
    // At this point, simulation has terminated successfully.
    return 0;
}