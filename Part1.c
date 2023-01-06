//
// Created by Cata on 2/4/2020.
//


#include <stdio.h>
#include "parser.h"
#include "simplicity.h"

int main(int argc, char* argv[]){
    struct gamestate data;
    struct parserConfig config;
    initParserConfig(&config);

    if (argc > 1) {
        config.inFile = fopen(argv[1], "r");
        if (config.inFile == NULL){
            fprintf(stderr,"Incorrect file name or file does not exist");
            return 1;
        }
    }

    parseCheck(&data, &config);

    fprintf(stdout, "Input file is valid\n");
    fprintf(stdout, "%d covered cards\n", data.numCovered);
    fprintf(stdout, "%d stock cards\n", data.numStock);
    fprintf(stdout, "%d waste cards\n", data.numWaste);


    fclose(config.inFile);
    return 0;
}


