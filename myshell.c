#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/queue.h>
#include <sys/types.h>




#define COMMAND_MAX_SIZE 100
#define HISTORY_MAX_SIZE 10

static char directory[COMMAND_MAX_SIZE];

// If we do not use it as array at first it gives core dumped error!
static char command[COMMAND_MAX_SIZE];
static const char spaceDelimeter[2] = " ";
static const char pipeDelimeter[2] = "|";
static char *token;
static const char *builtinArr[4] = {"cd", "dir", "history", "bye"};
static char *historyArr[HISTORY_MAX_SIZE];
static unsigned historyCount = 0;
static char *firstCmd;
static char buffer[COMMAND_MAX_SIZE+1];
static char message[COMMAND_MAX_SIZE+1]; 
// We use '\0' here to achieve null terminated elements. 
// NULL is used for to point pointers to nothing
// 0 is used for integers 
// Source: https://stackoverflow.com/questions/35331819/c-when-should-char-be-null-terminated
static char *argv[100] = { "\0" };

// For the history we will use last in first printed system.
// Function prints the elements in the last to first order.
void printHistoryArrayInReverseOrder(char** histArr, unsigned historyCount){
    // Assign i to historyCount and print commands in the newest to oldest order!
    printf("Command History\n");
    for(int i=historyCount-1;i>=0;i--){
        printf("[%d] %s\n", i+1, histArr[i]);
    }
}

// For the history we will use first in first out system.
// Functions takes a command and an array 
// If array does not exceed max size then adds the command to the proper place
// Else it frees the first argument slither other arguments one index below and adds given command to the last. 
void addCommandToHistory(const char *command, char** historyArr)
{
   if (historyCount < HISTORY_MAX_SIZE) {
        historyArr[historyCount++] = strdup( command );
   } else {
        free( historyArr[0] );
        for (unsigned index = 1; index < HISTORY_MAX_SIZE; index++) {
            historyArr[index - 1] = historyArr[index];
        }
        historyArr[HISTORY_MAX_SIZE - 1] = strdup( command );
    }
}

// Function takes a char pointer after removing whitespaces from it
// Returns a substring of the str.
// Source: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // Empty string 
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

int main()
{
    /* 
    INSTRUCTIONS

    print "myshell>" 
    read command line 
    parse command 
    if the command is built-in 
    execute command //in your implementation
    else 
    fork a child 
    if child 
    execute command //calls execvp 
    else if not background process
    wait for the child
    */
	while (1) {
    // Printing shell name.
        printf("myshell> ");
        
    // Read line
        fgets(command, 101, stdin);

        // Trim the whitespace from command
        trimwhitespace(command);

        // Save command to the historyArr before parsing.
        addCommandToHistory(command,historyArr);

    // Parse the command 
        token = strtok(command, spaceDelimeter);
        int tokenCounter = 0;
        /* walk through other tokens */
        while( token != NULL ) {
            // Trim the whitespace from token to make valid comparison
            trimwhitespace(token);
            // Print tokens
            printf("%s:token%d\n",token, tokenCounter);
            // If it is the command 
            if(tokenCounter == 0){
                firstCmd = strdup(token);
                printf("%s:firstCmdValue\n", firstCmd);
            } else{ // If it is an argument
                // Record argument to the argv array.
                argv[tokenCounter] = strdup(token);
            }
            token = strtok(NULL, spaceDelimeter);
            tokenCounter++;
        }
    // Parse is finished

    // If the command is built in
        // If the command is cd
        if(strcmp(firstCmd,builtinArr[0]) == 0){
            // Printing initial working directory
    printf("Initial Directory: %s\n", getcwd(directory, 100));
    if(!argv[1]){ // Argument is not present.
        // Print initial pwd value
        printf("%s | Initial Pwd Value\n", getenv("PWD"));
        
        // Change directory to $HOME Path
        chdir(getenv("HOME"));
        
        // Printing current working directory
        printf("Current Directory: %s\n", getcwd(directory, 100));

        // Update pwd
        setenv("PWD", getenv("HOME"),1);

        // Print updated pwd value
        printf("%s | updated Pwd Value\n",getenv("PWD"));
    }
    else{   // Argument is present.

        // We will check the first argument and change the directory accordingly.
        if(chdir(argv[1]) == -1){ // If argument is not valid
            printf("Error: There is not any directory with given name!\n");
        }
        else{ // Argument is valid
            // Printing current working directory
            printf("Current Directory: %s\n", getcwd(directory, 100));
        }
    }
        }
        // If the command is dir 
        else if(strcmp(firstCmd,builtinArr[1]) == 0){
            // Printing initial working directory.
            printf("Current Directory: %s\n", getcwd(directory,100));
        }
        // If the command is history
        else if(strcmp(firstCmd,builtinArr[2]) == 0){
            // Prints the history array in reverse order.
            printHistoryArrayInReverseOrder(historyArr,historyCount);
        }
        // If the command is bye
        else if(strcmp(firstCmd,builtinArr[3]) == 0){
            // Terminates the program 
            exit(0);
        }

    // Not built in part (else) is started.
        else{
            pid_t pid;

            pid = fork();
            // Pid could not created error occured. Exit with -1
            if (pid < 0)
            {
                printf("A fork error has occurred.\n");
                exit(-1);
            }
            // There is not any error.
            else 
                /* We are in the child. */    
                if (pid == 0) 
                {
                    printf("I am the child, about to call ps using system.\n");
                    // The first argument is the file you wish to execute, 
                    // and the second argument is an array of null-terminated strings that represent the appropriate arguments to the file 
                    execvp(firstCmd,argv);
                    printf("I am the child, about to exit.\n");
                    exit(127);
                }
                /* We are in the parent. */
                else  
                {
                    wait(0);               /* Wait for the child to terminate. */
                    printf("I am the parent.  The child just ended.  I will now exit.\n");
                    exit(0);
                }
        }

    // Not built-in part (else) is finished 
	}
	return 0;
    
}