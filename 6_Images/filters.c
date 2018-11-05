
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "filters.h"

//rows and cols should exist before calling
void allocateMatrixMemory(f_matrix * mRes){
	mRes->vals = calloc(mRes->rows, sizeof(float*)); //Rows
    for (int c = 0 ; c < mRes->rows ; ++c )
        mRes->vals[c] = calloc(mRes->cols, sizeof(float)); //Cols
}

void freeMatrixMemory(f_matrix * mRes){
	for (int c = 0 ; c < mRes->rows ; ++c )
        free( mRes->vals[c] );
    free(mRes->vals);
}

void readMatrixFile(char * filePath, f_matrix * mMatrix){
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
    mMatrix->divisor = 0.0;
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
                sscanf( pch , "%f", &mMatrix->vals[cRow][cCol]); // matrix[row][column] = float( pch ) ; pch is the 'n' substring after spliting line by spaces
                mMatrix->divisor += mMatrix->vals[cRow][cCol];
                pch = strtok (NULL, " "); //Split by space " ,.-"
                cCol++; //Increase col index
            }
            cRow++; //Current Row
        }
    }
    
    fclose(fp);
    if (line) free(line);

    if (mMatrix->divisor==0) mMatrix->divisor = 1.0; //and negatives??
}

void printMatrix(f_matrix* mat){
    printf("%d %d\n",mat->rows,mat->cols);
    for (int i = 0; i<mat->rows; i++) {
        for (int j = 0; j<mat->cols; j++) {
            printf("%f ",mat->vals[i][j]);
        }
        printf("\n\n");
    }
}

void applyFilter(ppm_t * image,f_matrix * m){
	//Assert matrix is a square matrix with odd rows/cols
	if (m->rows != m->cols || m->rows%2==0 || m->rows <= 1)
	{
		printf("Filter must be a square matrix with odd number of rows/cols (>1)\n");
		return;
	}
	//Create a copy of the image
	ppm_t temp;
	copyImage( image, &temp);
	printf("Size %dx%d\n", image->height,image->width);

#ifdef DEBUG
    clock_t t; 
    t = clock(); 
#endif
    
    #pragma omp parallel for default(none) shared(temp, image, m) collapse(2)
	for (int i = 0; i < image->height; ++i)
		for (int j = 0; j < image->width; ++j)
			applyKernel( &temp, image, m, i,j ); 

#ifdef DEBUG
    t = clock() - t; 
    printf("It took %f seconds",((double)t)/CLOCKS_PER_SEC);
#endif

	freeMemory(&temp);
}

//Filter must be a square matrix with odd number of rows/cols
//It applies the kernel to a pixel, it writes on dest but calculates from src
void applyKernel(const ppm_t * src, ppm_t * dest, f_matrix * filter, int row, int col){
	int middle = (int) filter->rows/2; //It does a floor

	float r = 0.0, g = 0.0, b = 0.0; //It will leave black if there is no change
	//Our reference in the image
	int fixed_row = row-middle; 
	int fixed_col = col-middle;
	//For moving in image
	int ix,jx; 

	for (int i = 0; i < filter->rows; ++i)
	{
		ix = fixed_row+i;
		for (int j = 0; j < filter->cols; ++j)
		{
			jx = fixed_col+j;
			if(jx<0 || jx>=src->width || ix<0 || ix>=src->height) continue; //Skip if we are out of range
			//DO THE MAGIC
			r += src->pixels[ix][jx].data[0]*filter->vals[i][j]/filter->divisor;
			g += src->pixels[ix][jx].data[1]*filter->vals[i][j]/filter->divisor;
			b += src->pixels[ix][jx].data[2]*filter->vals[i][j]/filter->divisor;
		}
	}
	//Update dest
	dest->pixels[row][col].data[0] = (unsigned char)r; 
	dest->pixels[row][col].data[1] = (unsigned char)g; 
	dest->pixels[row][col].data[2] = (unsigned char)b; 
}







