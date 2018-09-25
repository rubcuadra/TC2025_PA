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
    int playerChoice;
    int playerScore; //Player Score
    int dealerScore; //Dealer's score
    int blackjack = 0; //False for now
    
    srand(time(0)); //Use the current time as seed generator

    printf("Place your bet:\n"); //We ask the player for the bet they'd like to do
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
    
    // Step 3: Wait for your cards
    chars_read = recv(connection_fd, buffer, BUFFER_SIZE, 0);
    if (chars_read == -1) fatalError("recv");
    sscanf(buffer, "%d", &startGameFlag);
    if (startGameFlag == 1) {printf("Start Game\n");}

    int cds[2] = {f,s};
    Hand * myHand = newHand(2, cds);
    printHand(myHand);
    
    // int w = compareHands(dealerHand, clientHand);
    // printf( "Winner is %s\n", w==1?"Dealer":(w==0?"Nobody":"Player") );
    // return 0;


    startDeck(playerDeckSize, playerDeck); //Show the player's hand to the player himself
    printCards(playerDeckSize, playerDeck);

    showOptions();
    scanf("%d", &playerChoice); //Give our player the choice to hit or stnad
    while(playerChoice < 1 && playerChoice > 2) {
        printf("Please re-enter your choice, it has to be Hit or Stand");
        scanf("%d", &playerChoice);
        printf("\n");
    }

    while(playerChoice == 1) { //If our player decides to hit a card
        hitCard(playerDeckSize, playerDeck);
        playerDeckSize += 1;
        printf("Your current cards are:\n");
        printCards(playerDeckSize, playerDeck);
        if (checkLose(playerDeckSize, playerDeck) == 1) { //We check to see if the player score has gone over 21
            printf("You have lost the game for going over 21, the dealer has gotten all your money\n");
            exit(0);
        } else {
            showOptions();
            scanf("%d", &playerChoice); //Give our player the choice to hit or stnad
            while(playerChoice < 1 && playerChoice > 2) {
                printf("Please re-enter your choice, it has to be Hit or Stand");
                scanf("%d", &playerChoice);
                printf("\n");
            }
        }
    }

    if((playerDeck[0] == 1 && playerDeck[1] == 10) || (playerDeck[0] == 10 && playerDeck[1] == 1)) {
        playerScore = 21;
        blackjack = 1;
    } // In case of a blackjack
    else {
        playerScore = getDeckValue(playerDeckSize, playerDeck);
    }
    
    // Prepare the response to the server
    sprintf(buffer, "%d\n", playerScore);

    // SEND
    // Send the response
    if (send(connection_fd, buffer, strlen(buffer) + 1, 0) == -1) {
        fatalError("send");
    }

    // Clear the buffer
    bzero(&buffer, BUFFER_SIZE);

    // RECV
    // Receive the request
    chars_read = recv(connection_fd, buffer, BUFFER_SIZE, 0);
    if (chars_read == -1) {
        fatalError("recv");
    }

    sscanf(buffer, "%d", &dealerScore);

    // Print the result
    if ( dealerScore > 21) {
        if( playerScore == 21 && blackjack == 1) {
            printf("\nYou have won through blackjack\n\nDealer's score: %d\nPlayer's score: %d\n\n", dealerScore, playerScore);
            printf("Total winnings: %f", betPrice * 2.5);
            printf("$\n");
        } else {
            printf("\nThe dealer has lost since his score is over 21\n\nDealer's score: %d\nPlayer's score: %d\n\n", dealerScore, playerScore);
        }  
    } else {
        if (dealerScore > playerScore) {
            printf("\nThe dealer has won all the money since his score is higher than yours\n\nDealer's score: %d\nPlayer's score: %d\n\n", dealerScore, playerScore);
            printf("Total winnings: -%d", betPrice);
            printf("$\n");
        }
        else if (dealerScore == playerScore) {
            printf("\nThe dealer and you have the same score, so your bet is returned\n\nDealer's score: %d\nPlayer's score: %d\n\n", dealerScore, playerScore);
            printf("No winnings nor losses\n");
        }
        else if (playerScore > dealerScore) {
            printf("\nYou have won due to a higher score, you get your winnings and double of that\n\nDealer's score: %d\nPlayer's score: %d\n\n", dealerScore, playerScore);
            printf("Total winnings: %d", betPrice * 2);
            printf("$\n");
        }
    }
}

void printCards(int deckSize, int deck[deckSize]) {
    for (int i = 0; i < deckSize; i++) { //Iterate through the deckSize to initialize the deck
        printf("%d\t", deck[i]);
    }
    printf("\n\n");
}

void startDeck(int deckSize, int deck[deckSize]) {
    for (int i = 0; i < deckSize; i++) { //Iterate through the deckSize to initialize the deck
        deck[i] = (rand() % 10 + 1); //Give cards between 1 and 11
    }
}

void hitCard(int deckSize, int deck[deckSize]) {
    deck[deckSize] = (rand() % 10 + 1); //add a new card to our deck
    deckSize = deckSize + 1;
}

int checkLose(int deckSize, int deck[deckSize]) {
    int total = 0; //Have a variable to store the total amount of score
    for (int i = 0; i < deckSize; i++) { 
        total += deck[i];
    }
    if (total > 21)  //Check if it's an automatic loss
        return 1;
    else
        return 0;
}

int getDeckValue(int deckSize, int deck[deckSize]) { 
    int total = 0;
    for (int i = 0; i < deckSize; i++) total += deck[i];
    return total;
}

void fatalError(const char * message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int readIntInRange(int x,int y){
    int res = x-1;
    int answer, c;
    do{
        printf("Write a number between %d and %d\n",x,y);
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