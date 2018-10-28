#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "onitama.h"

#define BOARD_SIZE 5       //Board is 5x5
#define NUM_CARDS  16      //16 cards in the normal deck
#define CARD_NAME_SIZE 10  //'ELEPHANT' is the longest one
#define VALID_MOVEMENTS 13 
#define MAX_MOVEMENTS_PER_CARD 4
#define CARDS_PER_BOARD 5 //2 per player + 1
card_t     * deck;//Cards 
movement_t * vm;  //Valid Movements

int main(){
	initOnitama();
		onitama_board_t onit;
		initBoard(&onit);
			print(&onit);
		destroyBoard(&onit);
	destroyOnitama();
	return 0;
}

//Should be called before creating boards, inits data
void initOnitama(){
	//Init generators
	time_t t; 
	srand((unsigned) time(&t));
	//Movements inside the matrix used by the cards	
	vm = malloc(NUM_CARDS * sizeof(card_t)); 
	vm[0].x  =  1; vm[0].y  =  1; //DR  0
	vm[1].x  =  1; vm[1].y  = -1; //DL  1
	vm[2].x  = -1; vm[2].y  =  1; //UR  2
	vm[3].x  = -1; vm[3].y  = -1; //UL  3
	vm[4].x  = -1; vm[4].y  =  0; //U   4
	vm[5].x  = -2; vm[5].y  =  0; //UU  5
	vm[6].x  =  1; vm[6].y  =  0; //D   6
	vm[7].x  =  0; vm[7].y  =  1; //R   7
	vm[8].x  =  0; vm[8].y  =  2; //RR  8
	vm[9].x  =  0; vm[9].y  = -1; //L   9
	vm[10].x =  0; vm[10].y = -2; //LL  10
	vm[11].x = -1; vm[11].y = -2; //LLU 11
	vm[12].x = -1; vm[12].y =  2; //RRU 12
	
	//Init Deck
	deck = malloc(NUM_CARDS * sizeof(card_t)); 
	for (int i = 0; i < NUM_CARDS; ++i)
	{
		deck[i].id        = i;
		deck[i].name      = malloc(CARD_NAME_SIZE * sizeof(char)); 
		deck[i].movements = malloc(MAX_MOVEMENTS_PER_CARD * sizeof(movement_t *)); 
	}
	//SET CARDS
	strncpy(deck[0].name, "MANTIS", CARD_NAME_SIZE);
	deck[0].movements[0] = &vm[3];
	deck[0].movements[1] = &vm[2];
	deck[0].movements[2] = &vm[6];

	strncpy(deck[1].name, "OX", CARD_NAME_SIZE);
	deck[1].movements[0] = &vm[4];
	deck[1].movements[1] = &vm[7];
	deck[1].movements[2] = &vm[6];

	strncpy(deck[2].name, "HORSE", CARD_NAME_SIZE);
	deck[2].movements[0] = &vm[4];
	deck[2].movements[1] = &vm[9];
	deck[2].movements[2] = &vm[6];

	strncpy(deck[3].name, "EEL", CARD_NAME_SIZE);
	deck[3].movements[0] = &vm[3];
	deck[3].movements[1] = &vm[1];
	deck[3].movements[2] = &vm[7];

	strncpy(deck[4].name, "CRANE", CARD_NAME_SIZE);
	deck[4].movements[0] = &vm[1];
	deck[4].movements[1] = &vm[0];
	deck[4].movements[2] = &vm[4];

	strncpy(deck[5].name, "BOAR", CARD_NAME_SIZE);
	deck[5].movements[0] = &vm[9];
	deck[5].movements[1] = &vm[7];
	deck[5].movements[2] = &vm[4];

	strncpy(deck[6].name, "COBRA", CARD_NAME_SIZE);
	deck[6].movements[0] = &vm[9];
	deck[6].movements[1] = &vm[2];
	deck[6].movements[2] = &vm[0];

	strncpy(deck[7].name, "MONKEY", CARD_NAME_SIZE);
	deck[7].movements[0] = &vm[3];
	deck[7].movements[1] = &vm[2];
	deck[7].movements[2] = &vm[1];
	deck[7].movements[3] = &vm[0];

	strncpy(deck[8].name, "ELEPHANT", CARD_NAME_SIZE);
	deck[8].movements[0] = &vm[3];
	deck[8].movements[1] = &vm[2];
	deck[8].movements[2] = &vm[9];
	deck[8].movements[3] = &vm[7];

	strncpy(deck[9].name, "ROOSTER", CARD_NAME_SIZE);
	deck[9].movements[0] = &vm[0];
	deck[9].movements[1] = &vm[2];
	deck[9].movements[2] = &vm[9];
	deck[9].movements[3] = &vm[7];

	strncpy(deck[10].name, "GOOSE", CARD_NAME_SIZE);
	deck[10].movements[0] = &vm[3];
	deck[10].movements[1] = &vm[0];
	deck[10].movements[2] = &vm[9];
	deck[10].movements[3] = &vm[7];

	strncpy(deck[11].name, "TIGER", CARD_NAME_SIZE);
	deck[11].movements[0] = &vm[5];
	deck[11].movements[1] = &vm[6];

	strncpy(deck[12].name, "RABBIT", CARD_NAME_SIZE);
	deck[12].movements[0] = &vm[1];
	deck[12].movements[1] = &vm[2];
	deck[12].movements[2] = &vm[8];

	strncpy(deck[13].name, "FROG", CARD_NAME_SIZE);
	deck[13].movements[0] = &vm[0];
	deck[13].movements[1] = &vm[3];
	deck[13].movements[2] = &vm[10];

	strncpy(deck[14].name, "CRAB", CARD_NAME_SIZE);
	deck[14].movements[0] = &vm[10];
	deck[14].movements[1] = &vm[4];
	deck[14].movements[2] = &vm[8];

	strncpy(deck[15].name, "DRAGON", CARD_NAME_SIZE);
	deck[15].movements[0] = &vm[1];
	deck[15].movements[1] = &vm[0];
	deck[15].movements[2] = &vm[11];
	deck[15].movements[3] = &vm[12];
}

void initBoard(onitama_board_t * oniBoard){
	//Alloc Space for board & cards
	oniBoard->board = malloc(BOARD_SIZE * sizeof(int*)); //Rows
    for (int c = 0 ; c < BOARD_SIZE ; ++c )       //Cols
        oniBoard->board[c] = calloc(BOARD_SIZE, sizeof(int)); 
    oniBoard->cards = malloc( CARDS_PER_BOARD*sizeof(card_t*) );
    //Set initial Tokens
    oniBoard->board[0][2] = RED_MASTER  ;
		oniBoard->board[0][0] = RED_STUDENT ;
		oniBoard->board[0][1] = RED_STUDENT ;
		oniBoard->board[0][3] = RED_STUDENT ;
		oniBoard->board[0][4] = RED_STUDENT ;
	oniBoard->board[4][2] = BLUE_MASTER ;
		oniBoard->board[4][0] = BLUE_STUDENT;
		oniBoard->board[4][1] = BLUE_STUDENT;
		oniBoard->board[4][3] = BLUE_STUDENT;
		oniBoard->board[4][4] = BLUE_STUDENT;
	
	//Set initial Cards, random without repetition
	int ix = 0, add, new_num;
	while (ix < CARDS_PER_BOARD){
		add = 1;
		new_num = rand()%NUM_CARDS; //[0,NUM_CARDS)
		//Check that the card is not already in the array
		for (int j = 0; j < CARDS_PER_BOARD; ++j)
		{
			if(oniBoard->cards[j] == NULL) break;  //End of array
			//Already in arr, get other card
			if( oniBoard->cards[j]->id == new_num) {
				add = 0;
				break;
			}
		}
		if (add == 1) {
			oniBoard->cards[ix] = &deck[new_num];
			ix++;
		}
	}	
}

void destroyBoard(onitama_board_t * oniBoard){
	for (int c = 0 ; c < BOARD_SIZE ; ++c )
        free( oniBoard->board[c] );
    free(oniBoard->board);
    //Check this, we are pointing to deck and we do not want to delete deck
    free(oniBoard->cards); 
}

void print(onitama_board_t * oniBoard){
	for (int i = 0; i < BOARD_SIZE; ++i){
		for (int j = 0; j < BOARD_SIZE; ++j){
			printf("%d ",oniBoard->board[i][j]);
		}
		printf("\n");
	}
	printf("BLUE\n");
	printf("\t%s\n",oniBoard->cards[0]->name);
	printf("\t%s\n",oniBoard->cards[1]->name);
	printf("RED\n");
	printf("\t%s\n",oniBoard->cards[2]->name);
	printf("\t%s\n",oniBoard->cards[3]->name);
	printf("EXTRA\n");
	printf("\t%s\n",oniBoard->cards[4]->name);	
}

void destroyOnitama(){
	free(vm);
	for (int i = 0; i < NUM_CARDS; ++i)
	{
		free(deck[i].name);
		free(deck[i].movements);
	}
	free(deck);
}

int tokenIsOfPlayer(players_s p, tokens_s tok){
	return p==BLUE?(tok == BLUE_MASTER || tok == BLUE_STUDENT):(tok == RED_MASTER  || tok == RED_STUDENT );
}

int canMove(onitama_board_t * oniBoard,players_s p,card_t * c,int fromRow,int fromCol,int toRow,int toCol){
	//Cells in range
	if (fromRow > -1 && toRow > -1 && fromCol > -1 && toCol > -1 && fromRow < BOARD_SIZE && toRow < BOARD_SIZE && fromCol < BOARD_SIZE && toCol < BOARD_SIZE){
		// From is player's token AND To is not players token
        if( tokenIsOfPlayer(p, oniBoard->board[fromRow][fromCol]) && tokenIsOfPlayer(p, oniBoard->board[toRow][toRow])==0 )
        {
        	// Validate that the player has that card 
        	if ( (p==BLUE && (oniBoard->cards[0]->id==c->id || oniBoard->cards[1]->id==c->id)) || 
        	     (p==RED  && (oniBoard->cards[2]->id==c->id || oniBoard->cards[3]->id==c->id)) )
        	{ 	
        		// Get movement vector
        		int movement_x = p==BLUE?toRow-fromRow:fromRow-toRow;
        		int movement_y = p==BLUE?toCol-fromCol:fromCol-toCol;
        		//Validate card generates that movement
				for (int i = 0; i<MAX_MOVEMENTS_PER_CARD; ++i)
				{
					if (c->movements[i] == NULL) break; //No more movements
					if (c->movements[i]->x == movement_x && 
						c->movements[i]->y == movement_y )
						return 1; //Valid Movement
				}        		
        	}

        }
	}
	return 0; //False
}

// def canMove(self, player, fromCell, card, toCell):
//         fromRow, fromCol = fromCell
//         toRow,     toCol = toCell
//         #Checar indices
//         if fromRow > -1 and toRow > -1 and fromCol > -1 and toCol > -1 and fromRow < 5 and toRow < 5 and fromCol < 5 and toCol < 5:
//             #Checar indices origen y destino (que no se salgan)
//             #Origin is player's token
//             #Destin is NOT player's token
//             #Player has that card
//             if self.isPlayer(player,self[fromRow][fromCol]) and not self.isPlayer(player,self[toRow][toCol]) and card in self.cards[player]:
//                 mov = (toCell[0]-fromCell[0],toCell[1]-fromCell[1]) if player is self.BLUE else (fromCell[0]-toCell[0],fromCell[1]-toCell[1])
//                 return mov in OnitamaCards[card]
//         return False