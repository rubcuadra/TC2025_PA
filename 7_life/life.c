
#include <stdio.h>
// #include <sys/types.h>
#include <math.h>
#include <omp.h>
#include <pthread.h>
#include "life.h"
#include "matrix.h"

#define NUM_THREADS 8 //for MTgenerateLife

//Linear
void generateLife(const matrix_t * currentGeneration, matrix_t * nextGeneration){
	for (int i = 0; i < currentGeneration->rows; ++i)
		for (int j = 0; j < currentGeneration->cols; ++j)
			reproduce(currentGeneration,nextGeneration,i,j);
}

//Threads by OMP
void OMPgenerateLife(const matrix_t * currentGeneration, matrix_t * nextGeneration){
	#pragma omp parallel for default(none) shared(currentGeneration, nextGeneration) collapse(2)
	for (int i = 0; i < currentGeneration->rows; ++i)
		for (int j = 0; j < currentGeneration->cols; ++j)
			reproduce(currentGeneration,nextGeneration,i,j);
}

// void generateThreadsData(data_t * thread_data,int numThreads, matrix_t * currentGeneration){
// 	thread_data = malloc();
	
// 	//Split by area
// 	int area = currentGeneration->rows * currentGeneration->cols;
// 	int blockArea = area/NUM_THREADS;
// 	int side = floor(sqrt(blockArea)) ; //Sqr root
// 	//Prepare divisions for load balancing
// 	// int width_step  = currentGeneration->width >side?side:currentGeneration->width;
// 	// int height_step = currentGeneration->height>side?side:currentGeneration->height;
// 	int current_row = 0, current_col = 0;
// 	int row_end = 0;
// 	// Launch the threads
//     for (int i=0; i<NUM_THREADS; i++)
//     {
//         // Create an instance of the structure to pass
//         thread_data = malloc(sizeof (data_t));
//         //Set pointers for boards
//         thread_data->currentGeneration = currentGeneration;
//         thread_data->nextGeneration    = nextGeneration;
        
//         thread_data->from_row = current_row;
//         thread_data->from_col = current_col;

//         //Get TO
//         if ( current_row+side < currentGeneration->rows ){
//         	thread_data->to_row = current_row+side;
//         	current_row = thread_data->to_row;
//         }
//         else{
//         	thread_data->to_row = currentGeneration->rows; //Remaining
//         	current_row = 0;     //Return to left
//         	row_end = 1;         //For moving cols
//         }
        
//         thread_data->to_col   = current_col+side<currentGeneration->cols?current_col+side:currentGeneration->cols;
//         if (row_end == 1){
//         	current_col = thread_data->to_col; //CHECK THIS
//         	row_end = 0;
//         }
//         // printf("Thread %d, (%d,%d) (%d,%d)\n", i, thread_data->from_row,thread_data->from_col,thread_data->to_row,thread_data->to_col);
// 		//Data ready, create threads
// 		pthread_create(&tid[i], NULL, reproduceBlock, (void *)thread_data);		        	
//     }
//     //Last block
//     thread_data = malloc(sizeof (data_t));
//     //Set pointers for boards
//     thread_data->currentGeneration = currentGeneration;
//     thread_data->nextGeneration    = nextGeneration;
//     thread_data->from_row = current_row;
//     thread_data->from_col = current_col;
//     thread_data->to_row   = currentGeneration->rows;
//     thread_data->to_col   = currentGeneration->cols;
//     pthread_create(&tid[NUM_THREADS], NULL, reproduceBlock, (void *)thread_data);		        	
//     // Wait for the threads to finish
//     for (int i=0; i<=NUM_THREADS; i++) pthread_join(tid[i], NULL);  
// }

//Normal Threads
void MTgenerateLife(const matrix_t * currentGeneration, matrix_t * nextGeneration){
	
	pthread_t tid[NUM_THREADS+1];
	data_t * thread_data = NULL;
	
	//Split by area
	int area = currentGeneration->rows * currentGeneration->cols;
	int blockArea = area/NUM_THREADS;
	int side = floor(sqrt(blockArea)) ; //Sqr root
	//Prepare divisions for load balancing
	// int width_step  = currentGeneration->width >side?side:currentGeneration->width;
	// int height_step = currentGeneration->height>side?side:currentGeneration->height;
	int current_row = 0, current_col = 0;
	int row_end = 0;
	// Launch the threads
    for (int i=0; i<NUM_THREADS; i++)
    {
        // Create an instance of the structure to pass
        thread_data = malloc(sizeof (data_t));
        //Set pointers for boards
        thread_data->currentGeneration = currentGeneration;
        thread_data->nextGeneration    = nextGeneration;
        
        thread_data->from_row = current_row;
        thread_data->from_col = current_col;

        //Get TO
        if ( current_row+side < currentGeneration->rows ){
        	thread_data->to_row = current_row+side;
        	current_row = thread_data->to_row;
        }
        else{
        	thread_data->to_row = currentGeneration->rows; //Remaining
        	current_row = 0;     //Return to left
        	row_end = 1;         //For moving cols
        }
        
        thread_data->to_col   = current_col+side<currentGeneration->cols?current_col+side:currentGeneration->cols;
        if (row_end == 1){
        	current_col = thread_data->to_col; //CHECK THIS
        	row_end = 0;
        }
        // printf("Thread %d, (%d,%d) (%d,%d)\n", i, thread_data->from_row,thread_data->from_col,thread_data->to_row,thread_data->to_col);
		//Data ready, create threads
		pthread_create(&tid[i], NULL, reproduceBlock, (void *)thread_data);		        	
    }
    //Last block
    thread_data = malloc(sizeof (data_t));
    //Set pointers for boards
    thread_data->currentGeneration = currentGeneration;
    thread_data->nextGeneration    = nextGeneration;
    thread_data->from_row = current_row;
    thread_data->from_col = current_col;
    thread_data->to_row   = currentGeneration->rows;
    thread_data->to_col   = currentGeneration->cols;
    pthread_create(&tid[NUM_THREADS], NULL, reproduceBlock, (void *)thread_data);		        	
    // Wait for the threads to finish
    for (int i=0; i<=NUM_THREADS; i++) pthread_join(tid[i], NULL);    
}

//A thread runs this
void * reproduceBlock(void * arg){
	data_t * td = (data_t *) arg;
	for (int i = td->from_row; i < td->to_row; ++i)
		for (int j = td->from_col; j < td->to_col; ++j)
			reproduce(td->currentGeneration, td->nextGeneration, i,j);
    free(td);  // Release the memory used
    pthread_exit(NULL); // Finish the thread
}

void reproduce(const matrix_t * currentGeneration, matrix_t * nextGeneration, int row, int col){
	//Get Count of neighbors
	int c = 0;
	c += getCell(currentGeneration,row,col-1);   //U
	c += getCell(currentGeneration,row,col+1);   //D
	c += getCell(currentGeneration,row-1,col);   //L
	c += getCell(currentGeneration,row+1,col);   //R
	c += getCell(currentGeneration,row-1,col-1); //UL
	c += getCell(currentGeneration,row-1,col+1); //DL
	c += getCell(currentGeneration,row+1,col-1); //UR
	c += getCell(currentGeneration,row+1,col+1); //DR
	
	if( currentGeneration->vals[row][col] == ALIVE ){
		if (c == 2 || c == 3 ) //Any live cell with two or three live neighbors lives on to the next generation.
			nextGeneration->vals[row][col] = ALIVE;
		else 
			//Any live cell with more than three live neighbors dies, as if by overpopulation.
			//Any live cell with fewer than two live neighbors dies, as if by underpopulation.
			nextGeneration->vals[row][col] = DEAD;
	}
	else{
		//Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
		nextGeneration->vals[row][col] = c==3?ALIVE:DEAD;
	}
}

//Use it in border cells
int getCell(const matrix_t * gen, int i, int j){
	if (i < 0) i = gen->rows-1;
	if (i >= gen->rows)  i = 0;
	if (j < 0) j = gen->cols-1;
	if (j >= gen->cols)  j = 0;
	return gen->vals[i][j];
}