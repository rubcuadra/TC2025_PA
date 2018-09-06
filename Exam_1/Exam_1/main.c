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

    const char inputF[] = "/Users/rubcuadra/Desktop/encoded_aladdin.txt";
    const char outputF[] = "/Users/rubcuadra/Desktop/exampleEncr.txt";
    int num_rails = 5;
    MFile * cFile = getFile(inputF);

    decipherFile(cFile, num_rails);
//    cipherFile(cFile, num_rails);

    saveFile(outputF,cFile);

    freeMFile(cFile);

    return 0;
}
