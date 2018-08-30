//
//  main.c
//  1_bases
//
//  Created by Ruben Cuadra on 09/08/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

void fillSymbols(char *pointer_to_array, const int length ) {
    char ascii_ix = '0';                         //First char is always 0
    for (int i = 0; i < length-1; i++) {         //-1 because last position will be the empty char
        pointer_to_array[i] = ascii_ix;
        ascii_ix = ascii_ix=='9'?'A':ascii_ix+1; //Add +1 if we are in numbers, else jump to letters
    }
    pointer_to_array[length-1] = '\0';           //Empty char, meaning we finished
}

int getNumOfDigits(char *pointer_to_array){
    int length = 0;
    for (length = 0; pointer_to_array[length]; length++); //As long as we do not get a \0
    return length;
}
int getDecimalVal(const char c, char *pointer_to_array){
    for (int ix = 0; pointer_to_array[ix]; ix++) //Iterate over array
        if (c == pointer_to_array[ix])           //If we found the symbol
            return ix;                           //Return it
    printf("ERROR, SYMBOL NOT FOUND %d\n",c);
    return 0; //Throw error??
}

int main(int argc, char * argv[]) { //removed const
    
    //Get Args from the command line
    int opt, from_base = 1, to_base = 1;
    while ((opt = getopt (argc, argv, "i:o:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                from_base = atoi(optarg);
                break;
            case 'o':
                to_base = atoi(optarg);
                break;
        }
    }
    
    //Case same bases
    if (from_base == to_base) {
        printf("DONE Same bases\n");
        return 0;
    }
    
    printf("Total Numbers\t %d\n",argc-optind);
    printf("From base\t %d\n", from_base);
    printf("To base\t %d\n", to_base);
    
    int total_symbols = (from_base>to_base?from_base:to_base) + 1; //The max between bases +1 for empty char
    char symbols[total_symbols];                                   //Allocate memory for the symbols
    fillSymbols(symbols,total_symbols);                            //Fill the allocated memory with symbols for those bases
    printf("Symbols to use: %s\n",symbols);
    
    for (int i = optind; i < argc; i++){
        printf("%s b%d => ",argv[i], from_base); //argv[i] => Current number that we are converting
        int length = getNumOfDigits(argv[i]);

        //Convert to base 10, it is the middle point
        int num_base_ten = 0;
        for (int j = 0; argv[i][j]; j++){
            argv[i][j] = toupper(argv[i][j]);              //Convert everything to uppercases
            int val = getDecimalVal( argv[i][j], symbols );//Get the value of that symbol
            num_base_ten += val * pow(from_base,length-1-j) ;     //Convert to base 10
        }
        //printf("%d b10 =>",num_base_ten);
        //If final base == 10 then we have finished
        if (to_base == 10) {
            printf("%d  b10\n",num_base_ten);
            continue;
        }
        //Here we have the number in base ten, divide to convert to the next base
        int finalDigits = ceil(log10(num_base_ten)/log10(to_base)); //Calculate final digits with logarithms
        char result[finalDigits+1]; //+1 for the empty char
        
        int remaining = num_base_ten;
        for (int j = 0; j<finalDigits; j++) {
            int value = remaining%to_base; //Here we have the val as decimal number, convert to symbol
            remaining = remaining/to_base; //For next step
            result[finalDigits-j-1] = symbols[value]; //Save it in result array from the end to the beginning
        }
        result[finalDigits] = '\0';  //Final symbol
        printf( "%s b%d \n", result, to_base); //Print it
    }

    return 0;
}
