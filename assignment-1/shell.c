/*
    Assignment 1
    Question 2
    Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
    Due: February 19th, 2023
*/

/*
    REFERENCES:
    Practice Lab 3 Part 1 (forking processes)
    Test code for this question (uploaded on Teams in the Assignments channel)
    Linux manual for execvp functionality: https://linux.die.net/man/3/explain_execvp
    String.h string library: https://www.tutorialspoint.com/c_standard_library/string_h.htm
    Fgets in standard library: https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
    Operating Systems Concepts by Abraham Silberschatz, Peter Baaer Galvin, Greg Gagne
    https://drive.uqu.edu.sa/_/mskhayat/files/MySubjects/2017SS%20Operating%20Systems/Abraham%20Silberschatz-Operating%20System%20Concepts%20(9th,2012_12).pdf
    C library Functions
    https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
    Simon Fraser University - Project 2
    https://coursys.sfu.ca/2017fa-cmpt-300-d1/pages/Prj2/view
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80 //max length of command

char historyArr[5][MAX_LINE]; //array of length 5 to store most recent commands
int commandCount = 0; //keeps track of the number of commands entered
int historyCount = 0;

void history() {
    if (historyCount == 0) {
        printf("No commands in history.\n");
        return;
    }
    for (int i = 0; i < historyCount; i++) { //starting at most recent command
        printf("%d %s\n", commandCount - i, historyArr[i]); //printing each command with its index
    }
}

/////////////////////////// parts 1 and 2
int main(void)
{ 
    char *args[MAX_LINE/2 + 1];
    char input[MAX_LINE]; //array to hold inputs
    int should_run = 1; //flag to determine when to exit program
    pid_t pid;
    int ampersand = 0;

    while (should_run == 1) {
        printf("osh>");
        fflush(stdout);
        fgets(input, MAX_LINE, stdin); //read user input
        input[strcspn(input, "\n")] = '\0'; //remove new line
        
        if (strcmp(input, "history") == 0){ //if the user types "history", call history
            history();
            if (historyCount < 5) //if history buffer isnt full, increment
                historyCount++;
            for (int i = historyCount - 1; i > 0; i--) //iterate through buffer in reverse order (starting with most recent)
                strcpy(historyArr[i], historyArr[i-1]);
            strcpy(historyArr[0], "history");
            commandCount++;
            continue;
        }
        
        if (strcmp(input, "!!") == 0){ //if the user inputs "!!"
            if (historyCount > 0) //if there is a history, take most recent
                strcpy(input, historyArr[0]);
            else{
                printf("No commands in history.\n"); //if there isnt a history, print message
                continue;
            }
        }
        
        if (historyCount < 5) //if history buffer isnt full, increment
            historyCount++;
        
        for (int i = historyCount - 1; i > 0; i--) //iterate through buffer in reverse order (starting with most recent)
            strcpy(historyArr[i], historyArr[i-1]);

        strcpy(historyArr[0], input);
        
        int num = 0;

        //parse input and divide into arguments (command + argument)
        //separate based on the space 
        char *token = strtok(input, " "); // point to beginning of argument string, store as variable token
        while (token != NULL){
            args[num] = token;
            printf("%s\n", token);
            token = strtok(NULL, " "); //continues break string into tokens
            num++;
        }
        args[num] = NULL; //the last argument is set as NULL
        if (strcmp(args[num - 1],"&")==0) {     //run concurrently
            args[num - 1] = NULL;
            ampersand = 1;
        }

        if (strcmp(args[0], "exit") == 0) { //if the user types exit, program ends
            should_run = 0;
            continue;
        }
        pid = fork();
        if (pid < 0) //if the fork is unsuccessful, exit
            exit(1);
        else if (pid == 0){ //if the fork is successful
            printf("Fork was successful. Pid = %d. Executing...\n", pid);
            if (execvp(args[0], args) == -1) { //call execvp function
                perror("execvp error");
                exit(1);
            }
        } else {
            //if the last character is an ampersand, the parent continues while child executes in background
            if (ampersand == 0)  //check the last character
                wait(NULL);
        }
        commandCount++;
        ampersand = 0;
    }

    return 0;
}   
