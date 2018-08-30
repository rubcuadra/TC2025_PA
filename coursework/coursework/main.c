//
//  main.c
//  coursework
//
//  Created by Ruben Cuadra on 13/08/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//


#include <getopt.h>

//#include "parabolic_motion.h"
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
                sscanf(  optarg , "%d", &scale);
                break;
        }
    }
    
    printf("Inputfile %s\n",inputFile);
    printf("Outputfile %s\n",outputFile);
    printf("Scale factor %d\n",scale);
    printf("Negative: %s\n", negative?"YES\0":"NO\0" );
    
    PPMImage *img = readPPM(inputFile);
//    toNegative(img);
//    PPMImage *scaled = scaleImage(img,factor);
//    writePPM(output,scaled,P6);
//    freeImage(scaled);
    freeImage(img);
    
    return 0;
    
}
