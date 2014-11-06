#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdlib.h>

//Define global variables.
#define MAX_LENGTH 80
#define DELIMS " \t\r\n"
int externPro(char* cmd, char* cwd);
int pause();
int showEnviro();
int help(char* cwd);
int clr();


//Start program
int main (int argc, char ** argv) {
	//define buffers for input
	//user input
	char line[MAX_LENGTH];
	//the command the user wanted
	char *cmd;
	//current working directory
	char cwd[1024];
	//the argument needed for two part commands
	char *arg;
	//true false to determine if using a file or not
	char fn = '1';
	//the batch file to be read
	FILE *batch;
	//a duplicate of the original path needed when executing readme
	char cwd_dup[1024];
	//getting the original path for readme
	getcwd(cwd_dup, sizeof(cwd_dup));
	//define pointers
	//Set environment variable "shell"
	if (setenv("SHELL",cwd_dup,1)){
		printf("Failed to set environment!\n");
	}
	//If the argument ocunter is 2 when the program starts
	//open the file
	if (argc == 2)
	{
		//determines if the file can be opened
		//if it can, run the program from the file
		//file path is argv[1]
		batch = fopen(argv[1], "r");
		if (batch == NULL){
			printf("Error opening file!\n");
		}else{
			fn = '0';
		}
	}
		

	//print working directory
	//read command input from console in a loop
   
	//Keep reading input line until "quit" command, or eof of redirect input like "Ctrl+C", or eof of the file
	while(1){
		//get the current directory
	    getcwd(cwd, sizeof(cwd));
	    //if it is from console, fn is 1
	    //otherwise 0 is fro file
	    //if it is form console, display the current directory and gets the line from the user
	    if(fn == '1'){
	    	printf("%s: ", cwd);
	    	fgets(line, MAX_LENGTH, stdin);
	    }else if(fn == '0'){
	    	if(feof(batch) == 0)
	    	{
	    		fgets(line, MAX_LENGTH, batch);
	    	}else{
	    		fn = '1';
	    		fclose(batch);
	    		continue;
	    		//exit(0);
	    	}
	    }
		//Tokenize the input args array
		cmd = strtok(line, DELIMS);
		//Check for supported internal commands
		if (!cmd){
			//Do nothing with no command
		}else if (strcmp(cmd, "cd") == 0) {
			//store the command name and any arguments
			//pass command to OS through calling "system()", or call related functions
			//change environment variables if need it
			//display result if need it
			arg = strtok(0, DELIMS);
			//cd must have an argument to attempt path change
	        if (!arg) {
	        	fprintf(stderr, "cd missing argument.\n");
	        }
	        else {
	        	//If the path change fails, tell the user
	        	if(chdir(arg) == -1){
	        		printf("Directory change failed!\n");
	        	}
	        	
	        }
	    //clears the screen
		}else if(strcmp(cmd, "clr") == 0) {
			clr();
		//look at the specified directory by the user
		}else if(strcmp(cmd, "dir") == 0) {
			pid_t pid;
			pid = fork();
			if(pid == -1)
		        	printf("Error Creating child\n");
			else if ( pid == 0)
			{

				//if no argument after dir is put, it shows the current Directory
				DIR *dp;
				struct dirent *ep;  
				arg = strtok(0, DELIMS);  
				if(arg == NULL){
					dp = opendir (cwd);
				}else{
					dp = opendir (arg);
				}
				//if it cannot open the directory to show, tell the user
				if (dp != NULL)
				{
					//otherwise it reads each object in the directory
					while (ep = readdir (dp))
					{
						puts (ep->d_name);
					}
					//closes the directory read when it is done
				    (void) closedir (dp);
				}else{
				    perror ("Couldn't open the directory\n");
				}
				exit(0);
			}else if ( pid != 0)
			{
		        wait(NULL);
			}
		//displays the enviroment and all its variables
		}else if(strcmp(cmd, "environ") == 0) {
			pid_t pid;
			pid = fork();
			if(pid == -1)
		        	printf("Error Creating child\n");
			else if ( pid == 0)
			{
				//creates an external pointer to the enviroment
				extern char **environ;
				//finds the amount in environment and display it
				char **current;
				for(current = environ; *current; current++)
				{
					//new line for each element in environment
					printf("%s\n",*current);
				}
				exit(0);
			}else if ( pid != 0)
			{
		        wait(NULL);
			}
		//redisplay what the user wrote after echo
		}else if(strcmp(cmd, "echo") == 0) {
			pid_t pid;
			pid = fork();
			if(pid == -1)
		        	printf("Error Creating child\n");
			else if ( pid == 0)
			{
				//take out the command and display the rest as original text
				arg = strtok(0,"");
				printf("%s", arg);
				exit(0);
			}else if ( pid != 0)
			{
		        wait(NULL);
			}
		//show help
		}else if(strcmp(cmd, "help") == 0) {
			help(cwd_dup);
		//pauses the program
		}else if(strcmp(cmd, "pause") == 0) {
			pause();
		//quits the program
		//tells the user the program has terminated and then quits
		}else if(strcmp(cmd, "quit") == 0) {
			printf("Terminated\n");
			exit(0);
		//otherwise it tries to run an external
		}else{
			externPro(cmd, cwd);
		}
	}
}

int externPro(char* cmd, char* cwd){
	//call fork() or exec() to start a new program
	//char pointer array for the execute
	char* arg1[2];
	arg1[0] = cmd;
	arg1[1] = NULL;
	pid_t pid;
	pid = fork();
	if(pid == -1)
        	printf("Error Creating child\n");
	else if ( pid == 0)
	{
		//if execute returns then it could not run the program
		//ask the user if they are in the right directory for it
        execv(cmd,arg1);
        printf("Error: Cannot open program. Is it in this directory?\n\t\"%s\"\n", cwd);
		exit(0);
	}else if ( pid != 0)
	{
		//sleep for 1 second to allow for the external program to start before continuing
		//the child process
		sleep(1);
	}
}

int pause(){
	//just a temp buffer
	char buf[MAX_LENGTH];
	pid_t pid;
	pid = fork();
	if(pid == -1)
        	printf("Error Creating child\n");
	else if ( pid == 0)
	{
		//Tell the user the program has been paused and to press enter to continue
		printf("PAUSED\n");
		printf("Press Enter to continue: ");
		//used a buffer in case the user enters characters before pressing enter
		fgets(buf, MAX_LENGTH, stdin);
		exit(0);
	}else if ( pid != 0)
	{
        wait(NULL);
	}
}

int help(char* cwd){
	//show readMe file
	pid_t pid;
	pid = fork();
	if(pid == -1)
        	printf("Error Creating child\n");
	else if ( pid == 0)
	{
		//finds the more filter and uses that to display the readme
		//this makes it so that the readme file will only display enough to fill
		//the screen and wait for user to move to the next part to display until it
		//reaches the end of the file
		char* arg1[4];
    	arg1[0] = (char*)"/bin/more";
    	arg1[1] = (char*)"-d";
    	arg1[2] = (char*)(strcat(cwd,"/readme"));
    	arg1[3] = NULL;
		//if a value returns, then it did not display the readme
    	if(execv(arg1[0],arg1))
    	{
            	printf("Error: Cannot open readme file\n");
    	}
    	exit(0);
    }else if ( pid != 0)
	{
        	wait(NULL);
	}
	printf("\n");
}
int clr(){
	//creates a child process to clear the screen
	pid_t pid;
	pid = fork();
	if(pid == -1)
        	printf("Error Creating child\n");
	else if ( pid == 0)
	{
		//after the screen is cleared, it quits the child process
		system("clear");
		exit(0);
	}else if ( pid != 0)
	{
		//wait for the screen to be cleared
        wait(NULL);
	}
}