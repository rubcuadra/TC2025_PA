/*
    RUBEN CUADRA A01019102
*/
#ifndef ONITAMA_H
#define ONITAMA_H

#define BOARD_SIZE 5       //Board is 5x5
#define NUM_CARDS  16      //16 cards in the normal deck
#define CARD_NAME_SIZE 10  //'ELEPHANT' is the longest one
#define VALID_MOVEMENTS 13 
#define MAX_MOVEMENTS_PER_CARD 4
#define CARDS_PER_BOARD 5 //2 per player + 1

typedef enum valid_tokens {EMPTY_TOKEN = 0, BLUE_MASTER, BLUE_STUDENT, RED_MASTER, RED_STUDENT} tokens_s;
typedef enum {BLUE,RED,NO_PLAYER=-1} players_s; //Blue is the gone below, movements are according to him 
//It represents the movements within the matrix
typedef struct movement{
	int x;
	int y;
} movement_t;

typedef struct card {
	int id;
	char        *  name;
	movement_t  ** movements; //we iterate until MAX_MOVEMENTS_PER_CARD || if it is NULL
} card_t;

//Cards len == 5; 
//	0,1 -> BLUE
//	2,3 -> RED
//	4   -> EXTRA
typedef struct onitama {
	card_t ** cards;//We point to the deck
    int ** board;   //Matrix with valid_tokens values
} onitama_board_t;

void initOnitama();
void initBoard(onitama_board_t * oniBoard);
void destroyBoard(onitama_board_t * oniBoard);
void resetBoard(onitama_board_t * oniBoard);
void setRandomCards(onitama_board_t * oniBoard);
void destroyOnitama();
void print(onitama_board_t * oniBoard);
void boardToParams(onitama_board_t * oniBoard,char * loc);
card_t * getCard(char * name); //Used By Client
card_t * getCardById(int _id); //Used By Server communication
int canMove(onitama_board_t * oniBoard,players_s p,card_t * c,int fromRow,int fromCol,int toRow,int toCol);
int move(onitama_board_t * oniBoard,players_s p,card_t * c,int fromRow,int fromCol,int toRow,int toCol);
int tokenIsOfPlayer(players_s p, tokens_s tok);
int getWinner(onitama_board_t * oniBoard);

#endif  /* ONITAMA_H */
