#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "onitama.h"

card_t     * deck;//Cards 
movement_t * vm;  //Valid Movements

//SIMPLE EXAMPLE
// int main(){
// 	initOnitama();
// 		onitama_board_t onit;
// 		initBoard(&onit);
// 			print(&onit);
// 			int r = move(&onit,BLUE, getCard("TIGER") , 4,0,2,0 );
// 			if(r) print(&onit);
// 		destroyBoard(&onit);
// 	destroyOnitama();
// 	return 0;
// }

//Should be called before creating boards, inits data
void initOnitama(){
	//Init generators
	time_t t; 
	srand((unsigned) time(&t)); // srand(0);
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

void boardToParams(onitama_board_t * oniBoard,char * loc){
	// " bR   B  rrb      rrb    ;MONKEY FROG;HORSE MANTIS;EEL"
	int c = 0;
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			loc[c++] = oniBoard->board[i][j]+'0'; //TO ASCII VALUE
		}
	}
	loc[c++] = ';';
	//Add Cards
	sprintf( (char *)(loc+c), "%s %s;%s %s;%s",oniBoard->cards[0]->name,oniBoard->cards[1]->name,oniBoard->cards[2]->name,oniBoard->cards[3]->name,oniBoard->cards[4]->name);
}

void initBoard(onitama_board_t * oniBoard){
	//Alloc Space for board & cards
	printf("initBoard Start\n");
	oniBoard->board = malloc(BOARD_SIZE * sizeof(int*)); //Rows
    for (int c = 0 ; c < BOARD_SIZE ; ++c )       //Cols
        oniBoard->board[c] = calloc(BOARD_SIZE, sizeof(int)); 
    printf("BOARD ALLOC\n");
    oniBoard->cards = calloc( CARDS_PER_BOARD,sizeof(card_t*) );
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
	printf("FILL BOARD\n");
	//Set initial Cards, random without repetition
	int ix = 0, add, new_num;
	while (ix < CARDS_PER_BOARD){
		add = 1;
		new_num = rand()%NUM_CARDS; //[0,NUM_CARDS)
		printf("%d %d\n",ix, new_num);
		//Check that the card is not already in the array
		for (int j = 0; j < CARDS_PER_BOARD; ++j)
		{
			printf("CHECK %d %d\n",j,CARDS_PER_BOARD);
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
	printf("RANDOM CARDS SET\n");
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

card_t * getCard(char * name){
	for (int i = 0; i < NUM_CARDS; ++i)
		if( strcmp( deck[i].name,name) == 0 ) 
			return &deck[i];
	return NULL;
}

card_t * getCardById(int _id){
	if(_id >= 0 && _id < NUM_CARDS) return &deck[_id];
	return NULL;
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
        if( tokenIsOfPlayer(p, oniBoard->board[fromRow][fromCol]) && tokenIsOfPlayer(p, oniBoard->board[toRow][toCol])==0 )
        {
        	printf("USER TOKEN\n");
        	// Validate that the player has that card 
        	if ( (p==BLUE && (oniBoard->cards[0]->id==c->id || oniBoard->cards[1]->id==c->id)) || 
        	     (p==RED  && (oniBoard->cards[2]->id==c->id || oniBoard->cards[3]->id==c->id)) )
        	{ 	
        		printf("USER CARD\n");
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

int move(onitama_board_t * oniboard,players_s p,card_t * c,int fromRow,int fromCol,int toRow,int toCol){
	if(canMove(oniboard,p,c,fromRow,fromCol,toRow,toCol) == 0) return 0;
	//Update Board	
	// int toToken   = oniboard->board[toRow][toCol]; //maybe check if it is a master or endline for gameover?
	oniboard->board[toRow][toCol]     = oniboard->board[fromRow][fromCol];
	oniboard->board[fromRow][fromCol] = EMPTY_TOKEN; 		   //From goes EMPTY
	//Update Cards
	card_t * standBy = oniboard->cards[4];
	if (p == BLUE)
	{
		if(oniboard->cards[0]->id==c->id) //Switch with 0
		{
			oniboard->cards[4] = oniboard->cards[0];
			oniboard->cards[0] = standBy;
		}
		else{ 							  //Switch with 1
			oniboard->cards[4] = oniboard->cards[1];
			oniboard->cards[1] = standBy;
		}
	}
	else
	{
		if(oniboard->cards[2]->id==c->id) //Switch with 2
		{
			oniboard->cards[4] = oniboard->cards[2];
			oniboard->cards[2] = standBy;
		}
		else{ 							  //Switch with 3
			oniboard->cards[4] = oniboard->cards[3];
			oniboard->cards[3] = standBy;
		}
	}
	return 1;
}

int getWinner(onitama_board_t * oniboard){
	//A master arrived to the other start
	if (oniboard->board[0][2] == BLUE_MASTER) return BLUE;
	if (oniboard->board[4][2] == RED_MASTER)  return RED;

	int red_alive = 0, blue_alive = 0;
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			if      ( oniboard->board[i][j] == BLUE_MASTER) blue_alive = 1;
			else if ( oniboard->board[i][j] ==  RED_MASTER) red_alive  = 1;
			if( red_alive==1 && blue_alive==1) return NO_PLAYER; //No Winner yet
		}
	}
	//Case both alive already checked, just 1 master remains
	if (red_alive==1) return RED;
	else 			  return BLUE;
}
