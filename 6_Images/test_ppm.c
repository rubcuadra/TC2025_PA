/*
   Sample program to test libraries.
   Uses images in PPM format:
    http://netpbm.sourceforge.net/doc/ppm.html

    Gilberto Echeverria
    gilecheverria@yahoo.com
    31/01/2018
*/

#include <unistd.h>
#include <getopt.h>
#include "string_functions.h"
#include "ppms.h"
#include "filters.h"


void usage(char * program);
void parseOptions(int argc, char * argv[]);
void imageOperations(char * in_filename, char * out_filename, char * filter_filename, int doNegative, int rotationAngle, int newScale);


int main(int argc, char * argv [])
{
    printf("\n=== PROGRAM TO TEST PPM READING AND WRITING ===\n");

    parseOptions(argc, argv);

    return 0;
}

void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s -i {input_file} -o {output_file} -m {matrix_file} [-n] [-r {degrees}] [-s {percentage}]\n", program);
    printf("Available operations:\n");
    printf("\t-n : Compute the negative of an image\n");
    printf("\t-r : Rotate the image the specified degrees (90, 180, 270)\n");
    printf("\t-s : Scale the image up or down a percentage (50, 200)\n");
    exit(EXIT_FAILURE);
}

void parseOptions(int argc, char * argv[])
{
    int opt;
    char * in_filename;
    char * out_filename;
    char * filter_filename;
    int input_ok = 0;
    int output_ok = 0;
    int filter_ok = 0;
    int doNegative = 0;
    int rotationAngle = 0;
    int newScale = 0;

    while ( (opt = getopt(argc, argv, "i:o:m:nr:s:")) != -1 )
    {
        switch (opt)
        {
            case 'i':
                in_filename = optarg;
                input_ok = 1;
                break;
            case 'o':
                out_filename = optarg;
                output_ok = 1;
                break;
            case 'm':
                filter_filename = optarg;
                filter_ok = 1;
                break;
            case 'n':
                doNegative = 1;
                break;
            case 's':
                newScale = atoi(optarg);
                break;
            case 'r':
                rotationAngle = atoi(optarg);
                break;
            default:
                printf("Invalid option '%c'\n", opt);
                usage(argv[0]);
                break;
        }

    }

    if (input_ok && output_ok && filter_ok)
    {
        // Call the function to do image operations
        imageOperations(in_filename, out_filename, filter_filename, doNegative, rotationAngle, newScale);
    }
    else
    {
        usage(argv[0]);
    }
}

void imageOperations(char * in_filename, char * out_filename, char * filter_filename, int doNegative, int rotationAngle, int newScale)
{ 
    ppm_t input_image = {"", 0, 0, 0, NULL}; // Create the image variables and initialize to be empty
    f_matrix filter   = {0,0,NULL};          // Create the matrix variables and initialize to be empty

    // Load the image into the program
    readImageFile(in_filename, &input_image);
    printf("Read file '%s' successfully\n", in_filename);
    readMatrixFile(filter_filename, &filter);
    printf("Read file '%s' successfully\n", filter_filename);

#ifdef DEBUG
    // The file just read
    printf("THESE ARE THE PIXELS\n");
    printPixels(&input_image);
    printf("THESE IS THE FILTER MATRIX\n");
    printMatrix(&filter);
#endif /* DEBUG */

    //Always
    applyFilter(&input_image,&filter);

    //Optionals
    if (doNegative)
    {
        // Transform the image into a negative
        getNegativeImage(&input_image);
    }

    if (rotationAngle)
    {
        // Rotate the image
        rotateImage(&input_image, rotationAngle);
    }

    if (newScale)
    {
        // Scale down the image
        scaleImage(&input_image, newScale);
    }

    // Save the new image
    writeImageFile(out_filename, &input_image);
    printf("Wrote file '%s' successfully\n", out_filename);

    // Liberate the memory used for the structures
    freeMemory( &input_image );
    freeMatrixMemory( &filter );
}
