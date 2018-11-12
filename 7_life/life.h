#ifndef LIFE_H
#define LIFE_H

#include "matrix.h"
typedef enum life_status { DEAD = 0, ALIVE = 1 } life_s;

// Structure for the data that will be sent to each thread in MTgenerateLife
typedef struct data_struct {
    matrix_t * currentGeneration;
    matrix_t * nextGeneration;
    int from_row,to_row;
    int from_col,to_col;
} data_t;

void reproduce(const matrix_t * currentGeneration, matrix_t * nextGeneration, int row, int col);
void generateLife(const matrix_t * currentGeneration, matrix_t * nextGeneration);
void OMPgenerateLife(const matrix_t * currentGeneration, matrix_t * nextGeneration);//OMP multithreaded
void MTgenerateLife(const matrix_t * currentGeneration, matrix_t * nextGeneration, data_t * thread_data, int numThreads);
void * reproduceBlock(void * arg);//To be run by a thread
int getCell(const matrix_t * gen, int i, int j);

void setGenerations(data_t * thread_data,int numThreads, matrix_t * currentGeneration,matrix_t * nextGeneration);
void generateThreadsData(data_t * thread_data,int numThreads, matrix_t * currentGeneration,matrix_t * nextGeneration);
#endif  /* NOT LIFE_H */

