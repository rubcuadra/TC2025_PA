//
//  parabolic_motion.h
//  coursework
//
//  Created by Ruben Cuadra on 13/08/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//

#ifndef parabolic_motion_h
#define parabolic_motion_h

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float read_from_user(char *message, char *input_location, int size_of_location){
    float r = 0.0;                                       //to return
    printf("%s",message);
    fgets (input_location, size_of_location, stdin);     //Read
    sscanf (input_location, "%f", &r);                   //Convert to number
    memset(&input_location[0], '\0' , size_of_location); //Clear
    return r;
}

void run_parabolic_motion(){
    int SIZE_OF_INPUT = 20;
    char temp_input[SIZE_OF_INPUT];
    float u,t,a;
    
    u = read_from_user( "Write the 'u' value: " , temp_input, SIZE_OF_INPUT);
    t = read_from_user( "Write the 't' value: " , temp_input, SIZE_OF_INPUT);
    a = read_from_user( "Write the 'a' value: " , temp_input, SIZE_OF_INPUT);
    
    float x = u*t*cos(a);
    float y = u*t*sin(a);
    //printf( "X = %f * %f * cos(%f)\n", u,t,a );
    printf( "X = %f\n", x );
    //printf( "Y = %f * %f * sin(%f)\n", u,t,a );
    printf( "Y = %f\n", y );
}


#endif /* parabolic_motion_h */
