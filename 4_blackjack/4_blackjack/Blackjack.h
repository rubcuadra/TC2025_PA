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
                strncpy(st, "J", 3);
                break;
            case 44:
                strncpy(fm, "T", 3);
                strncpy(st, "J", 3);
                break;
            case 47:
                strncpy(fm, "C", 3);
                strncpy(st, "J", 3);
                break;
            case 50:
                strncpy(fm, "P", 3);
                strncpy(st, "J", 3);
                break;
            //Queens
            case 42:
                strncpy(fm, "H", 3);
                strncpy(st, "Q", 3);
                break;
            case 45:
                strncpy(fm, "T", 3);
                strncpy(st, "Q", 3);
                break;
            case 48:
                strncpy(fm, "C", 3);
                strncpy(st, "Q", 3);
                break;
            case 51:
                strncpy(fm, "P", 3);
                strncpy(st, "Q", 3);
                break;
            //Kings
            case 43:
                strncpy(fm, "H", 3);
                strncpy(st, "K", 3);
                break;
            case 46:
                strncpy(fm, "T", 3);
                strncpy(st, "K", 3);
                break;
            case 49:
                strncpy(fm, "C", 3);
                strncpy(st, "K", 3);
                break;
            case 52:
                strncpy(fm, "P", 3);
                strncpy(st, "K", 3);
                break;
        }
        return;
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
        else             sprintf(st,"%d", m==0?10:c%10);
    }
}
Hand * newHand(int s, int * cards){
    Hand * r = (Hand *) malloc( sizeof(Hand) );
    r->size = s;
    r->cards = cards;
    r->score     = 0;
    r->scoreX    = 0;
    int temp = 0;
    for (int i = 0; i < s; i++) {
        temp = getCardValue( r->cards[i] );
        r->score += temp;
        r->scoreX += temp;
        if(r->scoreX == r->score && temp==1) r->scoreX+=10; //Ace as 11
    }
    return r;
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
        r->scoreX += temp;
        if(r->scoreX == r->score && temp==1) r->scoreX+=10; //Ace as 11
    }
    return r;
}
//Score - first card, it is hidden
int peekScore(Hand * h){
    int fval = getCardValue(h->cards[0]);
    return h->score - fval;
}

//hidden = Num of hidden cards
void printHand(Hand * h, int hidden){
    char cv[3], cf[2];
    int tempHidden = hidden;
    for (int i = 0; i < h->size; i++) printf("_________ "); printf("\n");
    for (int i = 0; i < h->size; i++) {
        setCardName(h->cards[i], cv, cf);
        if(hidden>0)
        {
            printf("|       | ");
            hidden -= 1;
        }
        else{
            printf(cv[0]=='1'&&cv[1]=='0'?"|%s%s    | ":"|%s%s     | ",cv,cf);
        }
    } printf("\n"); hidden = tempHidden;
    for (int i = 0; i < h->size; i++) printf("|       | "); printf("\n");
    for (int i = 0; i < h->size; i++) printf("|       | "); printf("\n");
    for (int i = 0; i < h->size; i++) printf("|       | "); printf("\n");
    for (int i = 0; i < h->size; i++) {
        setCardName(h->cards[i], cv, cf);
        if(hidden>0)
        {
            printf("|       | ");
            hidden -= 1;
        }
        else{
            printf(cv[0]=='1'&&cv[1]=='0'?"|    %s%s| ":"|     %s%s| ",cv,cf);
        }
    } printf("\n"); hidden = tempHidden;
    for (int i = 0; i < h->size; i++) printf("|_______| "); printf("\n");
    printf("\n");
}

void hitHand(Hand * h){
    int * newha = (int*) malloc( (h->size+1)*sizeof(int) );
    //Copy and append card
    for (int i = 0; i < h->size; i++) newha[i] = h->cards[i];
    newha[h->size] = getRandomCard();
    //Set new Hand
    free(h->cards);
    h->cards = newha;
    h->size += 1;
    //Update Score
    int v = getCardValue( h->cards[h->size-1] );
    h->score += v;
    h->scoreX += v;
    if(h->scoreX == h->score && v==1) h->scoreX+=10; //Ace as 11
}

void appendCard(Hand * h, int card){
    int * newha = (int*) malloc( (h->size+1)*sizeof(int) );
    //Copy and append card
    for (int i = 0; i < h->size; i++) newha[i] = h->cards[i];
    newha[h->size] = card;
    //Set new Hand
    // free(h->cards); //TODO Por??
    h->cards = newha;
    h->size += 1;
    //Update Score
    int v = getCardValue( h->cards[h->size-1] );
    h->score += v;
    h->scoreX += v;
    if(h->scoreX == h->score && v==1) h->scoreX+=10; //Ace as 11
}

/*
 Winner comparison
 1  h1>h2 h1 Wins
 0  h1=h2 TIE
 -1 h1<h2 h2 Wins
 */
int compareHands(Hand * h1, Hand * h2){
    if (h1->score>21) { //h1 over 21
        if (h2->score>21) return 0; //h2 also over 21 TIE
        return -1;                  //h2 under 21     h2 Wins
    }
    if (h2->score>21){  //h2 over 21
        if (h1->score>21) return 0; //h1 also over 21 TIE
        return 1;                   //h1 under 21     h1 Wins
    }
    
    /*
     s2 will always be the highest, if it is not over 21 then pick that one
     The winner is the lowest hScore above 0
    */
    //h1
    int s12 = 21-h1->scoreX;
    int h1Score = s12<0?21-h1->score:s12;
    //h2
    int s22 = 21-h2->scoreX;
    int h2Score = s22<0?21-h2->score:s22;
    
    //Compare scores
    if(h1Score >= 0 && h2Score <0)   return 1; //h2 over 21
    if(h1Score < 0 && h2Score >=0)   return -1;//h1 over 21
    if(h1Score < 0 && h2Score < 0)   return 0; //Both over 21
    return h1Score<h2Score?1:-1; //Compare the best one
}

#endif /* Blackjack_h */
