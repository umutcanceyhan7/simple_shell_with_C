#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/queue.h>
#include <sys/types.h>


static char directory[100];

// If we do not use it as array at first it gives core dumped error!
static char command[100];

static const char pipeDelimeter[2] = " ";
static char *token;
static const char *builtinArr[4] = {"cd", "dir", "history", "bye"};
static char *historyArr[10] = {"deneme", "deneme2", "deneme3"};
static int counter = 0;
static char *firstCmd;
static char *argv[100];


int history(char* histArr[]){
    int length = sizeof(&histArr)/sizeof(&histArr[0]);
    for(int i =0;i<length;i++){
        printf("%s",histArr[i]);
    }

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

        // Save command to the history
        historyArr[counter] = command;
        // Counter is 
        counter = (counter+1) % 10;

    // Parse the command 
        token = strtok(command, pipeDelimeter);
        int tokenCounter = 0;
        /* walk through other tokens */
        while( token != NULL ) {
            printf( " %s\n", token );
            if(tokenCounter == 0){
                printf("first command için printledim\n");
                printf("%s token için\n", token);
                firstCmd = strdup(token);
                printf("%s first command için\n", firstCmd);
            } else{
                argv[tokenCounter] = token;
            }
            token = strtok(NULL, pipeDelimeter);
        }
        printf("%sfirstcmd\n", firstCmd);
        firstCmd = skipwhite(firstCmd);
        //printf("%sbuiltin0\n", builtinArr[0]);
        //printf("%sbuiltin1\n", builtinArr[1]);

    // Parse is finished

    // if the command is built in
        // If the command is cd
        // strcmp(firstCmd,builtinArr[0]) == 0
        if(strcmp(firstCmd,builtinArr[0]) == 0){
            // Printing initial working directory
            printf("Current Directory: %s\n", getcwd(directory, 100));
            
            // Changing directory
            chdir("./umutcan");
            
            // Printing current working directory
            printf("%s\n", getcwd(directory, 100));
        
            // Finish after chdir is executed.
            printf("Builtin1");
            return 0;
        }
        // If the command is dir 
        // strcmp(firstCmd,builtinArr[1]) == 0
        else if(1 != 1){
            // Printing initial working directory.
            printf("Current Directory: %s\n", getcwd(directory,100));
            // Finish after getcwd is executed.
            printf("Builtin2");
            return 0;

        }
        // If the command is history
        // strcmp(firstCmd,builtinArr[2]) == 0
        else if(1 != 1){
            int length = sizeof(historyArr)/sizeof(historyArr[0]);
            for(int i = 0; i<length;i++){
                printf("[%d] %s\n", i+1, historyArr[i]);
            }
            printf("Builtin3");
        }
        // If the command is bye
        else if(strcmp(firstCmd,builtinArr[3]) == 0){
            // Finish the program 
            printf("Builtin4");
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
                    system("ls");
                    /* Notice that we do continue in the child after the call to system(). */
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