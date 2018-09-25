//
//  client.c
//  4_blackjack
//
//  Created by Ruben Cuadra on 21/09/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include "Blackjack.h"
#define BUFFER_SIZE 1024

///// FUNCTION DECLARATIONS
int readIntInRange(int x,int y);
void usage(char * program);
int openSocket(char * address, char * port);
void gameStart(int connection_fd);
void startDeck(int deckSize, int deck[deckSize]);
void printCards(int deckSize, int deck[deckSize]);
void hitCard(int deckSize, int deck[deckSize]);
int checkLose(int deckSize, int deck[deckSize]);
int getDeckValue(int deckSize, int deck[deckSize]);
void fatalError(const char * message);
void showOptions();
void INThandler();

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int connection_fd;

    printf("\n=== Blackjack Client ===\n");
    if (argc != 3) usage(argv[0]); // Check the correct arguments
    signal(SIGINT, INThandler);
    //START
    connection_fd = openSocket(argv[1], argv[2]);
    gameStart(connection_fd);
    close(connection_fd);

    return 0;
}

void INThandler(int sig){
    printf("Ctrl C\n");
}

///// FUNCTION DEFINITIONS

// Show the user how to run this program
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {server_address} {port_number}\n", program);
    exit(EXIT_FAILURE);
}

//Open the socket to the server
int openSocket(char * address, char * port)
{
    struct addrinfo hints;
    struct addrinfo * server_info = NULL;
    int connection_fd;

    // Prepare the hints structure
    bzero (&hints, sizeof hints);
    // Set to use IPv4
    hints.ai_family = AF_INET;
    // Set type of socket
    hints.ai_socktype = SOCK_STREAM;
    // Set to look for the address automatically
    hints.ai_flags = AI_PASSIVE;

    // GETADDRINFO
    // Use the presets to get the actual information for the socket
    if (getaddrinfo(address, port, &hints, &server_info) == -1) {
        fatalError("getaddrinfo");
    }

    // SOCKET
    // Open the socket using the information obtained
    connection_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (connection_fd == -1) {
        close(connection_fd);
        fatalError("socket");
    }

    // CONNECT
    // Connect to the server
    if (connect(connection_fd, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        fatalError("connect");
    }

    // FREEADDRINFO
    // Free the memory used for the address info
    freeaddrinfo(server_info);

    return connection_fd;
}

//The player will decide what move the server is making or if it's the player (dealer vs player)
void gameStart(int connection_fd) 
{
    char buffer[BUFFER_SIZE];
    int chars_read;
    int playerDeckSize = 2;
    int playerDeck[playerDeckSize]; //Define our variables (decks for 2 players and their scores)
    int betPrice; //Bettings from the player
    int totalBetPrice;
    int startGameFlag;
    int f,s,f2,s2;
    int playerChoice;
    int playerScore; //Player Score
    int dealerScore; //Dealer's score
    int blackjack = 0; //False for now
    
    srand(time(0)); //Use the current time as seed generator

    printf("Place your bet (Number between 1 and 100):\n"); //We ask the player for the bet they'd like to do
    betPrice = readIntInRange(1,100);
    printf("%d\n",betPrice);
    // SEND to server the bet
    sprintf(buffer, "%d\n", betPrice);
    if (send(connection_fd, buffer, strlen(buffer) + 1, 0) == -1 )fatalError("send");
    bzero(&buffer, BUFFER_SIZE);

    // Step 2: Wait for the server to start the game, caseif other players appear
    chars_read = recv(connection_fd, buffer, BUFFER_SIZE, 0);
    if (chars_read == -1) fatalError("recv");
    sscanf(buffer, "%d", &startGameFlag);
    if (startGameFlag == 1) {printf("GAME STARTED\n");}
    else { fatalError("Server error"); }
    bzero(&buffer, BUFFER_SIZE);

    // Step 3: Wait for your cards
    chars_read = recv(connection_fd, buffer, BUFFER_SIZE, 0);
    if (chars_read == -1) fatalError("recv");
    sscanf(buffer, "%d %d,%d %d", &f, &s, &f2, &s2);
    bzero(&buffer, BUFFER_SIZE);

    printf("DEALER:\n");
    int cds2[2] = {f2,s2};
    Hand * serverHand = newHand(2, cds2);
    printHand(serverHand,1);

    printf("YOU:\n");
    int cds[2] = {f,s};
    Hand * myHand = newHand(2, cds);
    printHand(myHand,0);
    
    do{
        showOptions();
        playerChoice = readIntInRange(1,2);    
        //Send HIT to the server
        //Wait NEW_CARD
    } while( playerChoice == 1 ); //Player hit

    //Send STAND
    //Wait answer of WINNER
    //SHOW
}

void fatalError(const char * message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

//it includes x and y = [x,y]
int readIntInRange(int x,int y){
    int res = x-1;
    int answer, c;
    do{
        printf("> ");
        answer = scanf("%d", &res);
        while((c = getchar()) != '\n' && c != EOF); //Flush4567
    }
    while(answer!=1 || res<x || res>y);
    return res;
}

void showOptions() {
    printf("Choose:\n"); 
    printf("1) Hit\n");
    printf("2) Stand\n");
}