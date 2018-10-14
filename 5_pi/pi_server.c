/*
    Server program to compute the value of PI
    This program calls the library function 'get_pi'
    It will create child processes to attend requests
    It receives connections using sockets

    Gilberto Echeverria
    gilecheverria@yahoo.com
    21/02/2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
//SIGNALS
#include <signal.h>
#include <poll.h>
#include <errno.h>
// Custom libraries
#include "get_pi.h"
#include "fatal_error.h"
#include <pthread.h>
#include <ifaddrs.h>

#define BUFFER_SIZE 1024
#define MAX_QUEUE 5

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

///// FUNCTION DECLARATIONS
void usage(char * program);
void printLocalIPs();
int initServer(char * port);
void waitForConnections(int server_fd);
void attendRequest(int client_fd);
void * clientInterrupted (void * arg);

void msleep(int mmsDelay){
    #ifdef _WIN32
    Sleep(mmsDelay);
    #else
    usleep(mmsDelay*1000);  
    #endif
}
//Wait for all sub procs to finish, stop listening incoming connections
void mainCtrlcHandler(int arg);
//Send the value to the client
void clientCtrlcHandler(int arg);

int client_interrupted = 0;
int ssd = 0; //Server Shuting Down?
int sfd;
///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    signal(SIGINT, mainCtrlcHandler);

    printf("\n=== SERVER FOR COMPUTING THE VALUE OF pi ===\n");

    // Check the correct arguments
    if (argc != 2)
    {
        usage(argv[0]);
    }

	// Show the IPs assigned to this computer
	printLocalIPs();

    // Start the server
    sfd = initServer(argv[1]);
    // Listen for connections from the clients
    waitForConnections(sfd);
    // Close the socket
    close(sfd);

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
	Show the local IP addresses, to allow testing
	Based on code from:
		https://stackoverflow.com/questions/20800319/how-do-i-get-my-ip-address-in-c-on-linux
*/
void printLocalIPs()
{
	struct ifaddrs * addrs;
	struct ifaddrs * tmp;

    // Get the list of IP addresses used by this machine
	getifaddrs(&addrs);
	tmp = addrs;

    printf("Server IP addresses:\n");

	while (tmp) 
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
		{
            // Get the address structure casting as IPv4
			struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            // Print the ip address of the local machine
			printf("%s: %s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr));
		}
        else if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET6)
		{
            char name[INET6_ADDRSTRLEN];
            // Get the address structure casting as IPv6
			struct sockaddr_in6 *pAddr = (struct sockaddr_in6 *)tmp->ifa_addr;
            inet_ntop(pAddr->sin6_family, pAddr->sin6_addr.s6_addr, name, INET6_ADDRSTRLEN);
            // Print the ip address of the local machine
			printf("%s: %s\n", tmp->ifa_name, name);
		}

        // Advance in the linked list
		tmp = tmp->ifa_next;
	}

	freeifaddrs(addrs);
}

/*
    Prepare and open the listening socket
    Returns the file descriptor for the socket
    Remember to close the socket when finished
*/
int initServer(char * port)
{
    struct addrinfo hints;
    struct addrinfo * server_info = NULL;
    int server_fd;
    int reuse = 1;

    // Prepare the hints structure
    // Clear the structure for the server configuration
    bzero(&hints, sizeof hints);
    // Set to use IPV4
    hints.ai_family = AF_INET;
    // Use stream sockets
    hints.ai_socktype = SOCK_STREAM;
    // Get the local IP address automatically
    hints.ai_flags = AI_PASSIVE;

    // GETADDRINFO
    // Use the presets to get the actual information for the socket
    // The result is stored in 'server_info'
    if (getaddrinfo(NULL, port, &hints, &server_info) == -1)
    {
        fatalError("getaddrinfo");
    }

    // SOCKET
    // Open the socket using the information obtained
    server_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (server_fd == -1) 
    {
        close(server_fd);
        fatalError("socket");
    }

    // SETSOCKOPT
    // Allow reuse of the same port even when the server does not close correctly
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (int)) == -1)
    {
        fatalError("setsockopt");
    }

    // BIND
    // Connect the port with the desired port
    if (bind(server_fd, server_info->ai_addr, server_info->ai_addrlen) == -1)
    {
        fatalError("bind");
    }

    // LISTEN
    // Start listening for incomming connections
    if (listen(server_fd, MAX_QUEUE) == -1)
    {
        fatalError("listen");
    }

    // FREEADDRINFO
    // Free the memory used for the address info
    freeaddrinfo(server_info);

    printf("Server ready\n");

    return server_fd;
}

/*
    Main loop to wait for incomming connections
*/
void waitForConnections(int server_fd)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pid_t new_pid;

    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    while (1)
    {
        // ACCEPT
        // Wait for a client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
        if (client_fd == -1)
        {
            fatalError("accept");
        }
         
        // Get the data from the client
        inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
        printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);

        // FORK
        // Create a new child process to deal with the client
        new_pid = fork();
        // Parent
        if (new_pid > 0)
        {
            // Close the new socket
            close(client_fd);
        }
        else if (new_pid == 0)
        {
            signal(SIGINT, clientCtrlcHandler);
            // Close the main server socket to avoid interfering with the parent
            close(server_fd);
            printf("Child process %d dealing with client\n", getpid());
            // Deal with the client
            attendRequest(client_fd);
            // Close the new socket
            close(client_fd);
            // Terminate the child process
            exit(EXIT_SUCCESS);
        }
        else
        {
            fatalError("fork");
        }
    }
}

/*
    Hear the request from the client and send an answer
*/
void attendRequest(int client_fd)
{
    printf("%d\n",client_fd);
    char buffer[BUFFER_SIZE];
    int chars_read;
    unsigned long int iterations;
    double result = 4.0+0.0;
    // Clear the buffer
    bzero(buffer, BUFFER_SIZE);

    // RECV
    // Receive the request
    chars_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (chars_read == -1)
    {
        fatalError("recv");
    }
    sscanf(buffer, "%lu", &iterations);

    printf(" > Got request from client with iterations=%lu\n", iterations);

    // Create thread that stays in recv and modifies the value of client_interrupted
    pthread_t listener;
    int tRes;
    long int toThread = (long int) client_fd;
    tRes = pthread_create(&listener, NULL, clientInterrupted, (void*)toThread);
    
    // Calculate PI as long as the client is connected
    int sign = -1;
    unsigned long int divisor = 3;
    unsigned long int counter = 0;
    
    //While we haven't finished AND client is still waiting AND we are still up
    while(counter<iterations && client_interrupted == 0 && ssd==0){
        result += sign * (4.0/divisor);
        sign *= -1;
        divisor += 2;
        counter++;
    }

    if(client_interrupted == 0) pthread_cancel(listener); //Finish the recv
    printf(" > Sending PI=%.20lf\n", result);

    // Prepare the response to the client
    sprintf(buffer, "%.20lf", result);
    // SEND
    // Send the response
    if (send(client_fd, buffer, strlen(buffer)+1, 0) == -1)
    {
        fatalError("send");
    }
}

void * clientInterrupted (void * arg)
{
    char buffer[BUFFER_SIZE];
    long client_fd = (long)arg; 
    int chars_read, resp;
    chars_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (chars_read == 0) { 
        printf("Client disconnected\n"); 
        pthread_exit(0);
    }
    if (chars_read == 0) {
        printf("Client recieve error\n");
        pthread_exit(0);
    }
    sscanf(buffer, "%d", &resp); 
    
    if(resp == 316){ //Client sent a 316, meaning they are leaving before we finish
        printf("CLIENT WANTS TO LEAVE BEFORE TIME\n");
        client_interrupted = 1; 
    }

    pthread_exit(0);
}

void mainCtrlcHandler(){
    printf("Shutting Down!\n");
    msleep(500);//wait until all children are DONE sending (0.5 seconds)
    close(sfd); // Close the socket
}

void clientCtrlcHandler(){ //It is called inside the sub proc, the main proc will never run this    
    //Set variable shutdown to 1, it is used by the loop of PI
    ssd = 1;
}


