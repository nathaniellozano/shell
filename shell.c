#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/wait.h>
#include	<string.h>
#include	<unistd.h>
#include	"minishell.h"


char *lookupPath(char **, char **);
int parseCommand(char *, struct command_t *);
int parsePath(char **);
void printPrompt();
void readCommand(char *);

extern FILE *stdin;

int main() 
{
    char commandLine[LINE_LEN];
    char *pathv[MAX_PATHS];
    int numPaths;
    int i, j, len;
    int chPID;		// Child PID
    int stat;		// Used by parent wait
    pid_t thisChPID;
    struct command_t command;

    // Shell initialization
    for(i=0; i<MAX_ARGS; i++)
        command.argv[i] = (char *) malloc(MAX_ARG_LEN);

    parsePath(pathv);

    // Main loop
    while(TRUE) 
    {
        printPrompt();

        // Read the command line and parse it
        readCommand(commandLine);
        if( (strcmp(commandLine, "exit") == 0) ||
            (strcmp(commandLine, "quit") == 0))
        {
            break;	// Quit the shell
        }

        parseCommand(commandLine, &command);



        // Get the full pathname for the file
        command.name  = lookupPath(command.argv, pathv);



        if(command.name == NULL) 
        {
            fprintf(stderr, "Command %s not found\n", command.argv[0]);
            continue;
        }

        // Create a process to execute the command
        if((chPID = fork()) == 0) 
        {
            //  This is the child, that will execute the command requested



            execv(command.name, command.argv);
       }

       // Wait for the child to terminate



        thisChPID = wait(&stat);
  }

// Shell termination

}


char *lookupPath(char **argv, char **dir) 
{

    int i;
    char *result;
    char pName[MAX_PATH_LEN];

    // Check to see if file name is already an absolute path name
    if(*argv[0] == '/') 
    {
        result = (char *) malloc(strlen(argv[0])+1);
        strcpy(result, argv[0]);
        return result;
    }

    // Look in PATH directories
    // This code does not handle the case of a relative pathname
    for(i = 0; i < MAX_PATHS; i++) 
    {
        if(dir[i] == NULL) break;
        strcpy(pName, dir[i]);
        strcat(pName, "/");
        strcat(pName, argv[0]);


        if(access(pName, X_OK | F_OK) != -1) 
        {

            // File found
            result = (char *) malloc(strlen(pName)+1);
            strcpy(result, pName);
            return result;		// Return with success
        }
    }

    // File name not found in any path variable
    fprintf(stderr, "%s: command not found\n", argv[0]);
    return NULL;

}


int parseCommand(char *cLine, struct command_t *cmd) 
{
    // Determine command name and construct the parameter list
    int argc;
    int i, j;
    char **clPtr;

    // Initialization
    clPtr = &cLine;
    argc = 0;

    // Get the command name and parameters
    // This code does not handle multiple WHITESPACE characters
    while((cmd->argv[argc++] = strsep(clPtr, WHITESPACE)) != NULL) ;

    cmd->argv[argc--] = '\0';	// Null terminated list of strings
    cmd->argc = argc;

    return  1;	
}


int parsePath(char *dirs[]) 
{
    int i;
    char *pathEnvVar;
    register char *thePath, *oldp;

    for(i=0; i<MAX_ARGS; i++)
	dirs[i] = NULL;

    pathEnvVar = (char *) getenv("PATH");
    thePath = (char *) malloc(strlen(pathEnvVar) + 1);
    strcpy(thePath, pathEnvVar);

    i = 0;
    oldp = thePath;

    for(;; thePath++) 
    {
	if((*thePath == ':') || (*thePath == '\0')) 
        {
            dirs[i] = oldp;
            i++;

            if(*thePath == '\0') break;

            *thePath = '\0';
            oldp = thePath + 1;
        }
    }



}


void printPrompt() 
{
	char cwd[1024];
	getcwd(cwd,sizeof(cwd));
	printf("%s ", cwd);
}

void readCommand(char *buffer) 
{
   
    fgets(buffer, LINE_LEN, stdin);

    buffer[strlen(buffer)-1] = '\0';  
}

