/*
    RUBEN CUADRA 11/Nov/2018

    Simulation of Conway's Game of Life using OpenMP
    Based on the explanations at:
        https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
    Online versions:
        http://pmav.eu/stuff/javascript-game-of-life-v3.1.1/
        https://bitstorm.org/gameoflife/

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pgm_image.h"
// ADD YOUR EXTRA LIBRARIES HERE
#include "matrix.h"
#include "life.h"

#define COLOR_MULTIPLIER 255 
#define IMAGES_DIRECTORY "Images/"
#define STRING_SIZE 50
#define NUM_THREADS 8 

///// Function declarations /////
void usage(const char * program);
void lifeSimulation(int iterations, char * start_file);
void preparePGMImage(const matrix_t * board, pgm_t * board_image);
void saveAsPGM(const matrix_t * board, pgm_t * board_image, int iteration);
// ADD YOUR FUNCTION DECLARATIONS HERE

int main(int argc, char * argv[])
{
    char * start_file = "Boards/sample_4.txt";
    int iterations = 5;

    printf("\n=== GAME OF LIFE ===\n");
    printf("BY RUBEN CUADRA\n");

    // Parsing the arguments
    if (argc == 2)
    {
        iterations = atoi(argv[1]);
    }
    else if (argc == 3)
    {
        iterations = atoi(argv[1]);
        start_file = argv[2];
    }
    else // if (argc != 1)
    {
        usage(argv[0]);
        return 1;
    }

    // Run the simulation with the iterations specified
    printf("Running the simulation with file '%s' using %d iterations\n", start_file, iterations);
    lifeSimulation(iterations, start_file);

    return 0;
}

// Print usage information for the program
void usage(const char * program)
{
    printf("Usage:\n");
    printf("%s [iterations] [board file]\n", program);
}

// Main loop for the simulation
void lifeSimulation(int iterations, char * start_file)
{
    //Create Boards 
    matrix_t * currentGeneration = malloc( sizeof(matrix_t) );
    matrix_t * nextGeneration    = malloc( sizeof(matrix_t) ); 
    matrix_t * temp = NULL; //For Switching
    //Prepare image space
    pgm_t    generationPicture;
    readMatrixFile(start_file, currentGeneration);
    nextGeneration->rows = currentGeneration->rows;
    nextGeneration->cols = currentGeneration->cols;
    allocateMatrixMemory( nextGeneration);
    preparePGMImage( currentGeneration, &generationPicture );
    
    //Multithread without OMP
    #ifdef PARALLEL
        data_t * threads_data = malloc( (NUM_THREADS+1) * sizeof (data_t));;
        generateThreadsData(threads_data , NUM_THREADS, currentGeneration, nextGeneration);
    #endif

    #ifdef DEBUG
        clock_t t; 
        t = clock(); 
    #endif

    //Generate LIFE
    for (int i = 0; i < iterations; ++i)
    {
        #ifdef LINEAL
            generateLife( currentGeneration, nextGeneration);
        #endif
        #ifdef OMPT
            OMPgenerateLife( currentGeneration, nextGeneration );
        #endif
        #ifdef PARALLEL
            MTgenerateLife( currentGeneration, nextGeneration, threads_data, NUM_THREADS );
        #endif

        saveAsPGM( nextGeneration, &generationPicture, i);
        //Update gens
        temp = currentGeneration;          //For switching
        currentGeneration = nextGeneration;//Save Generation
        nextGeneration    = temp;          //nextGen will be overwritten
        
        #ifdef PARALLEL
            setGenerations(threads_data , NUM_THREADS, currentGeneration, nextGeneration);
        #endif 
    }

    #ifdef DEBUG
        t = clock() - t; 
        printf("It took %f seconds",((double)t)/CLOCKS_PER_SEC);
    #endif
    
    //Free space
    freeImage(&generationPicture.image);
    freeMatrixMemory(currentGeneration);
    freeMatrixMemory(nextGeneration);
    free(currentGeneration);
    free(nextGeneration);
    #ifdef PARALLEL
        free(threads_data);
    #endif 
}

// Get the memory necessary to store an image
void preparePGMImage(const matrix_t * board, pgm_t * board_image)
{
    //Prepare pgm wrapper for the image
    strncpy(board_image->magic_number, "P5", 3);
    board_image->max_value = 1;
    //Prepare image space
    board_image->image.width  = board->cols;
    board_image->image.height = board->rows;
    allocateImage(&board_image->image);
}

// Save the contents of the board as a PGM image
void saveAsPGM(const matrix_t * board, pgm_t * board_image, int iteration)
{
    char fileName[STRING_SIZE];
    sprintf(fileName, "%s/%d.pgm", IMAGES_DIRECTORY,iteration);

    //COPY board values to image
    for (int i=0; i<board_image->image.height; i++)
        for (int j=0; j<board_image->image.width; j++)
            board_image->image.pixels[i][j].value = board->vals[i][j]*COLOR_MULTIPLIER;
    
    //SAVE Image
    writePGMFile(fileName,board_image);
}
