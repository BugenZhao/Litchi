//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_MONITOR_H
#define LITCHI_MONITOR_H

#include <kernel/knlast.inc>


namespace monitor {
    int echo(int argc, char **argv);

    int help(int argc, char **argv);

    int uname(int argc, char **argv);

    int backtrace(int argc, char **argv);

    int vmshow(int argc, char **argv);

    int vmdumpv(int argc, char **argv);

    int vmdumpp(int argc, char **argv);

    int main();
}

#endif //LITCHI_MONITOR_H
