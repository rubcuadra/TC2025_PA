/*
    RUBEN CUADRA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Signals library
#include <errno.h>
#include <signal.h>
// Sockets libraries
#include <netdb.h>
#include <sys/poll.h>
// Posix threads library
#include <pthread.h>

// Custom libraries
#include "codes.h"
#include "sockets.h"
#include "fatal_error.h"

#define NUM_TABLES 5
#define BUFFER_SIZE 1024
#define MAX_QUEUE 5

//Table struct
//EMPTY => No Players       
//FREE  => 1 player missing
typedef enum table_status {EMPTY,FREE,PLAYING,GAME_OVER} table_s;
typedef struct table_struct {
    int tid;
    int p1_connection_fd;
    int p2_connection_fd;
    int status;
    //Mutex to wait until status == free or empty?
} table_t;

// GM Struct
typedef struct game_master_struct {
    int total_wins;         // Store the total number of wins against players
    int total_played;       // Store the total number of played matches against players
    table_t * tables_array; // An array to the tables
} gm_t;

//Struct passed to a thread
typedef struct thread_data_struct{
    int client_fd ;
    gm_t * gm_data;        
} thread_data_t;

///// FUNCTION DECLARATIONS
void usage(char * program);
void setupHandlers();
void initTables(gm_t * gm_data);
void waitForConnections(int server_fd, gm_t * gm_data);
void shutDownGM(gm_t * gm_data);
void * attentionThread(void * arg);
void onCtrlC(int arg);
void playVsPlayer(int client_fd, int difficulty);
int interrupted = 0;

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    if (argc != 2) usage(argv[0]); // Check the correct arguments

    int server_fd;
    gm_t gm_data; //GameMaster data
    printf("\n=== ONITAMA SERVER ===\n");
    // Configure the handler to catch SIGINT
    setupHandlers();
	// Show the IPs assigned to this computer
	printLocalIPs();
    // Initialize the data structures
    initTables(&gm_data);
    // Start the server
    server_fd = initServer(argv[1], MAX_QUEUE);
	// Listen for connections from the clients
    waitForConnections(server_fd, &gm_data); //, &bank_data, &data_locks);
    // Close the socket
    close(server_fd);

    // Clean the memory used
    shutDownGM(&gm_data);

    // Finish the main thread
    //If we do this then we should poll on threads, otherwise they can get stuck if client never leaves
    // pthread_exit(NULL); 

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {port_number}\n", program);
    exit(EXIT_FAILURE);
}

/*
    Modify the signal handlers for specific events
*/
void setupHandlers()
{
    signal(SIGINT, onCtrlC);
}

/*
    Function to initialize all the information necessary
*/
void initTables(gm_t * gm_data)
{
    //Init stats
    gm_data->total_wins = 0;
    gm_data->total_played = 0;
    
    // Allocate the arrays in the structures
    gm_data->tables_array = malloc(NUM_TABLES * sizeof (table_t));
    // set tables data
    for (int i = 0; i < NUM_TABLES; ++i)
    {
        gm_data->tables_array[i].tid = i;
        gm_data->tables_array[i].status = EMPTY;
        gm_data->tables_array[i].p1_connection_fd = -1;
        gm_data->tables_array[i].p2_connection_fd = -1;
    }
}

/*
    Main loop to wait for incomming connections
*/
void waitForConnections(int server_fd, gm_t * gm_data)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    int new_t_result;
    pthread_t new_tid;
    thread_data_t * connection_data = NULL;
    int poll_response;
	int timeout = 500;		// Time in milliseconds (0.5 seconds)

    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    while (1)
    {
		//// POLL
        // Create a structure array to hold the file descriptors to poll
        struct pollfd test_fds[1];
        // Fill in the structure
        test_fds[0].fd = server_fd;
        test_fds[0].events = POLLIN;    // Check for incomming data
        // Check if there is any incomming communication
        poll_response = poll(test_fds, 1, timeout);

		// Error when polling
        if (poll_response == -1)
        {
            // Test if the error was caused by an interruption
            if (errno == EINTR || interrupted == 1) break; //Ctrl C 
            else                fatalError("ERROR: poll");
            
        }
		// Timeout finished without reading anything
        else if (poll_response == 0)
        {
            // printf("No response after %d seconds\n", timeout);
        }
		// There is something ready at the socket
        else
        {
            // Check the type of event detected
            if (test_fds[0].revents & POLLIN)
            {
				// ACCEPT
				// Wait for a client connection
				client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
				if (client_fd == -1)
				{
					fatalError("ERROR: accept");
				}
				 
				// Get the data from the client
				inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
				printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);
				
                // Prepare the structure to send to the thread, must be freed by the thread
                connection_data = (thread_data_t *) malloc( sizeof(thread_data_t) );
                connection_data->client_fd = client_fd;
                connection_data->gm_data   = gm_data;
                // CREATE A THREAD
                new_t_result = pthread_create(&new_tid, NULL, attentionThread , (void*)connection_data);
                if (new_t_result == -1)
                {
                    printf("ERROR CREATING THREAD\n");
                    free(connection_data);
                    continue;
                }
            }
        }
    }

    // Print the total number of transactions performed
    printf("Total played: %d\n", gm_data->total_played);
    printf("Total wins: %d\n",   gm_data->total_wins);
}

/*
    Hear the request from the client and send an answer
*/
void * attentionThread(void * arg)
{
    char buffer[BUFFER_SIZE];
    int chars_read;
    int ans,difficulty,table,t;
    thread_data_t * tdt = (thread_data_t *) arg;
    int finish = 0;
    // Loop to listen for messages from the client
    while(interrupted==0 && finish==0){ 
        
        //Get choice from client
        chars_read = recv(tdt->client_fd, buffer, sizeof buffer, 0);
        if (chars_read == 0) {
            printf("Client disconnected\n");
            free(arg);
            pthread_exit(NULL);
        }
        t = sscanf(buffer, "%d", &ans);       //Read choice
        bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
        switch(ans){
            case PVE: //VsComputer
                sprintf(buffer, "%d", OK);           //Fine to continue
                sendString(tdt->client_fd, buffer);  //Return answer and continue
                //Ask difficulty
                chars_read = recv(tdt->client_fd, buffer, sizeof buffer, 0);
                if (chars_read == 0) {
                    printf("Client disconnected\n");
                    free(arg);
                    pthread_exit(NULL);
                }
                t = sscanf(buffer, "%d", &difficulty);    //Read choice
                bzero(&buffer, BUFFER_SIZE);              //Clean Buffer
                sprintf(buffer, "%d", t==1?OK:ERROR);     //Matched option
                sendString(tdt->client_fd, buffer);       //Return answer and continue
                playVsPlayer(tdt->client_fd, difficulty); //Will call subproc in python
                break;
            case PVP: //VsPlayer
                sprintf(buffer, "%d", OK);           //Fine to continue
                sendString(tdt->client_fd, buffer);  //Return answer and continue
                //Ask Table
                chars_read = recv(tdt->client_fd, buffer, sizeof buffer, 0);
                if (chars_read == 0) {
                    printf("Client disconnected\n");
                    free(arg);
                    pthread_exit(NULL);
                }
                t = sscanf(buffer, "%d", &table);    //Read choice
                bzero(&buffer, BUFFER_SIZE);         //Clean Buffer
                if (table > 0 && table < NUM_TABLES) //Check if it is a valid table
                {
                    
                }
                else{
                    
                }
                
                    //if it is free enter and play
                    //else wait until it gets free
                break;
            default:
                sprintf(buffer, "%d", ERROR);         //Matched option
                sendString(tdt->client_fd, buffer);   //Return answer and continue
                finish = 1;                           //Error BYE
                break;
        }
    }        
    pthread_exit(NULL);
}

void playVsPlayer(int client_fd, int difficulty){

}

/*
    Free all the malloc memory
*/
void shutDownGM(gm_t * gm_data){
    free(gm_data->tables_array);
}

void onCtrlC(int arg){
    interrupted = 1;
    printf("Shutting down...\n");
}
