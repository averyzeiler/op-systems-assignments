/*
    Assignment 1
    Question 
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
char historyArr[5][MAX_LINE]; //array of length 5 to store most recent commands
int commandCount = 0; //keeps track of the number of commands entered
int readInput(char input[], char *args[], int *ampersand);

/////////////////////////// parts 1 and 2
int main(void)
{ 
    char *args[MAX_LINE/2 + 1];
    char input[MAX_LINE]; // array to hold inputs
    int should_run = 1; // flag to determine when to exit program
    pid_t pid;
    int num = 0;

    while (should_run) {
        printf("osh>");
        fgets(input, MAX_LINE, stdin); //read user input
        input[strcspn(input, "\n")] = '\0'; // remove \n (newline character) and replace with null terminator
        fflush(stdout);

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
        //DRAFT CODE//
        // num = readInput(input, &args, &ampersand); //reads input
        /*pid = fork();

        if (pid < 0) //if the fork is unsuccessful, exit
            exit(1);

        else if (pid == 0){ //if the fork is successful 
            if (ampersand == 0) //if there is no &
                wait(NULL); // parent waits while child executes

          if (strcmp(args[0], "exit") == 0) {
            break;
        }
        if (num == 0) {
            // Blank line
            continue;
        }
        }*/
      
    }

    return 0;
}

int readInput(char input[], char *args[], int *ampersand){
    int i = 0;
    args[i] = strtok(input," ");
    // Divides up string at each space (ie separates into "words" or arguments)
    while (args[i] != NULL) {
        printf("%s\n", args[i]);
        i++;
        args[i] = strtok(NULL, " ");  
    }
    args[i] = NULL;
    if (strcmp(args[i - 1], "&") == 0) {
        *ampersand = 1;
    }
    return i;
    /*
    int parsing = read(STDIN_FILENO, input, MAX_LINE); //reading user input 
    for (int i = 0; i < parsing; i++) {
        if (input[i] == '&') //if the input is &, change value
            *ampersand = 1;
    }

    if (strcmp(args[0], "history") == 0){ //if the user intputs the history command
        if (commandCount > 0) //if there is history, call function
            history();
        else
        printf("\n No commands in history."); //if there is no history, display message

    }
    else if (**args == "!"){ //if the first character in the first string is an !
        int secondInput = args[0][1]; //check the second input
        if (secondInput == "!") //if second input is !
            strcpy(input, historyArr[0]); //return most recent command
    
    }*/
}
/////////////////////////// printing history 

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








