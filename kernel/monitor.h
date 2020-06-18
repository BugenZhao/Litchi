//
// Created by Bugen Zhao on 2020/3/27.
//

#ifndef LITCHI_MONITOR_H
#define LITCHI_MONITOR_H

#include <kernel/knlast.inc>

int monitorEcho(int argc, char **argv);

int monitorHelp(int argc, char **argv);

int monitorUname(int argc, char **argv);

int monitorBacktrace(int argc, char **argv);

int monitorVmshow(int argc, char **argv);

int monitorVmdumpv(int argc, char **argv);

int monitorVmdumpp(int argc, char **argv);

#endif //LITCHI_MONITOR_H
