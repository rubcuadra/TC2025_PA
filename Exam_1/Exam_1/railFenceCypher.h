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
typedef enum { false, true } bool;
typedef struct{
    int numLines;
    char **data;
} MFile;

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

static void cypherLine(const char * input, char * output, int rails){
    if (rails<3) {
        printf("number of rails must be bigger than 2\n");
        return;
    }
    unsigned long size = strlen(input);
    int i = 0,f,s,next,j;                 //index for output line
    int jump = rails+(rails-2);
    
    for (int r = 0; r < rails ; r++) {
        if (r == 0 || r == rails-1){ //First and last level increment in jump
            for (int j = 0; r+j < size; j+=jump)
                output[i++] = input[r+j];
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
            }
            
        }
    }
}

MFile * getEncryptedFile(const char * filePath, int rails){
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
        if (line[read-1] == '\n') line[read-1] = '\0';        //Remove \n
        f->data[c] = (char*) malloc( (read+1)*sizeof(char) ); //Allocate for encrypted
        strncpy(f->data[c],line,read);                        //Copy the original message
        f->data[c][read+1] = '\0';                            //Extra space for the \0
        if (!f->data[c]){
            printf("Unable to allocate memory\n");
            exit(EXIT_FAILURE);
        }
        cypherLine( line, f->data[c], rails);
        c++;
    }
    
    fclose(fp);
    if (line) free(line);
    return f;
}

#endif /* railFenceCypher_h */
