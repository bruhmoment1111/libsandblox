# libsandblox
A C library to parse game levels from various sandbox websites / games

Currently Supported sandbox games:
|Game|Format|Is Supported (Import)| Is Supported (Export)|
|----|-----|-------------|-------------------------------|
|Brick Hill| .brk | ✔️ | ⌛ |
|Roblox| .rbxl/.rbxlx | ❌ | ❌ |
|Polytoria|.poly| ⌛ | ❌ |

## Example
This example imports a .brk file and displays list of bricks


```c
#include <stdio.h>
#include <stdlib.h>
#include "sandblox.h"
int main(int argc, char** argv) {
    
    FILE* f;
    f = fopen("example.brk", "r");
    fseek(f, 0, SEEK_END);
    size_t s = ftell(f);
    fseek(f, 0, SEEK_SET);



    char* buf = malloc(s);
    for(unsigned i = 0; i < s; i++) buf[i] = fgetc(f);
    struct Game game = parseBrk(buf, s);
    printf("Version: %s\n", game.vstr);
    for(unsigned i = 0; i < game.pc; i++) {
        /* test flags for features before actions
            otherwise you will get a nasty segmentation fault
        */
        if(hasBrickName(game.bricks[i]->flags))
            printf("x: %.2f y: %.2f z: %.2f name: %s\n", game.bricks[i]->n, game.bricks[i]->x, game.bricks[i]->y, game.bricks[i]->z);
        else
            printf("x: %.2f y: %.2f z: %.2f\n", game.bricks[i]->x, game.bricks[i]->y, game.bricks[i]->z);
        
    }
    return 0;
}
```
