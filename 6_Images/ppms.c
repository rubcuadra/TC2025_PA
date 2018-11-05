#include "ppms.h"

///// FUNCTION DEFINITIONS

//dest is a just declared ppm_t
void copyImage(ppm_t *src, ppm_t *dest){
    dest->width     = src->width;
    dest->height    = src->height;
    dest->max_value = src->max_value;
    dest->pixels    = NULL; 
    strncpy ( dest->magic_number, src->magic_number, 3 ); 
    allocateMemory(dest);

    for (int i = 0; i < src->height; ++i)
    {
        for (int j = 0; j < src->width; ++j)
        {
            dest->pixels[i][j].data[0] = src->pixels[i][j].data[0];
            dest->pixels[i][j].data[1] = src->pixels[i][j].data[1];
            dest->pixels[i][j].data[2] = src->pixels[i][j].data[2];
        }
    }
}

// Get the memory necessary to store an image of the size indicated in the structure
void allocateMemory(ppm_t * image)
{
    // Allocate the memory for INDEX array
    image->pixels = malloc (image->height * sizeof(pixel_t *));
    // Allocate the memory for all the DATA array
    image->pixels[0] = calloc (image->height * image->width, sizeof(pixel_t));
    
    // Add the rest of the pointers to the INDEX array
    for (int i=1; i<image->height; i++)
    {
        // Add an offset from the beginning of the DATA array
        image->pixels[i] = image->pixels[0] + image->width * i;
    }
}

// Release the memory for the image
void freeMemory(ppm_t * image)
{
    // Free the DATA array
    free (image->pixels[0]);
    // Free the INDEX array
    free (image->pixels);

    // Set the values for an empty image
    image->width = 0;
    image->height = 0;
    image->pixels = NULL;
}

// Open an image file and copy its contents to the structure
void readImageFile(char * filename, ppm_t * image)
{
    FILE * file_ptr = NULL;
    
    file_ptr = fopen (filename, "r");
    if (!file_ptr)
    {
        printf("Unable to open the file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    
    // Get the data from the header
    fscanf (file_ptr, "%s", image->magic_number);
    fscanf (file_ptr, "%d", &image->width);
    fscanf (file_ptr, "%d", &image->height);
    fscanf (file_ptr, "%d", &image->max_value);
    // Get rid of the '\n' after max value and before the real data
    fgetc(file_ptr);
    
    // Get the memory for the image data
    allocateMemory(image);
    
    if (!strncmp(image->magic_number, "P3", 3))
    {
        getAsciiPixels(image, file_ptr);
    }
    else if (!strncmp(image->magic_number, "P6", 3))
    {
        getBinaryPixels(image, file_ptr);
    }
    else
    {
        printf("Not a correct PPM format");
    }

    fclose(file_ptr);
}

// Write the data stored in a structure into an image file
void writeImageFile(char * filename, const ppm_t * image)
{
    FILE * file_ptr = NULL;
    
    file_ptr = fopen (filename, "w");
    if (!file_ptr)
    {
        printf("Unable to open the file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    
    // Insert the header
    fprintf (file_ptr, "%s\n", image->magic_number);
    fprintf (file_ptr, "%d ", image->width);
    fprintf (file_ptr, "%d\n", image->height);
    fprintf (file_ptr, "%d\n", image->max_value);

    // Write the matrix of data for the pixels
    if (!strncmp(image->magic_number, "P3", 3))
    {
        writeAsciiPixels(image, file_ptr);
    }
    else if (!strncmp(image->magic_number, "P6", 3))
    {
        writeBinaryPixels(image, file_ptr);
    }
    else
    {
        printf("Not a correct PPM format");
    }

    fclose(file_ptr);
}

// Read each of the numerical values for the color components
//  and store them in the arrray for the pixels
void getAsciiPixels(ppm_t * image, FILE * file_ptr)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            fscanf (file_ptr, "%hhu", &image->pixels[r][c].data[R]);
            fscanf (file_ptr, "%hhu", &image->pixels[r][c].data[G]);
            fscanf (file_ptr, "%hhu", &image->pixels[r][c].data[B]);
            //fscanf (file_ptr, "%hu", &image->pixels[r][c].data[B]);
        }
    }
}

// Copy the bytes from the image into the data array for the pixels
void getBinaryPixels(ppm_t * image, FILE * file_ptr)
{
    fread (image->pixels[0], sizeof(pixel_t), image->width * image->height, file_ptr);    
}

// Write the numerical values for the color components into
//  the output file, giving them a nice format
void writeAsciiPixels(const ppm_t * image, FILE * file_ptr)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            fprintf (file_ptr, "%3hhu ", image->pixels[r][c].data[R]);
            fprintf (file_ptr, "%3hhu ", image->pixels[r][c].data[G]);
            fprintf (file_ptr, "%3hhu\t", image->pixels[r][c].data[B]);
        }
        fprintf(file_ptr, "\n");
    }
}

// Copy the pixel data into the file as bytes
void writeBinaryPixels(const ppm_t * image, FILE * file_ptr)
{
    fwrite (image->pixels[0], sizeof(pixel_t), image->width * image->height, file_ptr);    
}

// Compute the negative colors of an image
void getNegativeImage(ppm_t * image)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                image->pixels[r][c].data[color] = image->max_value - image->pixels[r][c].data[color];
            }
        }
    }
}

// Print the pixels of the image as hexadecimal on the console
void printPixels(ppm_t * image)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                printf("%02x ", image->pixels[r][c].data[color]);
            }
            printf("\t");
        }
        printf("\n");
    }
}


// Rotate the image a specified number of degrees
// Modify the structure received as a parameter
void rotateImage(ppm_t * source, int angle)
{
    // Local variable for an image structure
    ppm_t destination = {"", 0, 0, 0, NULL};

    // Copy the header data
    strncpy (destination.magic_number, source->magic_number, 3);
    destination.max_value = source->max_value;

    switch (angle)
    {
        case 90:
            // Invert the sizes
            destination.height = source->width;
            destination.width = source->height;
            // Get the memory for the image data
            allocateMemory(&destination);
            // Apply the rotation
            rotateImage90(&destination, source);
            break;
        case 180:
            // Keep the same image ration
            destination.height = source->height;
            destination.width = source->width;
            // Get the memory for the image data
            allocateMemory(&destination);
            // Apply the rotation
            rotateImage180(&destination, source);
            break;
        case 270:
            // Invert the sizes
            destination.height = source->width;
            destination.width = source->height;
            // Get the memory for the image data
            allocateMemory(&destination);
            // Apply the rotation
            rotateImage270(&destination, source);
            break;
        default:
            printf("Can only rotate the image 90, 180 or 270 degrees. Sorry!\n");
            break;
    }

    // Free the previous memory data
    freeMemory(source);
    // Copy the results back to the pointer received
    *source = destination;
}

// Rotate the image and write the result in another structure
void rotateImage90(ppm_t * destination, const ppm_t * source)
{
    // Prepare the limits for the loops
    int max_row = destination->height - 1;

    // Loops to reallocate the pixels
    for (int row_src=0, col_dst=0; row_src<source->height; row_src++, col_dst++)
    {
        for (int col_src=0, row_dst=max_row; col_src<source->width; col_src++, row_dst--)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = source->pixels[row_src][col_src].data[color];
            }
        }
    }
}

// Rotate the image and write the result in another structure
void rotateImage180(ppm_t * destination, const ppm_t * source)
{
    // Prepare the limits for the loops
    int max_row = destination->height - 1;
    int max_col = destination->width - 1;

    // Loops to reallocate the pixels
    for (int row_src=0, row_dst=max_row; row_src<source->height; row_src++, row_dst--)
    {
        for (int col_src=0, col_dst=max_col; col_src<source->width; col_src++, col_dst--)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = source->pixels[row_src][col_src].data[color];
            }
        }
    }
}

// Rotate the image and write the result in another structure
void rotateImage270(ppm_t * destination, const ppm_t * source)
{
    // Prepare the limits for the loops
    int max_col = destination->width - 1;

    // Loops to reallocate the pixels
    for (int row_src=0, col_dst=max_col; row_src<source->height; row_src++, col_dst--)
    {
        for (int col_src=0, row_dst=0; col_src<source->width; col_src++, row_dst++)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = source->pixels[row_src][col_src].data[color];
            }
        }
    }
}

// Scale an image by a given percentage
// Modify the structure received as a parameter
void scaleImage(ppm_t * source, int scale)
{
    // Local variable for an image structure
    ppm_t destination = {"", 0, 0, 0, NULL};

    // Copy the header data
    strncpy (destination.magic_number, source->magic_number, 3);
    destination.max_value = source->max_value;

    // Keep the same image ratio
    destination.height = source->height * (scale / 100.0);
    destination.width = source->width * (scale / 100.0);

    printf("New width: %d, height: %d\n", destination.width, destination.height);
    // Get the memory for the image data
    allocateMemory(&destination);
    // Apply the scale
    resizeImage(&destination, source, scale);
   
    // Free the previous memory data
    freeMemory(source);
    // Copy the results back to the pointer received
    *source = destination;
}

// Apply the scale to the pixels in an image
void resizeImage(ppm_t * destination, const ppm_t * source, int scale)
{
    // Compute the multiplication factors depending on the relative sizes
    double factor_x = source->width / (double) destination->width;
    double factor_y = source->height / (double) destination->height;
    int sum[3];
    // The sum of the pixels will be multiplied by this constant to get the average
    double divisor = 1 / (factor_x * factor_y);

    // Average the colors only when scaling down
    if (divisor > 1)
    {
        divisor = 1;
    }

#ifdef DEBUG
    printf("Factor_X: %lf | Factor_Y: %lf | Multiplied: %lf\n", factor_x, factor_y, factor_x * factor_y);
    printf("1 / Factor_X: %lf | Factor_Y: %lf | Multiplied: %lf\n", 1/factor_x, 1/factor_y, 1/(factor_x * factor_y));
#endif

    // Loops to fill in the pixels in the resized image
    for (int row_dst=0; row_dst<destination->height; row_dst++)
    {
        for (int col_dst=0; col_dst<destination->width; col_dst++)
        {
#ifdef DEBUG
            printf("DEBUG: Generating pixel %d, %d\n", row_dst, col_dst);
#endif
            // Reset the values for the new pixel
            for (int color=0; color<3; color++)
            {
                sum[color] = 0;
            }
#ifdef DEBUG
            printf("DEBUG: \tCollapsing pixel rows %lf, %lf\n", row_dst*factor_y, (row_dst+1)*factor_y);
#endif
            // Loops to average the pixels in the original image
            for (int row_src=row_dst*factor_y; row_src<(row_dst+1)*factor_y; row_src++)
            {
#ifdef DEBUG
            printf("DEBUG: \tCollapsing pixel columns %lf, %lf\n", col_dst*factor_x, (col_dst+1)*factor_x);
#endif
                for (int col_src=col_dst*factor_x; col_src<(col_dst+1)*factor_x; col_src++)
                {
#ifdef DEBUG
            printf("DEBUG: \tCollapsing source pixel %d, %d\n", row_src, col_src);
#endif
                    // Loop for the 3 color components
                    for (int color=0; color<3; color++)
                    {
                        sum[color] += source->pixels[row_src][col_src].data[color];
                    }
                }
            }

#ifdef DEBUG
            printf("DEBUG: \tsum: %d, %d, %d\n", sum[0], sum[1], sum[2]);
#endif
            // Assign the new color to the destination pixel
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = sum[color] * divisor;
            }

        }
    }
}
