//
//  ppmReader.h
//  coursework
//
//  Created by Ruben Cuadra on 23/08/2018.
//  Copyright © 2018 CUADRA. All rights reserved.

#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RGB_COMPONENT_COLOR 255

enum IMAGE_FORMAT {P3 , P6, UNKOWN};
typedef struct{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} PPMPixel;

typedef struct{
    int x, y;
    PPMPixel *data;
} PPMImage;

static enum IMAGE_FORMAT getFormat(char *f){
    if ( f[0] == 'P' && f[1] == '3' ) return P3;
    if ( f[0] == 'P' && f[1] == '6' ) return P6;
    return UNKOWN;
}

static PPMPixel *getPixel( PPMImage *img, int i, int j){
    return &img->data[ i*img->x + j  ];
}

static void setPixel(PPMImage*img, PPMPixel *px, int i, int j){
    img->data[ i*img->x + j  ] = *px;
}

// based on https://stackoverflow.com/questions/2693631/read-ppm-file-and-store-it-in-an-array-coded-with-c
static PPMImage *readPPM(const char *filename)
{
    char buff[16];                    //Buffer
    PPMImage *img;                    //to Return
    enum IMAGE_FORMAT format = UNKOWN;//Format
    int c;                            //for temp char
    int rgb_comp_color;
    
    //open PPM file for reading
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }
    
    //read image format
    if (!fgets(buff, sizeof(buff), fp)) {
        perror(filename);
        exit(1);
    }
    
    format = getFormat( buff );
    
    //Create struct (allocate space)
    img = (PPMImage *)malloc(sizeof(PPMImage));
    if (!img) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    
    //check for comments
    c = getc(fp);
    while (c == '#') {
        while (getc(fp) != '\n') ;//Skip everything until line break
        c = getc(fp);             //Check if we have comment or vals
    }
    ungetc(c, fp);                //Return it to the stream
    
    //read and asign image size information
    if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
        fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
        exit(1);
    }
    
    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
        fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
        exit(1);
    }
    
    //check rgb component depth
    if (rgb_comp_color != RGB_COMPONENT_COLOR) {
        fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
        exit(1);
    }
    
    while (fgetc(fp) != '\n') ; //Skip until break of line
    
    //memory allocation for pixel data
    img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));
    
    if (!img) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    
    //Required for P3
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int rgbCounter = 0, pixel = 0;
    
    switch (format) {
        case P6: //fread
            if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
                fprintf(stderr, "Error loading image '%s'\n", filename);
                exit(1);
            }
            break;
        case P3:
            while ( (read = getline(&line, &len, fp)) != -1 ) {
                char * pch = strtok (line," ");    //Split by space " ,.-"
                while (pch != NULL)
                {
                    switch (rgbCounter) {
                        case 0:
                            sscanf( pch , "%d", &img->data[pixel].red);
                            break;
                        case 1:
                            sscanf( pch , "%d", &img->data[pixel].green);
                            break;
                        case 2:
                            sscanf( pch , "%d", &img->data[pixel].blue);
                            break;
                    }
                    
                    pch = strtok (NULL, " ");    //Split by space " ,.-"
                    rgbCounter++;
                    
                    if (rgbCounter == 3) {
                            pixel++;       //Current Row
                            rgbCounter = 0;
                    }
                }
            }
            //Swap the diagonal
            PPMPixel * temp = NULL;
            for (int i = 0; i < img->y; i++) {
                for (int j = i; j < img->x; j++) {
                    temp = getPixel(img, i, j);
                    setPixel( img, getPixel(img, j, i) , i, j );
                    setPixel( img, temp , j, i );
                }
            }
            break;
        default:
            fprintf(stderr, "Unkown format \n");
            exit(1);
            break;
    }
    
    fclose(fp);
    return img;
}

static void writePPM(const char *filename, PPMImage *img, enum IMAGE_FORMAT img_format)
{
    //open file for output
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }
    
    //write the header file
    switch (img_format) {
        case P6:
            fprintf(fp, "P6\n");                       // image format
            fprintf(fp, "%d %d\n",img->x,img->y);      // image size
            fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);   // rgb component depth
            fwrite(img->data, 3 * img->x, img->y, fp); // pixel data
            break;
        case P3:
            fprintf(stderr, "Format not implemented yet \n");
            exit(1);
        default:
            fprintf(stderr, "Unkown format \n");
            exit(1);
            break;
    }
    
    fclose(fp);
}


static PPMImage * scaleImage( PPMImage * img, const int factor){

    PPMImage* newImg = (PPMImage *)malloc(sizeof(PPMImage));
    if (!img) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    //Set new size and Allocate pixel data
    newImg->x = floor(img->x*(float)factor/100);
    newImg->y = floor(img->y*(float)factor/100);
    newImg->data = (PPMPixel*)malloc(newImg->x * newImg->y * sizeof(PPMPixel));
    if (!img) {
        fprintf(stderr, "Unable to allocate image memory (%d,%d)\n",newImg->x,newImg->y);
        exit(1);
    }

    
    PPMPixel * temp = NULL;             //For assigning
    PPMPixel * newPixel = NULL;
    
    if (factor>100) { //Scale up with nearest neighbor
        float x_fact = (float)newImg->x/(float)(img->x-1);
        float y_fact = (float)newImg->y/(float)(img->y-1);

        for (int c1 = 0; c1<newImg->x; c1++) {
            for (int c2 = 0; c2<newImg->y; c2++) {
                temp = getPixel(img,round(c2/y_fact), round(c1/x_fact) ); //get scaled pixel
                setPixel(newImg, temp,c2,c1);                               //set it in the new image
            }
        }
    }else{
        //Algorithm from http://www.davdata.nl/math/bmresize.html
        
        float fx = (float)(img->x)/(float)newImg->x; //Width of projection
        float fy = (float)(img->y)/(float)newImg->y; //Height of projection
        float fyStep = fy*0.9999; //Example in url, otherwise we exceed dims
        float fxStep = fx*0.9999;
        float fix = 1/fx;
        float fiy = 1/fy;
        float sx1,sy1,sx2,sy2; //Upper Left,Lower Left,Upper Right,Lower Right points of projection
        float dx,dy; //Deltas (sides of the square)
        float destR, destG,destB; //Temps for summing colors
        float AP;  //Area factor
        
        for (int y = 0; y<newImg->y; y++) {
            sy1 = fy * y;
            sy2 = sy1 + fyStep;
            for (int x = 0; x<newImg->x; x++) {
                
                sx1 = fx * x;
                sx2 = sx1 + fxStep;
                
                dx = (sx2-sx1);
                dy = (sy2-sy1);
                
                destR = 0.0;
                destG = 0.0;
                destB = 0.0;
                
                for (int i = trunc(sy1); i < trunc(sy2); i++) {
                    for (int j = trunc(sx1); j < trunc(sx2); j++) {
                        temp = getPixel(img, i, j);
//
                        AP = fix*fiy*dx*dy;
                        destR += (float)((int)(temp->red))*AP;
                        destG += (float)((int)(temp->green))*AP;
                        destB += (float)((int)(temp->blue))*AP;
                    }
                }
                
                newPixel = (PPMPixel*)malloc(sizeof(PPMPixel));
                newPixel->red = (int) destR;
                newPixel->green = (int) destG;
                newPixel->blue = (int) destB;
                setPixel(newImg, newPixel, y, x);
            }
        }
    }
    
    return newImg;
}

static void toNegative(PPMImage *img) // 255 - currentValue
{
    //faster because we already know the lineal structure
    if(img){
        for(int i=0;i<img->x*img->y;i++){
            img->data[i].red=RGB_COMPONENT_COLOR-img->data[i].red;
            img->data[i].green=RGB_COMPONENT_COLOR-img->data[i].green;
            img->data[i].blue=RGB_COMPONENT_COLOR-img->data[i].blue;
        }
    }
}

static void freeImage(PPMImage *img){
    free(img->data); //remove the image pixels from the memory
    free(img);       //remove the image as a struc
}

static void printImage( PPMImage * img){ //just for debug not useful irl
    PPMPixel *px = NULL;
    for (int i = 0; i < img->y; i++) {
        for (int j = 0; j < img->x; j++) {
            px = getPixel(img, i, j);
            printf( "%d %d %d, ", px->red,px->green,px->blue  );
        }
        putchar('\n');
    }
}
