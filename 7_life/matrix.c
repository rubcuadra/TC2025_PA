
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "matrix.h"

//rows and cols should exist before calling
void allocateMatrixMemory(matrix_t * mRes){
	mRes->vals = calloc(mRes->rows, sizeof(int*)); //Rows
    for (int c = 0 ; c < mRes->rows ; ++c )
        mRes->vals[c] = calloc(mRes->cols, sizeof(int)); //Cols
}

void freeMatrixMemory(matrix_t * mRes){
	for (int c = 0 ; c < mRes->rows ; ++c )
        free( mRes->vals[c] );
    free(mRes->vals);
}

void readMatrixFile(char * filePath, matrix_t * mMatrix){
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    fp = fopen(filePath, "r");
    if (fp == NULL){ //Error opening the file
        printf("Could not open file %s\n",filePath);
        exit(EXIT_FAILURE);
    }
    
    int firstLine = 1; //Flag for reading dimensions
    int cRow = 0, cCol = 0;      //Iterators for filling the matrix

    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Line Length %zu :\n", read);
        if (firstLine) {
            char * pch;                 //Pointer to substring
            pch = strtok (line," ");    //Split by space " ,.-"
            int i = 0;                  //Control iter over words
            while (pch != NULL)
            {
                if (i == 0)      sscanf( pch , "%d", &mMatrix->rows); //First argument is rows
                else if(i == 1 ) sscanf( pch , "%d", &mMatrix->cols); //Second argument is cols
                else             break;                              //We only work with matrix of 2 dimensions
                
                pch = strtok (NULL, " "); //Split by space " ,.-"
                i++; //Increase iter
            }
            firstLine = 0; //We know the dimensions, time to create and fill the matrix
            
            allocateMatrixMemory( mMatrix ); //Required for the next lines
        }
        else{
            char * pch;                 //Pointer to substring
            pch = strtok (line," ");    //Split by space " ,.-"

            cCol = 0;                      //Iterating between cols
            while (pch != NULL)
            {
                sscanf( pch , "%d", &mMatrix->vals[cRow][cCol]); // matrix[row][column] = float( pch ) ; pch is the 'n' substring after spliting line by spaces
                pch = strtok (NULL, " "); //Split by space " ,.-"
                cCol++; //Increase col index
            }
            cRow++; //Current Row
        }
    }
    
    fclose(fp);
    if (line) free(line);
}

void printMatrix(matrix_t* mat){
    printf("%d %d\n",mat->rows,mat->cols);
    for (int i = 0; i<mat->rows; i++) {
        for (int j = 0; j<mat->cols; j++) {
            printf("%d ",mat->vals[i][j]);
        }
        printf("\n\n");
    }
}
