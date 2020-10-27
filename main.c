/*
* Description: Project 1 main file. Contains functionality to parse text files or 
* input manually entered by the user on the command line. After parsing is complete, 
* calls are referenced from the command header file 'command.h'
*
* Author: Kylie Quan
*
* Date: 10/20/2019
*/

/*-----------------------------Preprocessor Directives----------------------------*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "command.h"
/*--------------------------------------------------------------------------------*/

/*----------------------------------Declarations----------------------------------*/
int checkCommand(char *token);
int makeCall_File(char *command, char **arr, size_t arrSize);
int parseCommand_File(char **arr, size_t arrSize);
int getline_File(char *filename, char *buffer, size_t bufferSize);
int filemode(char filename[]);
int makeCall_Interactive(char** arr, size_t arrSize);
int splitTokens_Interactive(char** arr);
int intermode();
/*--------------------------------------------------------------------------------*/

/*------------Start of Functions Shared Between File & Interactive Mode-----------*/
int checkCommand(char *token) {
    // If command requires 0 args, return 1
    // If command requires 1 arg, return 2
    // If commmand requires 2 args, return 3
    // If command not valid, return 0

    if (strcmp(token, "ls") == 0 || strstr(token, "ls")) { // 0 args
        return 1;
    }
    else if (strcmp(token, "pwd") == 0 || strstr(token, "pwd")) { // 0 args
        return 1;
    }
    else if (strcmp(token, "mkdir") == 0 || strstr(token, "mkdir")) { // 1 arg
        return 2;
    }
    else if (strcmp(token, "cd") == 0 || strstr(token, "cd")) { // 1 arg
        return 2;
    }
    else if (strcmp(token, "cp") == 0 || strstr(token, "cp")) { // 2 args
        return 3;
    }
    else if (strcmp(token, "mv") == 0 || strstr(token, "mv")) { // 2 args
        return 3;
    }
    else if (strcmp(token, "rm") == 0 || strstr(token, "rm")) { // 1 arg
        return 2;
    }
    else if (strcmp(token, "cat") == 0 || strstr(token, "cat")) { // 1 arg
        return 2;
    }
    else {
        return 0; 
    }
}
/*----------------------------End of Shared Functions-----------------------------*/

/*--------------------------Start of File Mode Functions--------------------------*/
/* Make call based on command retrieved from file */
int makeCall_File(char *command, char **arr, size_t arrSize) {

	char *wait = ">>> ";

    if (strcmp(command, "ls") == 0 || strstr(command, "ls")) {
        write(1, wait, strlen(wait));
    	write(1, command, strlen(command));
    	write(1, "\n", 1);

        listDir();
    }
    else if (strcmp(command, "pwd") == 0 || strstr(command, "pwd")) {
    	write(1, wait, strlen(wait));
    	write(1, command, strlen(command));
    	write(1, "\n", 1);

        showCurrentDir();
    }
    else if (strcmp(command, "mkdir") == 0 || strstr(command, "mkdir")) {
        char *newDir = arr[0];

        write(1, wait, strlen(wait));
        write(1, command, strlen(command));
        write(1, " ", 1);
        write(1, newDir, strlen(newDir));
        write(1, "\n", 1);

        makeDir(newDir);
    }
    else if (strcmp(command, "cd") == 0 || strstr(command, "cd")) {
        char *newDir = arr[0];

    	write(1, wait, strlen(wait));
    	write(1, command, strlen(command));
    	write(1, " ", 1);
    	write(1, newDir, strlen(newDir));
    	write(1, "\n", 1);

        changeDir(newDir);
    }
    else if (strcmp(command, "cp") == 0 || strstr(command, "cp")) {
        char *fileSrc = arr[0];
        char *fileDst = arr[1];

    	write(1, wait, strlen(wait));
    	write(1, command, strlen(command));
    	write(1, " ", 1);
    	write(1, fileSrc, strlen(fileSrc));
    	write(1, " ", 1);
    	write(1, fileDst, strlen(fileDst));
    	write(1, "\n", 1);

        copyFile(fileSrc, fileDst);
    }
    else if (strcmp(command, "mv") == 0 || strstr(command, "mv")) {
        char *fileSrc = arr[0];
        char *fileDst = arr[1];

    	write(1, wait, strlen(wait));
    	write(1, command, strlen(command));
    	write(1, " ", 1);
    	write(1, fileSrc, strlen(fileSrc));
    	write(1, " ", 1);
    	write(1, fileDst, strlen(fileDst));
    	write(1, "\n", 1);

        moveFile(fileSrc, fileDst);
    }
    else if (strcmp(command, "rm") == 0 || strstr(command, "rm")) {
        char *filename = arr[0];

        write(1, wait, strlen(wait));
        write(1, command, strlen(command));
        write(1, " ", 1);
        write(1, filename, strlen(filename));
        write(1, "\n", 1);
 
        deleteFile(filename);
    }
    else if (strcmp(command, "cat") == 0 || strstr(command, "cat")) {
        char *filename = arr[0];

        write(1, wait, strlen(wait));
        write(1, command, strlen(command));
        write(1, " ", 1);
        write(1, filename, strlen(filename));
        write(1, "\n", 1);

        displayFile(filename);
    }
    return 1;
}

/* Split line read from file into individual commands */
int parseCommand_File(char **arr, size_t arrSize) {

    const char *s = " ";

    /* The purpose of args is to store the arguments corrresponding to the 
       command that was given. This will be sent to makeCall_File along with
       the string containing the actual function call (e.g. mkdir). */ 
    char **args;

    for (int i=0; i<arrSize; i++) {

        if (arr[i] == NULL) {
            continue;
        }

        else {
            int ctr = 0;
            char *line = arr[i];
            char *token = strtok(line, s);

            /* Save the first token in the variable command, because it will be
               used to indicate which UNIX system call is being referenced. */
            char *command = token;

            /* checkCommand ensures the command is a valid UNIX system 
               command. */
            int check = checkCommand(command);

            /* Allocate memory for the arguments. */
            size_t argSize = 2;
            args = (char**)malloc(argSize * sizeof(char*));

            /* Initialize values of args to NULL in case there are not enough 
               arguments given to occupy all of the allocated memory. */
            for (int i=0; i<argSize; i++) {
                args[i] = NULL;
            }

            /* Remove white space from the arguments of the call and then assign 
            the pointer 'args' */
            int idx = 0;
            while (token != NULL) {
                token = strtok(NULL, s);
                args[idx] = token;
                ctr++;
                idx++;
            }
            
            /* If ctr == check, then the given command and its arguments make a
            valid function call. Send to makeCall_File to actually provoke relative
            functionality */
            if (ctr == check) {
                makeCall_File(command, args, argSize);
            }

            free(args);
        }
    }
    return 1;
}

/* Read file */
int getline_File(char *filename, char *buffer, size_t bufferSize) {

    int file = open(filename, O_RDONLY);
    if (file == -1) {
    	char *error = "Error: Unable to open file '";
    	write(1, error, strlen(error));
    	write(1, filename, strlen(filename));
    	write(1, "'\n", 2);

        return -1;
    }

    if (read(file, buffer, bufferSize) == -1) {
    	char *error = "Error: Unable to read file '";
    	write(1, error, strlen(error));
    	write(1, filename, strlen(filename));
    	write(1, "'\n", 2);

        return -1;
    }
    close(file);
    return 1;
}

/* File Mode */
int filemode(char filename[]) {
	
	/* Connect open stream to output.txt in file mode only */
	freopen("output.txt", "w", stdout);

    char *buffer = NULL;
    size_t bufferSize = 300;
    ssize_t inputSize = 0;

    /* Allocate memory for the input buffer. */
    buffer = (char *)malloc(bufferSize * sizeof(char));

    /* Read text file given by the user. */
    inputSize = getline_File(filename, buffer, bufferSize);

    /* Place '\0' at the end of the string held in the input buffer 
       to signify the end of the string. */
    if (inputSize > 0) {
   		buffer[bufferSize] = '\0';
    }
    else {
    	buffer[0] = '\0';
    }

    char *token;

    size_t ptrSize = 300;
    char **ptr = (char **)malloc(ptrSize*sizeof(char*));
 
    /* Initialize values of the ptr to NULL */
    for (int i=0; i<ptrSize; i++) {
        ptr[i] = NULL;
    }  

    /* Tokenize the input string with the delimiters ';', newline character '\n'
       and carriage return '\r', and place each token in ptr. This collection of
       tokens represents a single command and its arguments. */
    token = strtok(buffer, ";\n\r");
    ptr[0] = token;
    int i = 1;

    while (token != NULL) {
        token = strtok(NULL, ";\n\r");
        ptr[i] = token;
        i++;
    }   

    /* Send the command and its arguments (held in ptr) to parseCommand_File */
    parseCommand_File(ptr, ptrSize);

    free(ptr);
    free(buffer);
    return 0;
}
/*---------------------------End of File Mode Functions---------------------------*/

/*-----------------------Start of Interactive Mode Functions----------------------*/
/* Make call based on command given by user */
int makeCall_Interactive(char** arr, size_t arrSize) {

	int ctr = 0;
	/* Get total count of valid entries */
	for (int i=0; i<arrSize; i++) {
		if (arr[i] != NULL) {
			ctr++;
		}
	}

	/* checkCommand returns how many strings should be in the line depending 
	   on the command that is called. If this value does not equal ctr, the 
	   call is invalid */
	char *command = arr[0];
	int check = checkCommand(command);

	/* Check whether command is valid */
	if (ctr == 1 && check == 0) {
		char *error = "Error! Unrecognized command: ";
		write(1, error, strlen(error));
		write(1, command, strlen(command));
		write(1, "\n", 1);
	}
	else if (check != ctr) {
		char *error = "Error! Unsupported parameters found for command: ";
		write(1, error, strlen(error));
		write(1, command, strlen(command));
		write(1, "\n", 1);
	}
	else {
		if (strcmp(command, "ls") == 0 || strcmp(command, "ls") == 13) {
			listDir();
		}
		else if (strcmp(command, "pwd") == 0 || strcmp(command, "pwd") == 13) {
			showCurrentDir();
		}
		else if (strcmp(command, "mkdir") == 0 || strcmp(command, "mkdir") == 13) {
			char *newDir = arr[1];
			makeDir(newDir);
		}
		else if (strcmp(command, "cd") == 0 || strcmp(command, "cd") == 13) {
			char *newDir = arr[1];
			changeDir(newDir);
		}
		else if (strcmp(command, "cp") == 0 || strcmp(command, "cp") == 13) {
			char *fileSrc = arr[1];
			char *fileDst = arr[2];
			copyFile(fileSrc, fileDst);
		}
		else if (strcmp(command, "mv") == 0 || strcmp(command, "mv") == 13) {
			char *fpSrc = arr[1];
			char *fpDst = arr[2];
			moveFile(fpSrc, fpDst);
		}
		else if (strcmp(command, "rm") == 0 || strcmp(command, "rm") == 13) {
			char *filename = arr[1];
			deleteFile(filename);
		}
		else if (strcmp(command, "cat") == 0 || strcmp(command, "cat") == 13) {
			char *filename = arr[1];
			displayFile(filename);
		}
	}
	return 1;
}

/* Split line read from file into individual commands */
int splitTokens_Interactive(char** arr) { 

	int i = 0;
	char** ptr = NULL;
	size_t ptrSize = 10;

    /* Check if the user entered multiple commands on one line separated by a
       a semicolon. If so, split the commands based on the location of the 
       delimiter ';'. Each command with its corresponding arguments is stored 
       in ptr to be sent to makeCall_Interactive. */
	while (i < sizeof(arr)) {
		int ctr = 0;
		ptr = (char**)malloc(ptrSize * sizeof(char*));

		while ((strcmp(arr[i], ";") != 0) && (strcmp(arr[i], "NULL") != 0)) {
			ptr[ctr] = arr[i];
			ctr++;
			i++;
		}

		makeCall_Interactive(ptr, ptrSize);

		if (strcmp(arr[i], "NULL") == 0) {
			break;
		}

		i++;
		free(ptr);
	}
	free(ptr);
	return 1;
}

/* Interactive Mode */
int intermode() {

	char *buffer = NULL;
	size_t bufsize = 100;

	const char *s = " \n";
	char *token = NULL;

	char *exitStr = "exit";

	/* Allocate memory for the input buffer. */
	buffer = (char *)malloc(bufsize * sizeof(char));

	/* Check if buffer contains (null) value before proceeding */
	if (buffer == NULL) {
		char *error = "Error! Unable to allocate input buffer.\n";
		write(1, error, strlen(error));
		exit(1);
	}
	
	char **ptr = NULL;
    /* The while loop should run as long as the user does not type 'exit'
       on the command line */
	while(1) {

		int ctr = 0;
		ptr = (char**)malloc(20 * sizeof(char*));

		/* Print >>> then get the input string */
		char *wait = ">>> ";
		write(1, wait, strlen(wait));
		getline(&buffer, &bufsize, stdin);

		/* Tokenize the input string */
		token = strtok(buffer, s);
		ptr[0] = token;
		ctr = 1;

		while (token != NULL) {
			if (strcmp(exitStr, buffer) == 0) {
				break;
			}

           	token = strtok(NULL, s);
          	ptr[ctr++] = token;
		}

		ptr[ctr-1] = "NULL";

		/* Check if user entered anything on the command line at all
           by ensuring that the first value in ptr is not a null value.
           If user input exists, send command to splitTokens_Interactive
           for further parsing. */
		if (strcmp(ptr[0], "NULL") != 0) {
        	splitTokens_Interactive(ptr);
        }

        /* Check if user wants to exit program */
		if (token != NULL) {
			if (strcmp(exitStr, buffer) == 0) {
				break;
			}
		}

		free(ptr);
	}
	free(ptr);
	free(buffer);
	return 1;
}
/*------------------------End of Interactive Mode Functions-----------------------*/

/*----------------------------------Program Main----------------------------------*/
int main(int argc, char *argv[]) {

	char check2[] = "-command";
	char check3[] = "-f";

	char *command = NULL;
	char *file = NULL;

	/* Check if first command called with executable is valid */
	if (argc == 2) {
		if (strcmp(argv[1], check2) == 0) {
			command = argv[1];
			intermode();
		}
		else {
			char *error = "Error! Unrecognized command: ";
			write(1, error, strlen(error));
			write(1, argv[1], strlen(argv[1]));
			write(1, "\n", 1);
			exit(0);
		}
	}
	else if (argc == 3) {
		if (strcmp(argv[1], check3) == 0) {
			command = argv[1];
			file = argv[2];
			filemode(file);
		}
		else{
			char *error = "Error! Unrecognized command: ";
			write(1, error, strlen(error));
			write(1, argv[1], strlen(argv[1]));
			write(1, " ", 1);
			write(1, argv[2], strlen(argv[2]));
			write(1, "\n", 1);
			exit(0);
		}
	}
	else {
		char *error = "Error! Unrecognized command.\n";
		write(1, error, strlen(error));
		exit(0);
	}
	return 1;
}
/*----------------------------------Program End-----------------------------------*/
