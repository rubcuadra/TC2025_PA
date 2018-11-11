#ifndef MATRIX_H
#define MATRIX_H

typedef struct matrix{
    int rows;
    int cols;
    int **vals;
} matrix_t;

void allocateMatrixMemory(matrix_t * m); // Get the memory necessary to store a matrix
void freeMatrixMemory(matrix_t * m); // Release the memory for matrix
void readMatrixFile(char * filename, matrix_t * m); // Open an image file and copy its contents to the structure
void printMatrix(matrix_t * m);

#endif  /* NOT MATRIX_H */
