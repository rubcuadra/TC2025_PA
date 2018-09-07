//
//  main.c
//  Exam_1
//
//  Created by Ruben Cuadra on 06/09/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "railFenceCypher.h"

#define FILEP_PATH_SIZE 100
#define BUFFER_SIZE 20
#define PROC_EXIT 63
#define PROC_SUCC 66
#define PROC_FAIL 67
#define WAIT_SIG 0
#define START_SIG -1
#define FINISH_SIG -2
#define ENCRYPT_FLAG 316
#define DECRYPT_FLAG 318

///// FUNCTION DECLARATIONS /////
void createProcess();
void userMenu(FILE * fp_out, FILE * fp_in);
void attendRequests(FILE * fp_out, FILE * fp_in);
void openPipe(int pipe_channel[]);
void preparePipes(int pipe_out[], int pipe_in[], FILE ** fp_out, FILE ** fp_in);
void closePipes(int pipe_out[], int pipe_in[], FILE ** fp_out, FILE ** fp_in);
unsigned long int factorial(int number);

int main(int argc, const char * argv[])
{
    createProcess();
    return 0;
}

///// FUNCTION DEFINITIONS /////

// Function to create the child process and declare the pipe variables
void createProcess()
{
    pid_t pid;
    // Variables for the pipes, indicating the direction of communication
    int pipe_parent_child[2];
    int pipe_child_parent[2];
    // Variables for the file pointers
    FILE * fp_in = NULL;
    FILE * fp_out = NULL;
    
    // Open the two channels
    openPipe(pipe_parent_child);
    openPipe(pipe_child_parent);
    
    // Create only one fork, the child will do the calculations
    pid = fork();
    
    
    if (pid > 0) // Parent process
    {
        preparePipes(pipe_parent_child, pipe_child_parent, &fp_out, &fp_in); // Open the corresponding pipes
        userMenu(fp_out, fp_in); // Start the loop to hear user requests and send them to child
        closePipes(pipe_parent_child, pipe_child_parent, &fp_out, &fp_in); // Close the pipes
        printf("Parent finishing\n");
    }
    else if (pid == 0) // Child process
    {
        
        preparePipes(pipe_child_parent, pipe_parent_child, &fp_out, &fp_in); // Open the corresponding pipes
        
        attendRequests(fp_out, fp_in); // Start the loop to hear user requests and get the factorial
        
        closePipes(pipe_child_parent, pipe_parent_child, &fp_out, &fp_in); // Close the pipes
        printf("Child finishing\n");
    }
    else // No process created
    {
        perror("Unable to create a new process\n");
        exit(EXIT_FAILURE);
    }
}

// Main loop for the parent process, listens for the user requests
// Receive: the file pointers to write and read to the child
void userMenu(FILE * fp_out, FILE * fp_in)
{
    int action = ENCRYPT_FLAG;
    int rails = 3;
    char buffer[BUFFER_SIZE];
    char inputFilePath[FILEP_PATH_SIZE];
    char outFilePath[FILEP_PATH_SIZE+20];
    char step = '_';
    int proc_result = 0;
    
    while(step!='q'){
        
        switch (step) {
            case '_':
                printf("Welcome! Press the button:\n\t'e' - Encrypt file\n\t'd' - Decrypt file\n\t'q' - Exit\nYour choice: ");
                scanf("%s", buffer);
                step = buffer[0];
                break;
            case ')':
                printf("Introduce the path to the input file\n");
                scanf("%100s", inputFilePath);
                printf("Introduce number of rails\n");
                scanf("%d", &rails);
                //Validate that inputs are okay?
                step = '$';   //Run it
                break;
            case '$':
                fprintf(fp_out, "%d\n",START_SIG); //send an start signal
                fprintf(fp_out, "%d\n",action); //ENCR or DECR
                fprintf(fp_out, "%100s\n",inputFilePath); //send inputFilePath
                fprintf(fp_out, "%d\n",rails); //send num rails
                fflush(fp_out); //do we need it?
                
                fscanf(fp_in, "%120s", outFilePath);
                printf("CHILDREN SENT: %120s\n",outFilePath);
                step = '/'; //Go to clean
                break;
            case 'e':
                action = ENCRYPT_FLAG;
                step   = ')'; //Ask for file
                break;
            case 'd':
                action = DECRYPT_FLAG;
                step   = ')'; //Ask for file
                break;
            case 'q':
//                fprintf(fp_out, "%d\n",FINISH_SIG); //send an exit signal
                fflush(fp_out);
                break;
            case '/':
                memset(inputFilePath, 0, FILEP_PATH_SIZE);
                memset(outFilePath, 0, FILEP_PATH_SIZE+20);
                memset(buffer, 0, BUFFER_SIZE);
                action = ENCRYPT_FLAG;
                step = '_'; //Clean and return to start
                break;
            default: //Error, reset everything and try again
                printf("Unkown command, try again! :)\n");
                step = '/'; //Clean everything
                break;
        }
    }
}

// Loop for the child process to wait for requests from the parent
// Receive: the file pointers to write and read to the parent
void attendRequests(FILE * fp_out, FILE * fp_in)
{
    int signal = WAIT_SIG;
    int rails = 3;
    char inputFilePath[FILEP_PATH_SIZE];
    int action = ENCRYPT_FLAG;
    
    while (1)
    {
        // Listen for requests from the parent
        fscanf(fp_in, "%d",&signal); //Know when to start
        if (signal == FINISH_SIG) {
            printf("PROC OUT\n");
            break;
        }
        else if (signal == START_SIG){
            fscanf(fp_in, "%d",&action);    //Obtain ENCR or DECR
            fscanf(fp_in, "%100s",inputFilePath); //send inputFilePath
            fscanf(fp_in, "%d",&rails); //send num rails
            
            
            int s = (int) strlen( inputFilePath );
            int lastDivI = -1;
            for (int i = s; i > 0 ; i--) { if (inputFilePath[i] == '/'){ lastDivI = i; break; } } //Find last /
            
            //Create outputfile path string
            int missingToCopy = lastDivI!=-1?s-lastDivI:0;                                        //It is 0 if there are no / in the path
            char outputFilePath[s+10]; //for extra + "\0"
            strncpy(outputFilePath, inputFilePath, s);
            const char * extra = action==ENCRYPT_FLAG?"encoded_":"decoded_";
            int ix = 0, ox=0;
            for (int i = lastDivI+1; i < s+10; i++) {
                if (ix < 8) outputFilePath[i] = extra[ix++];
                else outputFilePath[i] = inputFilePath[lastDivI+1+ox++];
            }
            
            MFile * cFile = getFile(inputFilePath);
            if (action == ENCRYPT_FLAG) {
                cipherFile(cFile, rails);
            }else{
                decipherFile(cFile, rails);
            }
            saveFile(outputFilePath,cFile);
            freeMFile(cFile);
            
            fprintf(fp_out, "%s\n", outputFilePath);
            fflush(fp_out);
        }
        else{
            fflush(fp_out);
        }
    }
}

// Verify the opening of a commuication pipe
void openPipe(int pipe_channel[])
{
    if (pipe(pipe_channel) == -1)
    {
        perror("Unable to create pipe\n");
        exit(EXIT_FAILURE);
    }
}

// Close the pipe directions that are not necessary, and open file pointers
// Receive the file descriptors and pointers to the file pointers
void preparePipes(int pipe_out[], int pipe_in[], FILE ** fp_out, FILE ** fp_in)
{
    // Close the unwanted channels
    close(pipe_in[1]);
    close(pipe_out[0]);
    
    // Open the file pointers
    *fp_out = fdopen(pipe_out[1], "w");
    if (*fp_out == NULL)
    {
        perror("Unable to open pipe for writing\n");
        exit(EXIT_FAILURE);
    }
    *fp_in = fdopen(pipe_in[0], "r");
    if (*fp_in == NULL)
    {
        perror("Unable to open pipe for reading\n");
        exit(EXIT_FAILURE);
    }
}

// Close the remaining pipes and file pointers
// Receive the file descriptors and pointers to the file pointers
void closePipes(int pipe_out[], int pipe_in[], FILE ** fp_out, FILE ** fp_in)
{
    // Close the file pointers
    fclose(*fp_in);
    fclose(*fp_out);
    // Close the file descriptors
    close(pipe_in[0]);
    close(pipe_out[1]);
}
