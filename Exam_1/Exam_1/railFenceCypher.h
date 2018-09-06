//
//  railFenceCypher.h
//  Exam_1
//
//  Created by Ruben Cuadra on 06/09/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//

#ifndef railFenceCypher_h
#define railFenceCypher_h

#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct{
    int numLines;
    char **data;
} MFile;

static void printFile(MFile *mf){
    for (int i = 0; i < mf->numLines; i++)
        printf("%s\n", mf->data[i]);
}

static void freeMFile(MFile *mf){
    for (int c = 0 ; c < mf->numLines ; ++c )
        free( mf->data[c] );
    free(mf->data);
}

static void saveFile(const char * outputPath, MFile * mf){
//    printf("Saving to File: %s\n", outputPath);
    
    FILE *fp; // FILE  pointer
    fp = fopen(outputPath, "w+"); // open the file
    if (fp == NULL){ //Error opening the file
        printf("ERROR opening the file: %s\n",outputPath);
        exit(EXIT_FAILURE);
    }
    
    //Write
    for (int i = 0; i<mf->numLines; i++) {
        fprintf(fp,"%s\n",mf->data[i]);
    }
    fclose(fp);
}

/*
 Rails: 6
 input:  I_REALLY_LIKE_PUZZLES!
 output: IIS_LKE!R_ELEY_ZALPZLU
 
 both cipher and decipher go in that logic, filling outputs and leaving spaces until that rail is visited

     0                                     10                                       20
         1                               9       11                              19      21
             2                       8               12                      18
                 3               7                       13              17
                     4       6                               14      16
                         5                                       15

 Decipher:
 Iterate over array and reasigns order in output
     inputString looks like: (numbers are indexes of the original messages, obtainable by extending the phrase in N rails)
     0,10,20
             1,9,11,19,21
                         2,8,12,18
                                  3-7-13-17
                                           4,6,14,16
                                                    5,15
     output
         first iter:
             0,,,,,,,,,10,,,,,,,,,20,
         second iter:
             0,1,,,,,,,9,10,11,,,,,,,19,20,21
 Cipher:
    inputString looks like:
        0,1,2,3,4,5,6,7,8,9...21
 
    output
        first iter
            0,10,20,,,,,,,,,,,,,,,,,,,
        second iter
            0,10,20,1,9,11,19,21,,,,,,,,,,,,,,
 */
static void decipherLine(const char * input, char * output, int rails){
    if (rails<3) {
        printf("number of rails must be bigger than 2\n");
        return;
    }
    
    unsigned long size = strlen(input);
    int jump = rails+(rails-2);
    int i = 0, f, s, iter;
    
    for (int r = 0; r < rails && r < size; r++) {
        int j = r;
        
        if (r == 0 || r == rails-1){ //First and last level same jump
            do{
                output[ j ] = input[i++];
                j += jump;
            }while(j < size);
        }
        else{
            f = jump-r*2;  s = r*2;   //Jumps go f s f s f s
            iter = 0;                 //To know which one to use
            do{
                output[ j ] = input[ i++ ];
                j += iter++%2==0?f:s; //compare iter, then increase by f or s
            }while( j < size );
            
        }
    }
}

static void cipherLine(const char * input, char * output, int rails){
    if (rails<3) {
        printf("number of rails must be bigger than 2\n");
        return;
    }
    unsigned long size = strlen(input);
    printf("%c\n",input[size-1]);
    int i = 0,f,s,next,j;                 //index for output line
    int jump = rails+(rails-2);
    
    for (int r = 0; r < rails ; r++) {
        if (r == 0 || r == rails-1){ //First and last level increment in jump
            for (int j = 0; r+j < size; j+=jump)
                {
                    output[i++] = input[r+j];
                    if (i >= size) return;
                }
        }
        else{                         //Else they jump in f s f s f s
            f = jump-r*2;
            s = r*2;
            next = 0;
            j = 0;
            while (r+j < size)
            {
                output[i++] = input[r+j];
                next = next==0?f:(next==f?s:f); //If it has not been set put f, else invert it between f and s
                j += next;
                
                if (i >= size) return;
            }
            
        }
    }
}

static void cipherFile(MFile * f, int num_rails){
    unsigned long s;
    
    for (int i = 0; i < f->numLines; i++) {
        s = strlen( f->data[i] );
        char res[s];
        strncpy(res, f->data[i], s);
        cipherLine( res, f->data[i], num_rails);
    }
}

MFile * getFile(const char * filePath){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int c = 0;
    
    fp = fopen(filePath, "r");
    if (fp == NULL){
        printf("Unable to open file %s\n",filePath);
        exit(EXIT_FAILURE);
    }
    
    MFile *f = (MFile *)malloc(sizeof(MFile));
    while ((read = getline(&line, &len, fp)) != -1) c++;
    f->data     = (char**) malloc(c * sizeof(char *)); //Assign space in memory for each line
    if (!f->data){
        printf("Unable to allocate memory\n");
        exit(EXIT_FAILURE);
    }
    f->numLines = c;
    
    rewind(fp);
    c=0;
    
    while ((read = getline(&line, &len, fp)) != -1){
        
        if (line[read-1] == '\n'){ //If break line, remove it and update size
            line[read-1] = '\0';
            read--;
        }
        
        f->data[c] = (char*) malloc( (read+1)*sizeof(char) ); //Allocate for encrypted
        strncpy(f->data[c],line,read);                        //Copy the original message
        f->data[c][read] = '\0';
        
        if (!f->data[c]){
            printf("Unable to allocate memory\n");
            exit(EXIT_FAILURE);
        }
        c++;
    }
    
    fclose(fp);
    if (line) free(line);
    return f;
}

void testLines(){
    int num_rails = 6;
    char *deciphered = "I_REALLY_LIKE_PUZZLES!";
    char *ciphered   = "IIS_LKE!R_ELEY_ZALPZLU";
    unsigned long s = strlen(deciphered);
    char res[s];
    //strncpy(res, deciphered , s);
    
    //Decipher
    decipherLine(ciphered, res, num_rails);
    assert( strcmp(deciphered, res)==0 );
    //Cipher
    cipherLine(  deciphered, res, num_rails);
    assert( strcmp(ciphered, res)==0 );
}

#endif /* railFenceCypher_h */
