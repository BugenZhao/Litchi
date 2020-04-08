//
// Created by Bugen Zhao on 2020/3/27.
//

#include <include/stdio.h>
#include <kernel/monitor.h>
#include <kernel/version.h>
#include <kernel/system.h>
#include <include/string.h>
#include <kernel/kdebug.h>
#include <kernel/pmap.h>

// 16 args at most, with command name
#define MAX_ARGS 16

static int lastRet = 0;

struct Command {
    const char *cmd;
    const char *desc;

    int (*func)(int argc, char **argv);
};

struct Command commands[] = {
        {
                .cmd = "echo",
                .desc = "Write colorful arguments to the standard output",
                .func = monitorEcho
        },
        {
                .cmd = "help",
                .desc = "Show this help message",
                .func = monitorHelp
        },
        {
                .cmd = "uname",
                .desc = "Print operating system name",
                .func = monitorUname
        },
        {
                .cmd = "clear",
                .desc = "Clear the console screen",
                .func = (int (*)(int, char **)) consoleClear
        },
        {
                .cmd = "reboot",
                .desc = "Restart the system",
                .func = (int (*)(int, char **)) reboot
        },
        {
                .cmd = "backtr",
                .desc = "Print backtrace",
                .func = monitorBacktrace
        },
        {
                .cmd = "vmmap",
                .desc = "Show memory map at virtual address [%1, %2)",
                .func = monitorVmMap
        }
};

int monitorEcho(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        consolePrintFmt("%<%s ", (i + stringLength(argv[i])) % 15 + BLUE, argv[i]);
    }
    consolePrintFmt("\n");
    return 0;
}

int monitorHelp(int argc, char **argv) {
    for (int i = 0; i < ARRAY_SIZE(commands); ++i) {
        consolePrintFmt("%<%8s%<: %s\n", WHITE, commands[i].cmd, DEF_FORE, commands[i].desc);
    }
    return 0;
}

int monitorUname(int argc, char **argv) {
    if (argc >= 2 && stringCompare(argv[1], "-a") == 0)
        consolePrintFmt("Litchi v%s by BugenZhao\n", LITCHI_VERSION);
    else consolePrintFmt("Litchi\n", LITCHI_VERSION);
    return 0;
}

int monitorBacktrace(int argc, char **argv) {
    backtracePrint();
    return 0;
}

int monitorVmMap(int argc, char **argv) {
    if (argc <= 1) {
        consoleErrorPrintFmt("vmmap: Invalid argument\n");
        return -1;
    }
    void *beginV = (void *) (stringToLong(argv[1], 0));
    void *endV = (argc >= 3) ? (void *) (stringToLong(argv[2], 0)) : beginV;
    vmemoryShow(kernelPageDir, beginV, endV);
    return 0;
}

int parseCmd(char *cmd) {
    char *argv[MAX_ARGS + 2];
    int argc = stringSplitWS(cmd, argv, MAX_ARGS + 2);

//    consolePrintFmt("argc: %d, argv:\n", argc);
//    for (int j = 0; j <= argc; ++j) {
//        consolePrintFmt("[%d]: %s\n", j, argv[j]);
//    }

    if (argc == MAX_ARGS + 1) {
        consoleErrorPrintFmt("Too many arguments\n");
        return -1;
    } else if (argc == -1) {
        consoleErrorPrintFmt("Bad syntax\n");
        return -2;
    }

    if (stringCaseCompare(argv[0], "fuck") == 0) return 0x80000000u;

    for (int i = 0; i < ARRAY_SIZE(commands); ++i) {
        if (stringCaseCompare(argv[0], commands[i].cmd) == 0) {
            return commands[i].func(argc, argv);
        }
    }
    consoleErrorPrintFmt("Bad command: %s\n", cmd);
    return -1;
}

int monitor(void) {
    char *cmd;
//    consolePrintFmt("%d %d 0x%08X 0x%08X\n",
//                    stringToLong("+2020", 0),
//                    stringToLong("-2147483648", 10),
//                    stringToLong("     0x3abcdefzz", 0),
//                    stringToLong("400  ", 16));
    while (lastRet != 0x80000000u) {
        if (lastRet == 0) cmd = consoleReadline("%<LitchiK%<> ", LIGHT_MAGENTA, DEF_FORE);
        else cmd = consoleReadline("%<LitchiK%<> ", LIGHT_MAGENTA, RED);
        if (*cmd && cmd[0]) lastRet = parseCmd(cmd);
    }
    return lastRet;
}