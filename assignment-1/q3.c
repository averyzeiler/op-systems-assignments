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
char arr[20] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
int threads =2;
struct sName{
    int from_index;
    int to_index;
    int sum;
};

void func (voidparams){
    struct sName * param = (struct sName ) params;
    int i;
    for (i = param->from_index; i <= param->to_index; i++){
        param->sum = param->sum+arr[i];
    }
}

int main (int argc, charargv[]){
    struct sName s1 = malloc(sizeof(struct sName));
    struct sNames2 = malloc(sizeof(struct sName));
    s1 -> from_index =0;
    s1 -> to_index =9;
    s1 -> sum =0;
    s2 -> from_index =10;
    s2 -> to_index =19;
    s2 -> sum =0;
    pthread_t tid[2];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid[0],&attr, func, s1);
    pthread_create(&tid[1],&attr, func, s2);

    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    printf("Sum = %d\n", s1->sum + s2->sum);
}