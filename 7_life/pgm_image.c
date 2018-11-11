/*
   Definition of an image data structure for PGM format images
   Uses typedef, struct
   Uses functions to read and write a file in PGM format, described here:
    http://netpbm.sourceforge.net/doc/pgm.html
    http://rosettacode.org/wiki/Bitmap/Write_a_PGM_file#C

   Gilberto Echeverria
   gilecheverria@yahoo.com
   06/10/2016
*/

#include "pgm_image.h"

//// FUNCTION DECLARATIONS

// Generate space to store the image in memory
void allocateImage(image_t * image)
{
    image->pixels = malloc(image->height * sizeof(pixel_t*)); //Rows
    for (int c = 0 ; c < image->height ; ++c )
        image->pixels[c] = malloc(image->width * sizeof(pixel_t)); //Cols
}

// Release the dynamic memory used by an image
void freeImage(image_t * image)
{
    for (int c = 0 ; c < image->height ; ++c )
        free( image->pixels[c] );
    free(image->pixels);
}

// Copy an image to another structure
void copyPGM(const pgm_t * source, pgm_t * destination)
{
    // Loop over the size of the image
    for (int i=0; i<source->image.height; i++)
    {
        for (int j=0; j<source->image.width; j++)
        {
            // Copy the pixel intensity value
            destination->image.pixels[i][j].value = source->image.pixels[i][j].value;
        }
    }
}

// Read the contents of a text file and store them in an image structure
void readPGMFile(const char * filename, pgm_t * pgm_image)
{
    FILE * file_ptr = NULL;

    printf("\nReading file: '%s'\n", filename);
    // Open the file
    file_ptr = fopen(filename, "r");
    if (file_ptr == NULL)
    {
        printf("Error: Unable to open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    // Read the header
    readPGMHeader(pgm_image, file_ptr);

    // Allocate the memory for the array of pixels
    allocateImage( &(pgm_image->image) );

    // Read the data acording to the type
    if ( !strncmp(pgm_image->magic_number, "P2", 3) )
    {
        readPGMTextData(pgm_image, file_ptr);
    }
    else if ( !strncmp(pgm_image->magic_number, "P5", 3) )
    {
        readPGMBinaryData(pgm_image, file_ptr);
    }
    else
    {
        printf("Invalid file format. Unknown type: %s\n", pgm_image->magic_number);
        exit(EXIT_FAILURE);
    }

    // Close the file
    fclose(file_ptr);
    printf("Done!\n");
}

// Read the first lines of the file and store them in the correct fields in the structure
void readPGMHeader(pgm_t * pgm_image, FILE * file_ptr)
{
    char line[LINE_SIZE];

    // Get the type of PGM file
    inputString(line, LINE_SIZE, file_ptr);
    sscanf(line, "%s", pgm_image->magic_number);

    // Read the width and height of the image
    inputString(line, LINE_SIZE, file_ptr);
    // Ignore the line if it has a comment
    if (line[0] == '#')
    {
        inputString(line, LINE_SIZE, file_ptr);
    }
    sscanf(line, "%d %d", &pgm_image->image.width, &pgm_image->image.height);

    // Read the maximum value used in the image format
    inputString(line, LINE_SIZE, file_ptr);
    sscanf(line, "%d", &pgm_image->max_value);
}

// Read data in plain text format (PGM P2)
void readPGMTextData(pgm_t * pgm_image, FILE * file_ptr)
{
    // Read the data for the pixels
    for (int i=0; i<pgm_image->image.height; i++)
    {
        for (int j=0; j<pgm_image->image.width; j++)
        {
            // Read the value for the pixel
            fscanf(file_ptr, "%hhu", &(pgm_image->image.pixels[i][j].value));
        }
    }
}

// Read data in binary format (PGM P5)
void readPGMBinaryData(pgm_t * pgm_image, FILE * file_ptr)
{
    unsigned char * data = NULL;
    int k = 0;
    // Count the number of pixels (each has 3 components, so we'll multiply by 3 later)
    size_t pixels = pgm_image->image.height * pgm_image->image.width;
    // Allocate a temporary array for all the data necessary
    data = (unsigned char *) malloc (pixels * 3 * sizeof (unsigned char));
    // Read all the data in one go
    if ( fread(data, sizeof (unsigned char), pixels * 3, file_ptr) )
    {
        // Copy the data into the data structure
        for (int i=0; i<pgm_image->image.height; i++)
        {
            for (int j=0; j<pgm_image->image.width; j++)
            {
                // Copy the data into the correct structure
                pgm_image->image.pixels[i][j].value = data[k++];
            }
        }
    }
    else
    {
        perror("Unable to read data from the PGM file\n");
        exit(EXIT_FAILURE);
    }
    // Release the temporary array
    free(data);
}

// Write the data in the image structure into a new PGM file
// Receive a pointer to the image, to avoid having to copy the data
void writePGMFile(const char * filename, const pgm_t * pgm_image)
{
    FILE * file_ptr = NULL;

    //printf("\nWriting file: '%s'\n", filename);
    // Open the file
    file_ptr = fopen(filename, "w");
    if (file_ptr == NULL)
    {
        printf("Error: Unable to open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    // Write the header for the file
    fprintf(file_ptr, "%s\n", pgm_image->magic_number);
    fprintf(file_ptr, "# %s\n", filename);
    fprintf(file_ptr, "%d %d\n", pgm_image->image.width, pgm_image->image.height);
    fprintf(file_ptr, "%d\n", pgm_image->max_value);
    // Write the data acording to the type
    if ( !strncmp(pgm_image->magic_number, "P2", 3) )
    {
        writePGMTextData(pgm_image, file_ptr);
    }
    else if ( !strncmp(pgm_image->magic_number, "P5", 3) )
    {
        writePGMBinaryData(pgm_image, file_ptr);
    }
    else
    {
        printf("Invalid file format. Unknown type: %s\n", pgm_image->magic_number);
        exit(EXIT_FAILURE);
    }
    fclose(file_ptr);
    //printf("Done!\n");
}

// Write the data in the image structure into a new PGM file
// Receive a pointer to the image, to avoid having to copy the data
void writePGMTextData(const pgm_t * pgm_image, FILE * file_ptr)
{
    // Write the pixel data
    for (int i=0; i<pgm_image->image.height; i++)
    {
        for (int j=0; j<pgm_image->image.width; j++)
        {
            fprintf(file_ptr, "%d", pgm_image->image.pixels[i][j].value);
            // Separate pixels in the same row with tabs
            if (j < pgm_image->image.width-1)
                fprintf(file_ptr, "\t");
            else
                fprintf(file_ptr, "\n");
        }
    }
}

// Write the data in the image structure into a new PGM file
// Receive a pointer to the image, to avoid having to copy the data
void writePGMBinaryData(const pgm_t * pgm_image, FILE * file_ptr)
{
    unsigned char * data = NULL;
    int k = 0;
    // Write the pixel data
    size_t pixels = pgm_image->image.height * pgm_image->image.width;
    // Create an array with the raw data for the image
    data = (unsigned char *) malloc (pixels * 3 * sizeof (unsigned char));
    // Copy the data into the new array
    for (int i=0; i<pgm_image->image.height; i++)
    {
        for (int j=0; j<pgm_image->image.width; j++)
        {
            // Copy the data into the correct structure
            data[k++] = pgm_image->image.pixels[i][j].value;
        }
    }
    // Write the binary data to the file
    fwrite(data, sizeof (unsigned char), pixels, file_ptr);
    // Release the temporary array
    free(data);
}
