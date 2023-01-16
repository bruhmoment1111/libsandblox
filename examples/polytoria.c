#include <stdio.h>
#include <stdlib.h>
#include "../sandblox.h"
/*
    This project is not affilated in Polytoria in any way

    Trademarks belongs to respective owners

*/

int main(void) {
    /* This is bare bones application to read polytoria games
        you must have a example.poly file in same directory
    */


    FILE* f;
    f = fopen("example.poly", "r");
    if(f == NULL) {
        fputs("File not found", stderr);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    size_t s = ftell(f);
    fseek(f, 0, SEEK_SET);



    char* buf = malloc(s);
    for(unsigned i = 0; i < s; i++) buf[i] = fgetc(f);
    struct Game game = parsePoly(buf, s);
    for(unsigned i = 0; i < game.pc; i++) {
        /* test flags for features before actions
            otherwise you will get a nasty segmentation fault
        */
        if(hasBrickName(game.bricks[i]->flags))
            printf("x: %.2f y: %.2f z: %.2f name: %s\n", game.bricks[i]->n, game.bricks[i]->x, game.bricks[i]->y, game.bricks[i]->z);
        else
            printf("x: %.2f y: %.2f z: %.2f\n", game.bricks[i]->x, game.bricks[i]->y, game.bricks[i]->z);
        
    }
    free(buf);
    return 0;   
}