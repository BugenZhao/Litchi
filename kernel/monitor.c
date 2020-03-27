//
// Created by Bugen Zhao on 2020/3/27.
//

#include <include/stdio.h>
#include <kernel/monitor.h>
#include <kernel/version.h>
#include <kernel/system.h>
#include <include/string.h>

struct command_t {
    const char *cmd;
    const char *desc;

    int (*func)(int argc, char **argv);
};

struct command_t commands[] = {
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
        }
};

int monitorHelp(int argc, char **argv) {
    for (int i = 0; i < ARRAY_SIZE(commands); ++i) {
        consolePrintFmt("%<%8s%<: %s\n", WHITE, commands[i].cmd, DEF_FORE, commands[i].desc);
    }
    return 0;
}

int monitorUname(int argc, char **argv) {
    consolePrintFmt("Litchi v%s by BugenZhao\n", LITCHI_VERSION);
    return 0;
}

int parseCmd(char *cmd) {
    for (int i = 0; i < ARRAY_SIZE(commands); ++i) {
        if (stringCompare(cmd, commands[i].cmd) == 0) {
            return commands[i].func(0, NULL);
        }
    }
    consoleErrorPrintFmt("Bad command: %s\n", cmd);
    return -1;
}

int monitor(void) {
    char *cmd;
    while (true) {
        cmd = consoleReadline("%<Litchi> ", LIGHT_MAGENTA);
        if (*cmd) parseCmd(cmd);
    }
}