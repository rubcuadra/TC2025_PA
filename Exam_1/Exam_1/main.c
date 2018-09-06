//
//  main.c
//  Exam_1
//
//  Created by Ruben Cuadra on 06/09/2018.
//  Copyright © 2018 CUADRA. All rights reserved.
//

#include <stdio.h>
#include "railFenceCypher.h"

int main(int argc, const char * argv[]) {

    const char inputF[] = "/Users/rubcuadra/Desktop/example.txt";
    const char outputF[] = "/Users/rubcuadra/Desktop/exampleEncr.txt";
    int num_rails = 4;
    MFile * cFile = getFile(inputF);
//    cipherFile(cFile, num_rails);
    printFile(cFile);
    saveFile(outputF,cFile);

    freeMFile(cFile);

//    testLines();
    
    return 0;
}
