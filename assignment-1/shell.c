/*
    Assignment 1
    Question 2
    Avery Zeiler (zeilera, 400305001) and Clara Dawang (dawangc, 400329049)
    Due: February 19th, 2023
*/

/*
    REFERENCES:
    ya ya lets get this bread #slay yass boots
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80 //max length of command

//PART 2: Printing History
char *historyArr[5][MAX_LINE]; //array of length 5 to store most recent commands
int commandCount = 0; //keeps track of the number of commands entered
int historyCount = 0;

void history(){
    int i;
    int start = historyCount - 1; //index of most recent command
    for (i = start; i >= 0; i--){ //starting at most recent command
        printf("%d %s\n", historyCount - i, historyArr[i%5]); //printing each command with its index
    }
}

/////////////////////////// parts 1 and 2
int main(void)
{ 
    char *args[MAX_LINE/2 + 1];
    char input[MAX_LINE]; //array to hold inputs
    int should_run = 1; //flag to determine when to exit program
    pid_t pid;

    while (should_run) {
        printf("osh>");
        fflush(stdout);
        fgets(input, MAX_LINE, stdin); //read user input
        input[strcspn(input, "\n")] = '\0'; //remove new line

        if (strcmp(input, "history") == 0){ //if the user types "history", call history
            history();
            continue;
        }

        if (strcmp(input, "!!") == 0){ //if the user inputs "!!"
            if (historyCount > 0) //if there is a history, take most recent
                strcpy(input, historyArr[0]);
            else{
                printf("No commands in history\n"); //if there isnt a history, print message
                continue;
            }
        }
        if (strcmp(args[0], "exit") == 0) //if the user types exit, program ends
            exit(0);
        
        if (historyCount < 5) //if history buffer isnt full, increment
            historyCount++;
        
        for (int i = historyCount - 1; i > 0; i--) //iterate through buffer in recerse order (starting with most recent)
            strcpy(historyArr[i], historyArr[i-1]);

        strcpy(historyArr[0], input);
        
        int num = 0;

        //parse input and divide into arguments (command + argument)
        //separate based on the space 

        char *token = strtok(input, " "); // point to beginning of argument string, store as variable token
        while (token != NULL){
            args[num] = token;
            token = strtok(NULL, " "); //continues break string into tokens
            num++;
        }
        args[num] = NULL; //the last argument is set as NULL

            pid = fork();
            if (pid < 0) //if the fork is unsuccessful, exit
                exit(1);
            else if (pid == 0){ //if the fork is successful
                execvp(args[0], args); //call execvp function
                exit(0);
            }
        
            else{
            //if the last character is an ampersand, the parent waits for child to execute
            if (input[strlen(input)-1] != '&')  //check the last character
            wait(NULL);
            }
            commandCount++; 
        }

    return 0;

    }   



/*
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
*/
