//
//  Blackjack.h
//  4_blackjack
//
//  Created by Ruben Cuadra on 24/09/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//

#ifndef Blackjack_h
#define Blackjack_h

typedef struct Hand{
    int size;
    int score;
    int scoreX;
    int *cards;
} Hand;

/*
 1-10   Hearths
 11-20  Tiles
 21-30  Clovers
 31-40  Pikes
 
 41,42,43 JQK H
 44,45,46 JQK T
 47,48,49 JQK C
 50,51,52 JQK P
 */ 
int getRandomCard(){ return (int)((rand()%52)+1); }
int getCardValue(int card){
    if (card>40) return 10; //If it is J,Q,K or 10 return 10
    int mod = card%10;
    return mod==0?10:mod;   //else Return numeric value
}

//st size == 3 . Numeric Value
//fm size == 2 . Family Value
void setCardName(int c,char * st,char *fm){
    if ( c > 40 ){
        switch (c) {
            //Jacks
            case 41:
                strncpy(fm, "H", 3);
            case 44:
                strncpy(fm, "T", 3);
            case 47:
                strncpy(fm, "C", 3);
            case 50:
                strncpy(st, "J", 3);
                strncpy(fm, "P", 3);
                break;
            //Queens
            case 42:
                strncpy(fm, "H", 3);
            case 45:
                strncpy(fm, "T", 3);
            case 48:
                strncpy(fm, "C", 3);
            case 51:
                strncpy(st, "Q", 3);
                strncpy(fm, "P", 3);
                break;
            //Kings
            case 43:
                strncpy(fm, "H", 3);
            case 46:
                strncpy(fm, "T", 3);
            case 49:
                strncpy(fm, "C", 3);
            case 52:
                strncpy(st, "K", 3);
                strncpy(fm, "P", 3);
                break;
        }
    }
    else{
        //Card Family
        if (c > 30)      strncpy(fm, "P", 3);
        else if (c > 20) strncpy(fm, "C", 3);
        else if (c > 10) strncpy(fm, "T", 3);
        else             strncpy(fm, "H", 3);
        int m = c%10;
        //Card Name
        if (m == 1)      strncpy(st, "A", 3);
        sprintf(st,"%d", m==0?10:c%10);
    }
}

Hand * getRandomHand( int handSize ){
    Hand * r = (Hand *) malloc( sizeof(Hand) );
    r->size = handSize;
    r->cards    = (int*) malloc( handSize*sizeof(int) ) ;
    r->score     = 0;
    r->scoreX    = 0;
    int temp = 0;
    for (int i = 0; i < handSize; i++) {
        r->cards[i] = getRandomCard();
        temp = getCardValue( r->cards[i] );
        r->score += temp;
        r->scoreX += temp==1?11:temp; //Ace can take 2 values
    }
    return r;
}

void printHand(Hand * h){
    char cv[3], cf[2];
    
    for (int i = 0; i < h->size; i++) {
        setCardName(h->cards[i], cv, cf);
        printf("|-------|\n");
        if (cv[0]=='1' && cv[1]=='0') printf("|%s%s    |\n",cv,cf);
        else                          printf("|%s%s     |\n",cv,cf);
        printf("|       |\n");
        printf("|       |\n");
        printf("|       |\n");
        if (cv[0]=='1' && cv[1]=='0') printf("|    %s%s|\n",cv,cf);
        else                          printf("|     %s%s|\n",cv,cf);
        printf("|_______|\n\n");
    }
}


#endif /* Blackjack_h */
