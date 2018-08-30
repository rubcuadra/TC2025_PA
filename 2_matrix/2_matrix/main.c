//
//  main.c
//  2_matrix
//
//  Created by Ruben Cuadra on 16/08/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

typedef enum { false, true } bool;
typedef enum { SUCCESS, ERROR_READING_FILE, UNABLE_TO_MULT, ERROR_CREATING_FILE } status_code;

typedef struct Matrix{
    int rows;
    int cols;
    float **vals;
} Matrix;

static void allocateMatrixMemory( Matrix * mRes ){ //calloc sets everything to 0
    mRes->vals = calloc(mRes->rows, sizeof(float*)); //Rows
    for (int c = 0 ; c < mRes->rows ; ++c )
        mRes->vals[c] = calloc(mRes->cols, sizeof(float)); //Cols
    //mRes->vals[rows][cols]
}

static void freeMatrixMemory(Matrix *mRes){ //Opposite of allocateMatrixMemory
    for (int c = 0 ; c < mRes->rows ; ++c )
        free( mRes->vals[c] );
    free(mRes->vals);
}

Matrix mult(Matrix *A, Matrix *B, status_code *code) {
    Matrix mRes;
    mRes.rows = 0;
    mRes.cols = 0;
    mRes.vals = NULL;
    
    if(A->cols != B->rows) //Check if it is possible
    {
        *code = UNABLE_TO_MULT;
        printf("UNABLE TO MULTIPLY SHAPES (%d,%d) x (%d,%d)\n",A->rows,A->cols,B->rows,B->cols);
        return mRes;
    }
    
    //Update dimensions and allocate memory
    mRes.rows = A->rows;
    mRes.cols = B->cols;
    allocateMatrixMemory( &mRes );
    
    for (int i = 0; i<mRes.rows; i++){
        for (int j = 0; j<mRes.cols; j++){
            for (int k = 0; k < A->cols; k++){ //A->cols == B->rows
                mRes.vals[i][j] += A->vals[i][k] * B->vals[k][j];
            }
        }
    }
    
    return mRes;
}

static struct Matrix read_Matrix(const char *filePath, status_code *code) {
    Matrix mMatrix;
    mMatrix.rows = 0;
    mMatrix.cols = 0;
    mMatrix.vals = NULL;
//    mMatrix.someFunction = &someFn;
    
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    fp = fopen(filePath, "r");
    if (fp == NULL){ //Error opening the file
        *code = ERROR_READING_FILE;
        return mMatrix;
    }
    
    bool firstLine = true; //Flag for reading dimensions
    int cRow = 0, cCol = 0;      //Iterators for filling the matrix
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Line Length %zu :\n", read);
        if (firstLine) {
            char * pch;                 //Pointer to substring
            pch = strtok (line," ");    //Split by space " ,.-"
            int i = 0;                  //Control iter over words
            while (pch != NULL)
            {
                if (i == 0)      sscanf( pch , "%d", &mMatrix.rows); //First argument is rows
                else if(i == 1 ) sscanf( pch , "%d", &mMatrix.cols); //Second argument is cols
                else             break;                              //We only work with matrix of 2 dimensions
                
                pch = strtok (NULL, " "); //Split by space " ,.-"
                i++; //Increase iter
            }
            firstLine = false; //We know the dimensions, time to create and fill the matrix
            
            allocateMatrixMemory( &mMatrix ); //Required for the next lines
        }
        else{
            char * pch;                 //Pointer to substring
            pch = strtok (line," ");    //Split by space " ,.-"

            cCol = 0;                      //Iterating between cols
            while (pch != NULL)
            {
                sscanf( pch , "%f", &mMatrix.vals[cRow][cCol]); // matrix[row][column] = float( pch ) ; pch is the 'n' substring after spliting line by spaces
                
                pch = strtok (NULL, " "); //Split by space " ,.-"
                cCol++; //Increase col index
            }
            cRow++; //Current Row
        }
    }
    
    fclose(fp);
    if (line) free(line);
    
    return mMatrix;
}

static void printMatrix(Matrix* mat){
    printf("%d %d\n",mat->rows,mat->cols);
    for (int i = 0; i<mat->rows; i++) {
        for (int j = 0; j<mat->cols; j++) {
            printf("%f ",mat->vals[i][j]);
        }
        printf("\n\n");
    }
}

static void saveToFile(Matrix* mat,const char *filePath, status_code *code){
    printf("Saving to File: %s\n", filePath);
    
    FILE *fp; // create a FILE typed pointer
    fp = fopen(filePath, "w"); // open the file "name_of_file.txt" for writing
    if (fp == NULL){ //Error opening the file
        *code = ERROR_CREATING_FILE;
        return;
    }
    
    //Write
    fprintf(fp,"%d %d\n",mat->rows,mat->cols);
    for (int i = 0; i<mat->rows; i++) {
        for (int j = 0; j<mat->cols; j++) {
            fprintf(fp,"%f ",mat->vals[i][j]);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}

//First Arg is matrix A
//Second Arg is matrix B
// ./a.out -o my_result_matrix.txt matrix_a.txt matrix_b.txt
int main(int argc, char * argv[]) {
    
    char *outputFile = NULL;
    
    //Get Args from the command line
    int opt;
    while ((opt = getopt (argc, argv, "o:")) != -1)
    {
        switch (opt)
        {
            case 'o':
                outputFile = optarg;
                break;
        }
    }
    
    const char *input1 = argv[optind];   //Read filepath for first matrix
    const char *input2 = argv[optind+1]; //Read filepath for second matrix
    
    printf("Matrix A: %s\n", input1);
    printf("Matrix B: %s\n", input2);
    
    status_code code = SUCCESS;
    
    struct Matrix matrixA = read_Matrix(input1, &code);
    if ( code != SUCCESS) return EXIT_FAILURE;
    printMatrix(&matrixA);
    
    struct Matrix matrixB = read_Matrix(input2, &code);
    if ( code != SUCCESS) return EXIT_FAILURE;
    printMatrix(&matrixB);

    struct Matrix matrixResult = mult( &matrixA, &matrixB , &code);
    if ( code != SUCCESS) return EXIT_FAILURE;
    printMatrix(&matrixResult);

    saveToFile( &matrixResult, outputFile, &code );
    if ( code != SUCCESS) return EXIT_FAILURE;
    
//    struct Matrix mRead = read_Matrix(outputFile, &code); //This one should be equal to matrixResult
//    if ( code != SUCCESS) return EXIT_FAILURE;
//    printMatrix(&mRead);
//    freeMatrixMemory(&mRead);
    freeMatrixMemory(&matrixA);
    freeMatrixMemory(&matrixB);
    freeMatrixMemory(&matrixResult);
    
    return EXIT_SUCCESS;
}
