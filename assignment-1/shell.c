//question 2

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80 //max length of command

// parts 1 and 2
int main(void)
{ 
    char *args[MAX_LINE/2 + 1];
    char input[MAX_LINE]; //array to hold inputs
    int should_run = 1; //flag to determine when to exit program
    pid_t pid;
    int ampersand;  //& symbol determines wait/concurrent executions

    while (should_run){
        printf("osh>");
        fflush(stdout);
        readInput(input, args, &ampersand); //reads input
        pid = fork();

        if (pid < 0) //if the fork is unsuccessful, exit
            exit(1);

        else if (pid == 0){ //if the fork is successful 
            if (ampersand == 0) //if there is no &
                wait(NULL); // parent waits while child executes
        }
    }

    return 0;
}








