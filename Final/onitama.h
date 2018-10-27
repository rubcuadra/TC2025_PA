#ifndef ONITAMA_H
#define ONITAMA_H

typedef enum valid_tokens {EMPTY = 0, BLUE_MASTER, BLUE_STUDENT, RED_MASTER, RED_STUDENT} tokens_s;
typedef enum {BLUE,RED} players_s; //Blue is the gone below, movements are according to him 
//It represents the movements within the matrix
typedef struct movement{
	int x;
	int y;
} movement_t;

typedef struct card {
	int id;
	char        *  name;
	movement_t  ** movements;
} card_t;

typedef struct onitama {
	card_t ** cards;//We point to the deck
    int ** board;   //Matrix with valid_tokens values
} onitama_board_t;

void initOnitama();
void initBoard(onitama_board_t * oniBoard);
void destroyBoard(onitama_board_t * oniBoard);
void destroyOnitama();
void print(onitama_board_t * oniBoard);
//For Debug
int main();

#endif  /* ONITAMA_H */