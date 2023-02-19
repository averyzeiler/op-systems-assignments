//question 2

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80 //max length of command

/////////////////////////// parts 1 and 2
int main(void)
{ 
    char *args[MAX_LINE/2 + 1];
    char input[MAX_LINE]; //array to hold inputs
    int should_run = 1; //flag to determine when to exit program
    pid_t pid;
    int ampersand = 0;  //& symbol determines wait/concurrent executions

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

int commandCount = 0; //keeps track of the number of commands entered

void readInput(char input[], char *args[], int *ampersand){
    int parsing = read(STDIN_FILENO, input, MAX_LINE); //reading user input 
    for (int i = 0; i < parsing; i++){
        if (input[i] == '&')
            *ampersand = 1;
    }

    if (strcmp(args[0], "history") == 0){
        if (commandCount > 0)
            history();
        else
        printf("\n No commands in history.");

    }
}
/////////////////////////// printing history 

char historyArr[5][MAX_LINE]; //array of length 5 to store most recent commands

void history(){
    int historyCount = commandCount; //number of history is equal to number of commands
    int j = 0;
    for (int i = 0; i<5; i++){
        printf("%d. ", historyCount); //print the command number
        while (historyArr[i][j] != '\0' && historyArr[i][j] != '\n'){ //while the nect character is not a new line or null
            j++;
            printf("%c", historyArr[i][j]); //print the command
        }

        printf("\n"); //new line
        historyCount --; //history listed from last to first
        j = 0;
        if (historyCount == 0) //if at the last entry, break
            break;
            
    }
}








