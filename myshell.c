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
// Flags
static char isPipeUsed = 0;
static char isAmpersandUsed = 0;

// If we do not use it as array at first it gives core dumped error!
static char command[COMMAND_MAX_SIZE];
static char spaceDelimeter[2] = " ";
static const char pipeDelimeter[2] = "|";
static const char ampersandDelimeter[2] = "&";
static char *token;
static const char *builtinArr[4] = {"cd", "dir", "history", "bye"};
static char *historyArr[HISTORY_MAX_SIZE];
static unsigned historyCount = 0;
static char buffer[COMMAND_MAX_SIZE+1];
static char message[COMMAND_MAX_SIZE+1]; 
// We use '\0' here to achieve null terminated elements. 
// NULL is used for to point pointers to nothing
// 0 is used for integers 
// Source: https://stackoverflow.com/questions/35331819/c-when-should-char-be-null-terminated
static char *argv[100] = { "\0" };
static char *argv2[100] = { '\0' };


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

char** sliceFunction(char * command, char* delimeter, char** argvArr){
    static char *tempToken;

    tempToken = strtok(command, delimeter);
    int tokenCounter = 0;
    while( tempToken != NULL ) {
        // Trim the whitespace from token to make valid comparison
        trimwhitespace(tempToken);
        // Print tokens
        printf("%s:token%d\n",tempToken, tokenCounter);
        // Record argument to the argv array.
        argvArr[tokenCounter] = strdup(tempToken);
        // Continue to split command
        tempToken = strtok(NULL, spaceDelimeter);
        tokenCounter++;
    }
    return argvArr;
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
        // Check if the pipe is used or not
        char *secondCmdPtr = strchr(command,*pipeDelimeter);
        char *ampersandPtr = strchr(command,*ampersandDelimeter);

        // There is not any pipe or ampersand
        if(secondCmdPtr == NULL && ampersandPtr == NULL){
            sliceFunction(command,spaceDelimeter,argv);
        }
        else if(secondCmdPtr != NULL && ampersandPtr == NULL){ // Only pipe used
            isPipeUsed = 1;
            *secondCmdPtr = '\0';
            char * secondCommand = secondCmdPtr + 1;
            // Split first part of the command
            sliceFunction(command,spaceDelimeter,argv);
            // Split second part of the command
            sliceFunction(secondCommand,spaceDelimeter,argv2);
        }
        else if(secondCmdPtr == NULL && ampersandPtr != NULL){// Only ampersand used
            isAmpersandUsed = 1;
            *ampersandPtr = '\0';
            sliceFunction(command,spaceDelimeter,argv);
        }
        else{ // If pipe and ampersand both used.
            isPipeUsed = 1;
            isAmpersandUsed = 1;
            *secondCmdPtr = '\0';
            *ampersandPtr = '\0';
            char * secondCommand = secondCmdPtr + 1;
            // Split first part of the command
            sliceFunction(command,spaceDelimeter,argv);
            // Split second part of the command
            sliceFunction(secondCommand,spaceDelimeter,argv2);
        }
    // Parse is finished

    // If the command is built in
        // If the command is cd
        if(strcmp(argv[0],builtinArr[0]) == 0){
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
        else if(strcmp(argv[0],builtinArr[1]) == 0){
            // Printing initial working directory.
            printf("Current Directory: %s\n", getcwd(directory,100));
        }
        // If the command is history
        else if(strcmp(argv[0],builtinArr[2]) == 0){
            // Prints the history array in reverse order.
            printHistoryArrayInReverseOrder(historyArr,historyCount);
        }
        // If the command is bye
        else if(strcmp(argv[0],builtinArr[3]) == 0){
            // Terminates the program 
            exit(0);
        }
    
    // Not built in part (else) is started.
        else{
            if(!isPipeUsed && !isAmpersandUsed){ // Pipe and ampersand are not used
                execvp(argv[0], argv);
            }
            else if(!isPipeUsed && isAmpersandUsed){ // Only ampersand is used!
                pid_t pid;
                int status;
                pid = fork();
                // Pid could not created error occured. Exit with -1
                if (pid < 0)
                {
                    perror("A fork error has occurred.\n");
                    exit(-1);
                }
                /* We are in the child. */    
                if (pid == 0) {         
                    // The first argument is the file you wish to execute, 
                    // and the second argument is an array of null-terminated strings that represent the appropriate arguments to the file 
                    printf("I am the child, about to call execvp.\n");
                    execvp(argv[0], argv);
                    printf("Error invalid command\n");
                    exit(1);
                }
                /* We are in the parent. */
                while (wait(&status) != pid);
                
            }
            else if(isPipeUsed && !isAmpersandUsed){ // Only pipe is used!
                pid_t pid;

                int fileDescripters[2];

                if(pipe(fileDescripters) == -1) {
                    perror("Pipe failed");
                    exit(1);
                }

                
                if(fork() == 0)            //first fork
                {
                    close(STDOUT_FILENO);  //closing stdout
                    dup(fileDescripters[1]);         //replacing stdout with pipe write 
                    close(fileDescripters[0]);       //closing pipe read
                    close(fileDescripters[1]);
                    execvp(argv[0], argv);
                    perror("Error: Execvp is failed!");
                    exit(1);
                }

                if(fork() == 0)            //creating 2nd child
                {
                    close(STDIN_FILENO);   //closing stdin
                    dup(fileDescripters[0]);         //replacing stdin with pipe read
                    close(fileDescripters[1]);       //closing pipe write
                    close(fileDescripters[0]);
                    execvp(argv2[0], argv2);
                    perror("Error: Execvp2 is failed");
                    exit(1);
                }

                close(fileDescripters[0]);
                close(fileDescripters[1]);
                wait(0);
                wait(0);
            }
            
            else{ // Pipe and Ampersand are used!
                pid_t pid;

                int fileDescripters[2];

                pipe(fileDescripters);

                pid = fork();
                // Pid could not created error occured. Exit with -1
                if (pid < 0)
                {
                    printf("A fork error has occurred.\n");
                    exit(-1);
                }
                // There is not any error.
                else{ 
                    /* We are in the child. */    
                    if (pid == 0) 
                    {

                        dup2(fileDescripters[1],STDOUT_FILENO);  
                        //close read to pipe, in child    
                        close(fileDescripters[0]);               
                        // The first argument is the file you wish to execute, 
                        // and the second argument is an array of null-terminated strings that represent the appropriate arguments to the file 
                        execvp(argv[0], argv);
                        printf("I am the child, about to call ps using system.\n");
                        
                        /* Notice that we do continue in the child after the call to system(). */
                        printf("I am the child, about to exit.\n");
                        exit(127);
                    }
                    /* We are in the parent. */
                    else  
                    {
                        //Replace stdin with the read end of the pipe
                        dup2(fileDescripters[0],STDIN_FILENO);  
                        //close write to pipe, in parent
                        close(fileDescripters[1]);    
                        // The first argument is the file you wish to execute, 
                        // and the second argument is an array of null-terminated strings that represent the appropriate arguments to the file            
                        execvp(argv2[0],argv2);
                        wait(0);
                    }
                }
            }
        }

    // Not built-in part (else) is finished 
	}
	return 0;
}