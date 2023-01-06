#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplicity.h"

/*
I don't think any of these are actually necessarry.
I did find a bug by not using it (it was only showing itself when I left it out, but it theoretically could have happened before too), so I think its best to keep it out.
void initGamestate(struct gamestate *data) {
    //data->numCovered = 0;
    //data->numStock = 0;
    //data->numWaste = 0;

    //data->turn = 0;
    //data->limit = 0;

    unsigned i;
    //for (i = 0; i < 54; i++) data->wasteStock[i] = 0;
    //for (i = 0; i < 4; i++) data->foundations[i] = 0;
    //for (i = 0; i < 147; i++) data->tableau[i] = 0;

}*/

void initParserConfig(struct parserConfig *config) {
    config->inFile =  stdin;
    config->outFile = stdout;

    config->currentLine = 1;
    config->currentMove = 0;

    config->maxMovesToRead   = -1;
    config->maxDepthToSearch = 1000;

    config->isExchange = 0;
    config->verbose = 0;
}

struct gamestate* copyGameState(struct gamestate *toCopy) {
    struct gamestate *retval = malloc(sizeof(struct gamestate));
    if (!retval) {
        fprintf(stderr,"Malloc fail for %lu bytes! Exiting...\n",sizeof(struct gamestate));
    exit(1);
    }
    memcpy(retval, toCopy, sizeof(struct gamestate));
    return retval;
}
