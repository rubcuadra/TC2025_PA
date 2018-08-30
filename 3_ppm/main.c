//
//  main.c
//  ppm
//
//  Created by Ruben Cuadra on 13/08/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//
#include <getopt.h>
#include "ppmReader.h"

typedef enum { false, true } bool;

int main(int argc, char * argv[]) {
    
    char *inputFile = NULL;
    char *outputFile = NULL;
    int scale = 100; //remain
    bool negative = false;
    int opt;
    while ((opt = getopt (argc, argv, "i:o:n:s:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                inputFile = optarg;
                break;
            case 'o':
                outputFile = optarg;
                break;
            case 'n':
                negative = true;
                break;
            case 's':
                sscanf( optarg , "%d", &scale);
                break;
        }
    }
    
    printf("Inputfile %s\n",inputFile);
    printf("Outputfile %s\n",outputFile);
    printf("Scale factor %d\n",scale);
    printf("Negative: %s\n", negative?"YES\0":"NO\0" );
    
    PPMImage *img = readPPM(inputFile);
    if (negative) toNegative(img); 
    if (scale!=100) //Scale it
    {
        PPMImage *scaled = scaleImage(img,scale);
        writePPM(outputFile,scaled,P6);
        freeImage(scaled);
    }
    else{
        writePPM(outputFile,img,P6);  //It shoud be negative or equal
        freeImage(img);
    }
   
    return 0; 
}