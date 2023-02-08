/*
 Assignment 1
 Question 3
 Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
 Due: February 15th, 2023
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

/*
REQUIREMENTS
- Start by creating n students; each student runs as a separate thread
- TA also runs as a separate thread (so threads = n+1)
- Students alternate btwn programming for a period of time + seeking TA help
- TA available -> get TA help
- TA unavailable but chairs open -> sit in chair
- TA unavailable and no chairs -> keep programming + seek help later
- TA sleeps if no students need help
- Students notify sleeping TA using a semaphore
- After TA helps student, they check if there's any students left
- If YES, help NEXT student
- If NO, take a nap
- Have threads + help time be RANDOM!
*/

/*
MUTEX LOCKS
- Only 1 thread can use a shared resource at once
- Context switch -> other threads are UNBLOCKED
- Unblocked just means that ???
- Mutex lock can only be released by the thread that locked it
- Thus we can make the TA the one who does the mutex locks
https://www.thegeekstuff.com/2012/05/c-mutex-examples/
*/

/*
SEMAPHORES
- Semaphore is an integer
- Usually, # of resources available = initial value of a semaphore
- Doesn't require busy waiting!!
- Wait operation: decrements semaphore if resources available, sleeps if no resources available
- Signal operation: selects + wakes up a process if there are processes sleeping on semaphore, otherwise increments semaphore
    - Incrementing semaphore -> "hey resources are available!!! use me!"
https://www.geeksforgeeks.org/use-posix-semaphores-c/
https://www.baeldung.com/cs/semaphore
*/

/*
REQUIREMENTS FOR CS PROBLEM
1. Mutual exclusion (only 1 process executing critical section at once)
2. If no other process executing CS + a process wishes to enter CS:
- Only processes not executing remainder can decide which process enters CS next
- Decision must take a finite amount of time
3. Bound must exist on # of times other processes can enter CS

CS HERE is when TA is helping STUDENT; STUDENT's critical section.
*/
int threads = 0;
sem_t sem;
struct sName{
    int from_index;
    int to_index;
    int sum;
};
/*
void func (voidparams){
    struct sName * param = (struct sName ) params;
    int i;
    for (i = param->from_index; i <= param->to_index; i++){
        param->sum = param->sum+arr[i];
    }
}*/
void student (void params) {
    int num = atoi(param);
    printf("Student number %d thread created.\n", num);
}

void teachingAssistant (void params) {
    printf("Teaching assistant thread created.\n");
}

int main (int argc, char *argv[]){
    //Arguments given: number of students!
    //Because of this, argc = 2 and argv has a size of 2
    num_threads = argv[1];
    pthread_t tid_s[num_threads];
    pthread_t tid_ta;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    //Create all student threads
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&tid_s[i], &attr, student, i) != 0) {
            //Error in creating student thread
            printf("Error in creating student thread %d.\n", i);
        }
    }
    if (pthread_create(&tid_ta, &attr, teachingAssistant, num_threads) != 0) {
        //Error in creating TA thread
        printf("Error in creating TA thread.\n");
    }
    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    printf("Sum = %d\n", s1->sum + s2->sum);
}