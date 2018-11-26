/*
    RUBEN CUADRA A01019102
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
#include "onitama.h"

#define NUM_TABLES 5
#define BUFFER_SIZE 256
#define MAX_QUEUE 5

//Table struct
typedef struct table_struct {
    int tid;
    int p1_connection_fd;
    int p2_connection_fd;
    int status; //ENUM
    pthread_mutex_t table_mutex; //For updating status/fds
    onitama_board_t oni_board;
} table_t;

// GM Struct
typedef struct game_master_struct {
    pthread_mutex_t matches_mutex; //For wins/played
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
void startGame(table_t * table); //Contains clients_fd
void resetTable(table_t * t);
table_t * getRandomTable(table_t * tables_array);

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

    // gm_data.tables_array[0].oni_board.cards[0] = getCardById(7); 
    // gm_data.tables_array[0].oni_board.board[3][3] = BLUE_STUDENT  ;
    // gm_data.tables_array[0].oni_board.board[4][3] = 0  ;
    // print(&gm_data.tables_array[0].oni_board);
    // int done = move(&gm_data.tables_array[0].oni_board,BLUE,getCardById(7),4,2,3,1);
    // printf(done?"done\n":"FAIL\n");
    // print(&gm_data.tables_array[0].oni_board);
    
    printf("STARTING\n");
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

//Table is in status playing and with locked mutex
void startGame(table_t * table){
    //COM
    char buffer[BUFFER_SIZE];
    int chars_read;
    //GAME
    int playing=0, waiting=1;
    int connections[2];         //0->BLUE, 1->RED
    int winner = NO_PLAYER;     //BLUE,RED,NO_PLAYER
    int scanned,fr,fc,tr,tc,mov_id;   //FromRow,FromCol,ToRow,ToCol
    card_t * to_use = NULL;

    if(1){//rand()%2){ //First one to sit will be the first one to move, with random it changes (PvE)
        connections[0] = table->p1_connection_fd;
        connections[1] = table->p2_connection_fd;
    }else{
        connections[0] = table->p2_connection_fd;
        connections[1] = table->p1_connection_fd;
    }
    bzero(&buffer, BUFFER_SIZE);
    //SEND players game started, and their colors
    //BLUE 
    sprintf(buffer, "%d %d %d %d %d %d", 0, table->oni_board.cards[0]->id,table->oni_board.cards[1]->id,table->oni_board.cards[2]->id,table->oni_board.cards[3]->id,table->oni_board.cards[4]->id ); //BLUE = 0, RED = 1
    if ( send(connections[0], buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
    {
        printf("Client disconnected - GAME STARTED\n");
        resetTable(table); 
        //TODO AVISAR AL OTRO??
        return;
    } 
    bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
    //RED
    sprintf(buffer, "%d %d %d %d %d %d", 1, table->oni_board.cards[0]->id,table->oni_board.cards[1]->id,table->oni_board.cards[2]->id,table->oni_board.cards[3]->id,table->oni_board.cards[4]->id ); //BLUE = 0, RED = 1
    if ( send(connections[1], buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
    {
        printf("Client disconnected - GAME STARTED\n");
        resetTable(table);
        //TODO AVISAR AL OTRO??
        return;
    } 
    bzero(&buffer, BUFFER_SIZE);          //Clean Buffer

    while(winner == NO_PLAYER){
        // print(&table->oni_board);
        // printf("%s turn\n> ",playing==0?"BLUE":"RED");
        //RECV fr,fc,tr,tc,mov_id <- connections[playing]
        chars_read = recv(connections[playing], buffer, sizeof buffer, 0);
        if (chars_read == 0) {
            printf("Client disconnected - GET MOVEMENT\n");
            resetTable(table);
            //TODO AVISAR AL OTRO??
            return;
        } //RECV fr,fc,tr,tc,mov_id <- client_fd aka. Movement
        scanned = sscanf(buffer, "%d %d %d %d %d", &fr,&fc,&tr,&tc,&mov_id);
        bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
        //IF Valid Card
        if (scanned==5 && (to_use = getCardById(mov_id))!=NULL) 
        {   
            //IF movement was done
            if(move(&table->oni_board,playing==0?BLUE:RED,to_use,fr,fc,tr,tc) == 1){
                //SEND OK                 -> connections[playing] 
                printf("MOVEMENT DONE, SENDING OK TO PLAYER THAT PlAYED\n");
                bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
                sprintf(buffer, "%d", OK); 
                if ( send(connections[playing], buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                {
                    printf("Client disconnected - WAITING MOVEMENT ANSWER\n");
                    resetTable(table);
                    //TODO avisar al otro?
                    return;
                } 
                bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
                usleep(50000); //Delay, es mas rapido aqui que el del cliente
                //WAIT, CLIENT IS DOING THE MOVEMENT
                printf("MOVEMENT DONE, SENDING MOVEMENT TO WAITING PLAYER\n");
                //SEND fr,fc,tr,tc,mov_id -> connections[waiting] //Movement done by other
                sprintf(buffer, "%d %d %d %d %d", fr,fc,tr,tc,mov_id); 
                if ( send(connections[waiting], buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                {
                    printf("Client disconnected - SENDING OPPONENT MOVEMENT\n");
                    resetTable(table);
                    return;
                } 

                //Change Turn
                playing=(playing+1)%2;
                waiting=(waiting+1)%2;
                winner = getWinner(&table->oni_board);//Check winner
                continue;
            }else{
                //SEND MOVEMENT_ERROR -> connections[playing]
                printf("PLAYER %s SCANNED %d: ERROR TRYING TO MOVE %d %d %d %d %s\n", playing==0?"BLUE":"RED" ,scanned,fr,fc,tr,tc, getCardById(mov_id)->name);
                print( &table->oni_board );
                sprintf(buffer, "%d", WRONG_MOVEMENT); 
                if ( send(connections[playing], buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                {
                    printf("Client disconnected - WRONG CARD\n");
                    resetTable(table);
                    //TODO avisar al otro?
                    return;
                } 
                bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
            }
        }
        else{
            //SEND CARD_ERROR -> connections[playing]
            sprintf(buffer, "%d", WRONG_CARD); 
            if ( send(connections[playing], buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
            {
                printf("Client disconnected - WRONG CARD\n");
                resetTable(table);
                //TODO avisar al otro?
                return;
            } 
            bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
            continue;
        }
    }
    printf("Winner is %s\n",winner==0?"BLUE":"RED");
}

//Spawn proc to play for us
void playVsPlayer(int client_fd, int difficulty){
    //COM
    char buffer[BUFFER_SIZE];
    int chars_read;
    //PYTHON COMM
    char answer[BUFFER_SIZE];
    char boardtext[70];
    char command[140];
    char mov_name[CARD_NAME_SIZE];
    //INIT BOARD
    printf("INIT BOARD\n");
    onitama_board_t onit;
    initBoard(&onit);
    printf("BOARD\n");
    //START
    int playing=0;
    int player = rand()%2;      //His turn, first or second
    int us     = player==0?RED:BLUE;
    int winner = NO_PLAYER;     //BLUE,RED,NO_PLAYER
    int scanned,fr,fc,tr,tc,mov_id;   //FromRow,FromCol,ToRow,ToCol
    card_t * to_use = NULL;
    printf("SENDING INFO, WE %d, PLAYER %d\n",us,player);
    //SEND players his color(turn) and cards
    sprintf(buffer, "%d %d %d %d %d %d", player, onit.cards[0]->id,onit.cards[1]->id,onit.cards[2]->id,onit.cards[3]->id,onit.cards[4]->id ); //BLUE = 0, RED = 1
    printf("%s\n",buffer);
    if ( send(client_fd, buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
    {
        printf("Client disconnected - GAME STARTED\n");
        destroyBoard(&onit);
        return;
    } 
    bzero(&buffer, BUFFER_SIZE);  
    while(winner == NO_PLAYER){
        print(&onit);
        if(playing == player){
            printf("PLAYER\n");
            //Get movement from player
            chars_read = recv(client_fd, buffer, sizeof buffer, 0);
            if (chars_read == 0) {
                printf("Client disconnected - GET MOVEMENT\n");
                destroyBoard(&onit);
                return;
            } //RECV fr,fc,tr,tc,mov_id <- client_fd aka. Movement
            scanned = sscanf(buffer, "%d %d %d %d %d", &fr,&fc,&tr,&tc,&mov_id);
            printf("RECV %s\n",buffer);
            bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
            if (scanned < 5){ //WRONG FORMAT, TRY AGAIN
                sprintf(buffer, "%d", WRONG_MOVEMENT); 
                if ( send(client_fd, buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                {
                    printf("Client disconnected - WRONG FORMAT\n");
                    destroyBoard(&onit);
                    return;
                } 
                bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
                continue;
            }
            to_use  = getCardById(mov_id);
            //IF Valid Card
            if (to_use != NULL) 
            {
                //IF movement was done
                if(move(&onit,player==0?BLUE:RED,to_use,fr,fc,tr,tc) == 1){
                    sprintf(buffer, "%d", OK); 
                    if ( send(client_fd, buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                    {
                        printf("Client disconnected - WRONG CARD\n");
                        destroyBoard(&onit);
                        return;
                    } 
                    bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
                    //Change Turn
                    playing=(playing+1)%2;
                    winner = getWinner(&onit);//Check winner
                    continue;
                }else{
                    printf("WRONG MOVEMENT %s %s %d %d %d %d\n",player==0?"BLUE":"RED",to_use->name,fr,fc,tr,tc);
                    sprintf(buffer, "%d", WRONG_MOVEMENT); 
                    if ( send(client_fd, buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                    {
                        printf("Client disconnected - WRONG MOVEMENT\n");
                        destroyBoard(&onit);
                        return;
                    } 
                    bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
                }
            }
            else{
                sprintf(buffer, "%d", WRONG_CARD); 
                if ( send(client_fd, buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                {
                    printf("Client disconnected - WRONG CARD\n");
                    destroyBoard(&onit);
                    return;
                } 
                bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
                continue;
            }
        }else{
            printf("COMPUTER\n");
            boardToParams(&onit,boardtext);
            sprintf( command, "%s %s \"%s\" %d %d","python3","game_logic.py",boardtext,us,2);
            printf("%s\n", command);
            FILE *fp; 
            /* Open the command for reading. */
            fp = popen(command, "r");
            if (fp == NULL) {
                printf("Failed to run python script\n" );
                return; //ERROR Decirle que al jugador que gano
            }
            fgets(answer, sizeof(answer)-1, fp); //Read Result
            printf("%s\n",answer);
            scanned = sscanf(answer, "%d %d %d %d %s", &fr,&fc,&tr,&tc,mov_name);
            to_use  = getCard(mov_name);
            pclose(fp); /* close */
            if (scanned != 5 || to_use == NULL) 
            {
                printf("ERROR on python script, check log\n" );
                return; //ERROR Decirle al jugador que gano
            }
            if(move(&onit,us,to_use,fr,fc,tr,tc) == 1){
                printf("SENDING \n");
                sprintf(buffer, "%d %d %d %d %d", fr,fc,tr,tc,to_use->id); 
                if ( send(client_fd, buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                {
                    printf("Client disconnected - BOT ANSWER\n");
                    destroyBoard(&onit);
                    return;
                } 
                //Clean Buffers
                bzero(&buffer, BUFFER_SIZE);         
                bzero(&answer, BUFFER_SIZE);          
                bzero(&boardtext, 70);          
                bzero(&command, 140);
                bzero(&mov_name, CARD_NAME_SIZE); 
                //Change Turn
                playing=(playing+1)%2;
                winner = getWinner(&onit);//Check winner
                continue;
            }
            else{
                printf("ERROR, WRONG MOVEMENT BY SCRIPT\n");
                return; //ERROR
            }
            break;
        }
    }
    printf("Winner is %s\n",winner==0?"BLUE":"RED");
    //Kill Board
    destroyBoard(&onit);
}

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
    initOnitama(); 

    //Init stats
    gm_data->total_wins = 0;
    gm_data->total_played = 0;
    
    // Allocate the arrays in the structures
    gm_data->tables_array = malloc(NUM_TABLES * sizeof (table_t));
    pthread_mutex_init(&gm_data->matches_mutex, NULL);
    // set tables data
    for (int i = 0; i < NUM_TABLES; ++i)
    {
        gm_data->tables_array[i].tid = i;
        gm_data->tables_array[i].status = EMPTY;
        gm_data->tables_array[i].p1_connection_fd = -1;
        gm_data->tables_array[i].p2_connection_fd = -1;
        pthread_mutex_init(&gm_data->tables_array[i].table_mutex, NULL);
        initBoard(&gm_data->tables_array[i].oni_board);
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
        else if (poll_response == 0){} // printf("No response after %d seconds\n", timeout);
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
    int opponent_fd,inTable;
    thread_data_t * tdt = (thread_data_t *) arg;
    int finish = 0;
    printf("ATTENDING\n");
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
        printf("SWITCH PVP OR PVE\n");
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
                sprintf(buffer, "%d", t==1?OK:WRONG_DIFFICULTY);     //Matched option
                if ( send(tdt->client_fd, buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                {
                    printf("Client disconnected - SENDING DIFFICULTY ANSWER\n");
                    close(tdt->client_fd);    
                    pthread_exit(NULL); //we get here with the finish = 1 or interrupted
                } 
                printf("STARTING playVsPlayer\n");
                playVsPlayer(tdt->client_fd, difficulty); //Will call subproc in python
                finish = 1;
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
                //Check if it is a valid table
                if(table == RANDOM_TABLE || (table >= 0  && table < NUM_TABLES) ){ 
                    table_t *tt = table==RANDOM_TABLE?getRandomTable(tdt->gm_data->tables_array):&(tdt->gm_data->tables_array[table]);
                    //TODO
                    pthread_mutex_lock(&tt->table_mutex);
                    switch(tt->status){
                        case EMPTY: //Assign user to table, both fd are free, pick the first one
                            tt->p1_connection_fd = tdt->client_fd;
                            tt->status = FREE; //Change table status to FREE (1 empty seat)
                            sprintf(buffer, "%d", 1); //Send number of users in table
                            if ( send(tdt->client_fd, buffer, strlen(buffer)+1, 0) == -1 ) //(from here Client waits GAME_STARTED flag)
                            {
                                //IF it fails, return to EMPTY and release lock
                                tt->p1_connection_fd = -1;
                                tt->status = EMPTY;
                            } //After this it is the same as default, leave until another one picks the table
                        default: //PLAYING? this is an error, it should have been blocked
                            pthread_mutex_unlock(&tt->table_mutex);
                            pthread_exit(NULL);
                            break;
                        case FREE: //Assign user to table(WHERE px_connection_id == -1)
                            opponent_fd = -1;
                            inTable = 2;
                            if     (tt->p1_connection_fd == -1) {
                                tt->p1_connection_fd = tdt->client_fd;
                                opponent_fd = tt->p2_connection_fd;
                            }
                            else if(tt->p2_connection_fd == -1) {
                                tt->p2_connection_fd = tdt->client_fd;
                                opponent_fd = tt->p1_connection_fd;
                            }
                            else   { //Weird error, FREE but no fd, reset table
                                tt->p1_connection_fd = -1;
                                tt->p2_connection_fd = -1;
                                tt->status = EMPTY;
                                pthread_mutex_unlock(&tt->table_mutex);
                                close(tdt->client_fd);    
                                pthread_exit(NULL);
                            }
                            
                            //CHECK opponent is still connected
                            bzero(&buffer, BUFFER_SIZE);    //Clean Buffer
                            sprintf(buffer, "%d", inTable); //Send number of users in table to OPPONENT that was waiting
                            if ( send( opponent_fd, buffer, strlen(buffer)+1, 0) == -1 ) 
                            {   //IF it fails then the player that was waiting is gone, remove him
                                inTable -= 1; 
                                if (tt->p1_connection_fd == opponent_fd) tt->p1_connection_fd = -1;
                                else                                     tt->p2_connection_fd = -1;
                                tt->status = FREE;
                            } //After this it is the same as default, leave until another one picks the table
                            
                            //CHECK that we are still connected
                            bzero(&buffer, BUFFER_SIZE);    //Clean Buffer
                            sprintf(buffer, "%d", inTable); //Send number of users in table to OUR client
                            if ( send( tdt->client_fd, buffer, strlen(buffer)+1, 0) == -1 ) 
                            {   //IF it fails then the our client is gone, remove him
                                inTable -= 1; 
                                if (tt->p1_connection_fd == tdt->client_fd) tt->p1_connection_fd = -1;
                                else                                        tt->p2_connection_fd = -1;
                                if (inTable == 0 ) tt->status = EMPTY; 
                            } 
                            
                            if (inTable == 2) //START MATCH
                            {
                                tt->status = PLAYING;
                                startGame(tt); //Here they play
                                //GAME IS OVER, FREE EVERYTHING
                                tt->p1_connection_fd = -1;
                                tt->p2_connection_fd = -1;
                                tt->status = EMPTY;
                                //RESET BOARD
                                resetBoard(&tt->oni_board);
                            }

                            pthread_mutex_unlock(&tt->table_mutex);
                            close(tdt->client_fd);    
                            pthread_exit(NULL);
                            break;
                    }
                }else{
                    bzero(&buffer, BUFFER_SIZE);          //Clean Buffer
                    sprintf(buffer, "%d", WRONG_TABLE);   //Matched option
                    sendString(tdt->client_fd, buffer);   //Return answer and continue    
                }
                break;
            case EXIT:
                bzero(&buffer, BUFFER_SIZE);         //Clean Buffer
                sprintf(buffer, "%d", BYE);           //Prepare BYE
                sendString(tdt->client_fd, buffer);   //send it
                finish = 1;                           //BYE
                break;
            default:
                bzero(&buffer, BUFFER_SIZE);         //Clean Buffer
                sprintf(buffer, "%d", ERROR);         //prepare ERROR
                sendString(tdt->client_fd, buffer);   //send it
                break;                                //Continue
        }
    }    
    close(tdt->client_fd);    
    pthread_exit(NULL); //we get here with the finish = 1 or interrupted
}

//TODO
table_t * getRandomTable(table_t * tables_array){
    //First try for a FREE table (somebody is waiting)
    for (int i = 0; i < NUM_TABLES; ++i)
        if ( tables_array[i].status == FREE) 
            return &tables_array[i];
    //Then try for an EMPTY table (nobody is waiting)
    for (int i = 0; i < NUM_TABLES; ++i)
        if ( tables_array[i].status == EMPTY) 
            return &tables_array[i];
    return  &tables_array[0]; //everything is FULL, just return the first one
}

void shutDownGM(gm_t * gm_data){
    destroyOnitama();
    free(gm_data->tables_array);
}

void onCtrlC(int arg){
    interrupted = 1;
    printf("Shutting down...\n");
    exit(1);
}

void resetTable(table_t * t){
    t->status = EMPTY;
    t->p1_connection_fd = -1;
    t->p2_connection_fd = -1; 
    resetBoard(&t->oni_board);  
}
