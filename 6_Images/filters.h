#ifndef FILTERS_H
#define FILTERS_H

#include "ppms.h"

typedef struct filter_matrix{
    int rows;
    int cols;
    float **vals;
} f_matrix;

void allocateMatrixMemory(f_matrix * m); // Get the memory necessary to store a matrix
void freeMatrixMemory(f_matrix * m); // Release the memory for matrix
void readMatrixFile(char * filename, f_matrix * m); // Open an image file and copy its contents to the structure
void printMatrix(f_matrix * m);
void applyFilter(ppm_t * image,f_matrix * m);
void applyKernel(const ppm_t * src, ppm_t * dest, f_matrix * filter, int row, int col);
#endif  /* NOT FILTERS_H */
