#ifndef ONITAMA_H
#define ONITAMA_H

typedef enum valid_tokens {EMPTY = 0, BLUE_MASTER, BLUE_STUDENT, RED_MASTER, RED_STUDENT} tokens_s;
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
void destroyOnitama();
void print(onitama_board_t * oniBoard);
int canMove(onitama_board_t * oniBoard,players_s p,card_t * c,int fromRow,int fromCol,int toRow,int toCol);
int move(onitama_board_t * oniBoard,players_s p,card_t * c,int fromRow,int fromCol,int toRow,int toCol);
int tokenIsOfPlayer(players_s p, tokens_s tok);
int getWinner(onitama_board_t * oniBoard);
//For Debug
int main();

#endif  /* ONITAMA_H */