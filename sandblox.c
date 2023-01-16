/* sandblox.c Open files of sandbox games with ease






*/
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "sandblox.h"
#define is_digit(chr) ((chr >= '0' && chr <= '9'))

/*
    parses .brk files

*/
struct Game parseBrk(const char* in, size_t len) {
    struct Game game;
    game.vstr = calloc(2, 1);
    /*  I should use struct of arrays rather than arrays of structs for
        performance reasons. I want to make this library fast as possible 
    */
    game.bricks = malloc(sizeof(struct Brick*));
    game.pc = 0;
    char* numstr = calloc(41, 1);
    char* buffer = malloc(sizeof(struct Brick));
    char* property = calloc(10, 1);
    
    /*
        s at end means state
        i at end is iterator
    */
    uint8_t vstri = 0;
    uint8_t vstrs = 1;

    uint8_t parse = 0;
    /* amount of \r\n sequences */
    uint8_t wp = 0;
    uint8_t tab_s = 0;
    uint8_t ci = 0;
    uint8_t numstri = 0;
    uint8_t propertyrs = 0;
    uint8_t propertyvalrs = 0;
    uint8_t instance_found = 0;
    uint8_t propi = 0;
    uint8_t propvali = 0;
    uint16_t i_ = 0;
    uint8_t props = 0;
    for(unsigned i = 0; i < len; i++) {
        /* read the version string */
        if(vstrs) {
            if(in[i] == '\r') { vstrs = 0; parse = 1;}
            char* ec = realloc(game.vstr, vstri + 1);
            if(ec != NULL) game.vstr = ec;
            game.vstr[vstri] = in[i];
            
            vstri++;
        }
        
        if(parse) {
            /* avoid reading outside of buffer
            */
           if(i + 3 <= len)
                if(in[i] == '\r' && in[i + 1] == '\n' && in[i + 2] == '\r' && in[i + 3] == '\n') {
                
                    wp++;

                }
            if(wp == 2) {
                
                tab_s = 1;
                wp = 1;
            }    

            if(propertyvalrs) {
                if(in[i] == '\r') {
                    tab_s = 1;
                    propertyvalrs = 0;
                }
                /* TODO: Optimize this by avoiding recalculations
                    and optimize by storing string length at first two bytes */
                if(props == 1) {
                    const uint32_t index = game.pc - (game.pc > 0);
                    char* ec = realloc(game.bricks[index]->n, 2 + i_);
                    if(ec != NULL) game.bricks[index]->n = ec;
                    game.bricks[index]->n[1 + i_] = 0;
                    game.bricks[index]->n[i_] = in[i];
                    i_++;
                    game.bricks[index]->flags |= 2;
                }

            }
            if(propertyrs) {
                if(in[i] == ' ') {propertyrs = 0; propertyvalrs = 1;}
                if(propi > 5) propi = 0;
                property[propi] = in[i];
                propi++;
                if(strstr(property, "N") != NULL) props = 1;
                if(strstr(property, "S") != NULL) props = 2;
                if(strstr(property, "R") != NULL) props = 4;
                i_ = 0;
            }
            if(in[i] == '+') {
                memset(property, 0, 6);                
                propertyrs = 1;
                propi = 0;
            }
            if(tab_s) {
                if(in[i] == ' ' || ((in[i] == '\r') && instance_found)) {

                    float r = atof(numstr);
                    if(ci <= 3)
                    memcpy(buffer + (sizeof(float) * ci), &r, sizeof(float));
                    memset(numstr, 0, 40);
                    numstri = 0;
                    ci++;
                    if(ci >= 10) { 
                        ci = 0;
                    
                        /* resize parts */
                        char* ec = realloc(game.bricks, (game.pc + 1) * sizeof(struct Brick*));
                        if(ec != NULL) game.bricks = (struct Brick**)ec;
                        game.bricks[game.pc] = (struct Brick*)buffer;
                        buffer = calloc(1, sizeof(struct Brick));
                        ((struct Brick*)buffer)->n = calloc(2, 1);
                        game.pc++;
                        
                    }
                    if(in[i] != '\r')
                    instance_found = 1;
                    else instance_found = 0;

                }               
                /* printf("%c", in[i]); */
                if(in[i] == 0x09) {
                    tab_s = 0;
                }
                if(is_digit(in[i]) || in[i] == '.' || in[i] == '-') {
                    numstr[numstri] = in[i];
                    numstri++;
                }
            }       
        }
    }
    return game;
}

/*
    .poly is polytoria game map format
    format is json which is parsable in a tree structure
*/
struct Game parsePoly(const char* in, size_t len) {
    struct Game game;
    game.pc = 0;
    unsigned lines = 0;

    /* bit arrays are more efficient but lazy */
    unsigned char startedTag = 0;
    unsigned char readProperty = 0;
    char* currentTag = calloc(2, 1);
    char* property = calloc(2, 1);
    char* propertyVal = calloc(2, 1);

    /* iterator of current tag */
    unsigned cti = 0;
    /* property iterator */
    unsigned pti = 0;
    
    unsigned pvti = 0;



    for(unsigned i = 0; i < len; i++) {
        /*
            I should refactor this in a future data to something less nested
        */

        if(in[i] == '\n') lines++;
        /* we do not want metadata about the xml file itself */
        if(lines > 0) {
            if(readProperty) {
                property[pti] = in[i];
                pti++;
                char* ec = realloc(property, 2 + pti);
                property = ec;
            }
            if(in[i] == '>') {
                /* stop reading the tag but that's still not the end
                get to step 2*/
                startedTag = 0;
                puts(currentTag);
                free(currentTag);
                cti = 0;
                currentTag = calloc(2, 1);
            }  
            if(startedTag && !readProperty) {
                if(in[i] == ' ') {
                    readProperty = 1;
                }
                currentTag[cti] = in[i];
                cti++;
                char* ec = realloc(currentTag, 2 + cti);
                if(ec != NULL) currentTag = ec;
                else {
                    fputs("Cannot realloc\n", stderr);
                    return game;
                }
                currentTag[1 + cti] = 0;
            }
            if(in[i] == '<' && in[i + ((i + 1) < len)] != '/') {
                /* start reading the tag 
                    if in step 2
                    also make this sub part of tag we are begin
                    */
                   startedTag = 1;
            }
          
            if(in[i] == '/') {
                /*portion ends completely*/
            }
        }
    }
    free(property);
    free(propertyVal);
    free(currentTag);
    return game;
}

char* exportBrk(struct Game game) {
    size_t vstrs = strlen(game.vstr);
    char* buffer = calloc(vstrs + 2, 1);


    memcpy(buffer, game.vstr, vstrs - 1);
    buffer[vstrs - 1] = '\n';

    return buffer;
}