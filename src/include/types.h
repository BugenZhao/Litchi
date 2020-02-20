#ifndef LITCHI_TYPES_H
#define LITCHI_TYPES_H

#define NULL 0

#define TRUE 1
#define FALSE 0

typedef unsigned int   uint32_t;
typedef int            int32_t;
typedef unsigned short uint16_t;
typedef short          int16_t;
typedef unsigned char  uint8_t;
typedef char           int8_t;

typedef enum {
    BLACK = 0b0000,
    BLUE,
    GREEN,
    CYAN,
    RED,
    PURPLE,
    YELLOW,
    WHITE,
    BBLACK,
    BBLUE,
    BGREEN,
    BCYAN,
    BRED,
    BPURPLE,
    BYELLOW,
    BWHITE
} color_t;

#endif // LITCHI_TYPES_H
