#include <stddef.h>
#include <stdint.h>
#define hasBrickName(flags) (((flags >> 1) & 1))
/* order of structs are very important, do not edit this if you do not know */

struct Brick{
    /* Position */
    float x; float y; float z;
    /* Size */
    float xs; float ys; float zs;
    /* Color */
    float r; float g; float b; float a;
    /* Extra */
    char*n;  float reflect; unsigned char flags; unsigned char shape;
};

struct Game{
    /* version string */
    char* vstr;
    /* brick count */
    uint64_t pc;
    struct Brick** bricks;
};

struct Game parseBrk(const char* in, size_t len);
char* exportbrk(struct Game game);