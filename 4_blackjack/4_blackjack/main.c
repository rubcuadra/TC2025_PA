//
//  server.c
//  4_blackjack
//
//  Created by Ruben Cuadra on 21/09/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include "Blackjack.h"
#define BUFFER_SIZE 1024
#define MAX_QUEUE 5

///// FUNCTION DECLARATIONS
void usage(char * program);
void printIPs(char * running_port);
int startServer(char * port);
void fatalError(const char * message);
void waitForConnections(int server_fd);
void StartGame(int client_fd);
void startDeck(int deckSize, int deck[deckSize]);
void printCards(int deckSize, int deck[deckSize]);
void hitCard(int deckSize, int deck[deckSize]);
int checkLose(int deckSize, int deck[deckSize]);
int totalScore(int deckSize, int deck[deckSize]);
void attendBetPrice(int client_fd);

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    srand(time(0)); //Seed
    int server_fd;
    
    printf("\n=== Blackjack Server ===\n");
    
    
    if (argc != 2) usage(argv[0]); // Check the correct arguments
    printIPs(argv[1]);                    // Easier for others to connect
    
    //START
    server_fd = startServer(argv[1]); 
    waitForConnections(server_fd);
    close(server_fd);
    
    return 0;
}


// Show the user how to run this program
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {port_number}\n", program);
    exit(EXIT_FAILURE);
}

/*
 Available interfaces for connecting to the server, based on
 https://stackoverflow.com/questions/20800319/how-do-i-get-my-ip-address-in-c-on-linux
 */
void printIPs(char * running_port)
{
    struct ifaddrs * addrs;
    struct ifaddrs * tmp;
    
    // Get the list of IP addresses used by this machine
    getifaddrs(&addrs);
    tmp = addrs;
    
    printf("Server available @\n");
    
    while (tmp)
    {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
        {
            // Get the address 
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            // Print the ip address of the local machine
            printf("\t%s \t%s:%s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr), running_port);
        }
        tmp = tmp->ifa_next; //Get next interface
    }
    
    freeifaddrs(addrs);
}

// Initialize the server to be ready for connections
// returns file descriptor for the socket
int startServer(char * port)
{
    struct addrinfo hints;
    struct addrinfo * server_info = NULL;
    int server_fd;
    int reuse = 1;
    
    // Prepare the information to determine the local address
    // Clear the structure
    bzero (&hints, sizeof hints);
    // IPv4
    hints.ai_family = AF_INET;
    // Type of socket
    hints.ai_socktype = SOCK_STREAM;
    // Set to look for the address automatically
    hints.ai_flags = AI_PASSIVE;
    
    // GETADDRINFO
    // Use the presets to get the actual information for the socket
    if (getaddrinfo(NULL, port, &hints, &server_info) == -1) {
        fatalError("getaddrinfo");
    }
    
    // SOCKET
    // Open the socket using the information obtained
    server_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (server_fd == -1) {
        close(server_fd);
        fatalError("socket");
    }
    
    // SETSOCKOPT
    // Allow reuse of the same port even when the server does not close correctly
    if (setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
        fatalError("setsockopt");
    }
    
    // BIND
    // Connect the port with the desired port
    if (bind(server_fd, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        fatalError("bind");
    }
    
    // LISTEN
    // Start listening for incomming connections
    if (listen(server_fd, MAX_QUEUE) == -1) {
        fatalError("listen");
    }
    
    // FREEADDRINFO
    // Free the memory used for the address info
    freeaddrinfo(server_info);
    
    printf("Server UP\n");
    
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
        client_fd = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);

        if (client_fd == -1) {
            //If client discconects it falls here
            fatalError("accept");
        }
        
        // Create a child to deal with the new connection
        new_pid = fork();
        if (new_pid > 0) // Parent process
        {
            close(client_fd); //Close so we can continue listening for connections
        }
        else if (new_pid == 0) // Child process
        {
            // Get the data from the client
            inet_ntop (client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
            printf("New challenger approching from %s on port %d\n", client_presentation, client_address.sin_port);
            
            //GAME LOGIC AND PROTOCOLS
            attendBetPrice(client_fd); //Ask for the bet
            
            // Finish the child process
            close(client_fd);
            exit(EXIT_SUCCESS);
        }
        // Error
        else
        {
            fatalError("Unable to fork");
        }
    }
}



/*
 Hear the request from the client and send an answer
 */
void attendBetPrice(int client_fd) {
    char buffer[BUFFER_SIZE];
    int chars_read;
    int totalBetPrice;
    int dealerDeckSize = 2;
    int dealerDeck[dealerDeckSize];
    int dealerScore;
    int playerScore;
    int clientMove;
    int newCard;
    
    bzero(&buffer, BUFFER_SIZE);
    
    // RECV
    // Read the request from the client and parse BET
    chars_read = recv(client_fd, buffer, sizeof buffer, 0);
    if (chars_read == 0) {
        printf("Client disconnected\n");
        return;
    }
    if (chars_read == 0) {
        printf("Client recieve error\n");
        return;
    }
    sscanf(buffer, "%d", &totalBetPrice); //Client must send a number
    printf(" - Client bets:%d\n", totalBetPrice);
    bzero(&buffer, BUFFER_SIZE);
    // Recieved the bet
    //Case more players? it does not matters, only that one or the dealer wins and we are not counting cards
    // START the game
    int answer = 1;
    sprintf(buffer, "%d\n", answer);
    if (send(client_fd, buffer, strlen(buffer) + 1, 0) == -1) {
        printf("Could not send reply");
    }
    bzero(&buffer, BUFFER_SIZE);
    
    Hand * clientHand = getRandomHand(2);
    Hand * serverHand = getRandomHand(2);
    
    //Send the cards
    sprintf(buffer, "%d %d,%d %d\n", serverHand->cards[0],serverHand->cards[1],clientHand->cards[0],clientHand->cards[1]);
    if (send(client_fd, buffer, strlen(buffer) + 1, 0) == -1) {
        printf("Could not send reply");
    }
    bzero(&buffer, BUFFER_SIZE);
    
    do{
        //Wait for the client movement
        chars_read = recv(client_fd, buffer, sizeof buffer, 0);
        if (chars_read == 0) {
            printf("Client disconnected\n");
            return;
        }
        if (chars_read == 0) {
            printf("Client recieve error\n");
            return;
        }
        sscanf(buffer, "%d", &clientMove); //Client must send a number
        bzero(&buffer, BUFFER_SIZE);
        if (clientMove == 1) { //HIT
            hitHand(clientHand);
            //SEND NEW CARD
            sprintf(buffer, "%d\n", clientHand->cards[clientHand->size-1] );
            if (send(client_fd, buffer, strlen(buffer) + 1, 0) == -1) printf("Could not send reply");
            bzero(&buffer, BUFFER_SIZE);
        }
    }while(clientMove == 1);
    
    if (clientMove == 0){ //STAND
        if (peekScore(clientHand)) ; //client already lost just stand
        else{
            int threshold = 5; //Que tanto nos la jugamos
            int finish = 0;
            
            //DEALER LOGIC to Take cards
            while (finish == 0)
            {
                if(serverHand->scoreX == 21 || serverHand->score == 21){ finish=1; } //Stand, we have a 21
                else if(serverHand->scoreX > 21 && serverHand->score > 21) {finish=1;}
                else if(serverHand->scoreX > 21) {
                    if ( (21 - serverHand->score) > threshold) {
                        hitHand(serverHand); //HIT
                    }
                    else{ finish = 1; }      //STAY
                }
                else{
                    if ( (21-serverHand->scoreX) > threshold) {
                        hitHand(serverHand);
                    }
                    else{ finish=1;}
                }
            }
        }
    }
    //SEND DEALER HAND
    //PRINT WINNER
}

void fatalError(const char * message)
{
    perror(message);
    exit(EXIT_FAILURE);
}
