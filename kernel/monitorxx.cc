//
// Created by Bugen Zhao on 7/3/20.
//

#include "kernel/monitor.h"
#include "include/stdio.h"
#include "include/color.h"

int monitorCount(int argc, char **argv) {
    consolePrintFmt("%<argc%< = %d\n", GREEN, DEF_FORE, argc);
    return 0;
}
