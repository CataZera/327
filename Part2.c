//
// Created by Cata on 3/7/2020.
//

#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "simplicity.h"
#include "thiccHelper.h"

//#define DEBUG_PRINT

int main(int argc, char **argv){
    struct gamestate data;
    struct parserConfig config;
    initParserConfig(&config);

    unsigned i;
    for (i = 1; i < argc; i++) { //Magnus' update: check for valid switches. Not that I doubt our instructor for a second.
        if (!strcmp(argv[i], "-m")) sscanf(argv[i+1], "%d", &(config.maxMovesToRead)); //changed these back to somewhat of what they were before
        else if (!strcmp (argv[i], "-o")) config.outFile = fopen(argv[i+1], "w"); //they shouldn't return if there is more it should just
        else if (!strcmp (argv[i], "-x")) config.isExchange = 1; //stay with the value they were given. review the document and you will see it
        else
            if (strcmp(argv[i-1],"-o") && strcmp(argv[i-1],"-m")) //if we are not one of the switches above but we do follow -o or -m then we are not the input file
                {config.inFile=fopen(argv[i],"r");}
        //if (strstr(argv[i], ".txt") != NULL && strcmp(argv[i-1], "-o")) data.inFile = fopen(argv[i], "r"); //Magnus' update: technically, the input filename need not be a .txt
        //Technically the input doesn't have to end in .txt true but for the test of this program it would have sufficed. Maybe for a broader not in class project we wouldn't check for .txt -Cody
    }

    if (config.inFile == NULL) { //Magnus' updates: originally, in and outfile were uninitialized so it was anyone's guess if it was actually null when we got here. Also, easier to just set it to stdin and know that it is an error when you get to here.
        fprintf(stderr, "Error reading from the input file. It probably does not exist\n");
        return 1;
    }
    if (config.outFile == NULL) {
        fprintf(stderr, "Error creating the output file. You probably don't have write permissions, try using 'sudo ./advance' or stop messing around with the administrator's stuff\n");
        return 1;
    }

    parseAdvance(&data, &config);

    fprintf(stdout,"Processed %u %s\n", config.currentMove-1, (config.currentMove == 2)? "move, which was valid":"moves, all valid");
    printConfiguration(&data, &config);

    #ifdef DEBUG_PRINT
        printf("Num covered: %d\nNum stock: %d\nNum waste: %d\n",data.numCovered, data.numStock, data.numWaste);
    #endif

    fclose(config.inFile);
    fclose(config.outFile);
    return 0;
}
