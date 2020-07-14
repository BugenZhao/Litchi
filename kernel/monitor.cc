//
// Created by Bugen Zhao on 2020/3/27.
//

#include <include/stdio.hpp>
#include <kernel/monitor.hpp>
#include <kernel/version.h>
#include <kernel/system.hpp>
#include <include/string.hpp>
#include <kernel/kdebug.hpp>
#include <kernel/vmem.hpp>
#include "console.hpp"
#include "kdebug.hpp"
#include "system.hpp"
#include "vmem.hpp"
#include "task.hh"

using namespace console::out;
extern uint8_t embUserElf[];

namespace monitor {
    // 16 args at most, with command name
    constexpr int MAX_ARGS = 16;

    struct Command {
        const char *cmd;
        const char *desc;

        FuncType *func;
    };
}

namespace monitor {
    int echo(int argc, char **argv, trap::Frame *tf) {
        for (int i = 1; i < argc; ++i) {
            console::out::print("%<%s ", (i + str::count(argv[i])) % 15 + BLUE, argv[i]);
        }
        console::out::print("\n");
        return 0;
    }

    int uname(int argc, char **argv, trap::Frame *tf) {
        if (argc >= 2 && str::cmp(argv[1], "-a") == 0)
            console::out::print("Litchi v%s by BugenZhao\n", LITCHI_VERSION);
        else console::out::print("Litchi\n", LITCHI_VERSION);
        return 0;
    }

    int backtrace(int argc, char **argv, trap::Frame *tf) {
        kdebug::backtrace();
        return 0;
    }

    int vmshow(int argc, char **argv, trap::Frame *tf) {
        if (argc <= 1) {
            print("%s: Invalid argument\n", argv[0]);
            return -1;
        }
        void *beginV = (void *) (str::toLong(argv[1], 0));
        void *endV = (argc >= 3) ? (void *) (str::toLong(argv[2], 0)) : beginV;
        vmem::utils::show(vmem::kernelPageDir, beginV, endV);
        return 0;
    }

    int vmdumpv(int argc, char **argv, trap::Frame *tf) {
        if (argc <= 1) {
            print("%s: Invalid argument\n", argv[0]);
            return -1;
        }
        void *beginV = (void *) (str::toLong(argv[1], 0));
        void *endV = (argc >= 3) ? (void *) (str::toLong(argv[2], 0)) : beginV;
        vmem::utils::dumpV(vmem::kernelPageDir, beginV, endV);
        return 0;
    }

    int vmdumpp(int argc, char **argv, trap::Frame *tf) {
        if (argc <= 1) {
            print("%s: Invalid argument\n", argv[0]);
            return -1;
        }
        physaddr_t beginP = (str::toLong(argv[1], 0));
        physaddr_t endP = (argc >= 3) ? (str::toLong(argv[2], 0)) : beginP;
        vmem::utils::dumpP(vmem::kernelPageDir, beginP, endP);
        return 0;
    }

    int runtask(int argc, char **argv, trap::Frame *tf) {
        using namespace task;
        static bool started = false;
        if (started) return -1;

        auto[task, r] = Task::create(embUserElf, TaskType::user);
        started = true;
        task->run();
    }

    int cont(int, char **, trap::Frame *tf) {
        if (tf) tf->pop();
        else {
            console::err::print("Not a breakpoint\n");
            return -1;
        }
    }

    struct Command commands[] = {
            {
                    .cmd = "echo",
                    .desc = "Write colorful arguments to the standard output",
                    .func = echo
            },
            {
                    .cmd = "help",
                    .desc = "Show this help message",
                    .func = help
            },
            {
                    .cmd = "uname",
                    .desc = "Print operating system name",
                    .func = uname
            },
            {
                    .cmd = "cpu",
                    .desc = "Print CPU information",
                    .func = (FuncType *) sys::cpuInfo
            },
            {
                    .cmd = "clear",
                    .desc = "Clear the console screen",
                    .func = (FuncType *) console::clear
            },
            {
                    .cmd = "reboot",
                    .desc = "Restart the system",
                    .func = (FuncType *) sys::reboot
            },
            {
                    .cmd = "shut",
                    .desc = "Shutdown the system",
                    .func = (FuncType *) sys::shutdown
            },
            {
                    .cmd = "backtr",
                    .desc = "Print backtrace",
                    .func = backtrace
            },
            {
                    .cmd = "vmshow",
                    .desc = "Show memory map at virtual address [%1, %2]",
                    .func = vmshow
            },
            {
                    .cmd  = "vmdumpv",
                    .desc = "Dump the contents at virtual address [%1, %2]",
                    .func = vmdumpv
            },
            {
                    .cmd  = "vmdumpp",
                    .desc = "Dump the contents at physical address [%1, %2]",
                    .func = vmdumpp
            },
            {
                    .cmd  = "runtask",
                    .desc = "Run the initial task",
                    .func = runtask
            },
            {
                    .cmd  = "cont",
                    .desc = "Continue the task",
                    .func = cont
            },
    };

    int help(int argc, char **argv, trap::Frame *tf) {
        for (size_t i = 0; i < ARRAY_SIZE(commands); ++i) {
            console::out::print("%<%8s%<: %s\n", WHITE, commands[i].cmd, DEF_FORE, commands[i].desc);
        }
        return 0;
    }

    int parseCmd(char *cmd, trap::Frame *tf) {
        char *argv[MAX_ARGS + 2];
        int argc = str::splitWs(cmd, argv, MAX_ARGS + 2);

        //    printFmt("argc: %d, argv:\n", argc);
        //    for (int j = 0; j <= argc; ++j) {
        //        printFmt("[%d]: %s\n", j, argv[j]);
        //    }

        if (argc == MAX_ARGS + 1) {
            print("Too many arguments\n");
            return -1;
        } else if (argc == -1) {
            print("Bad syntax\n");
            return -2;
        }


        for (size_t i = 0; i < ARRAY_SIZE(commands); ++i) {
            if (str::cmpCase(argv[0], commands[i].cmd) == 0) {
                return commands[i].func(argc, argv, tf);
            }
        }
        print("Bad command: %s\n", cmd);
        return -1;
    }

    [[noreturn]] int main(trap::Frame *tf) {
        char *cmd;
        int lastRet = 0;
        console::out::print("\n");
        while (true) {
            if (task::Task::current != nullptr)
                cmd = console::in::readline("%<Litchi%<[%08x]%<> ",
                                            LIGHT_MAGENTA, WHITE, task::Task::current->id, lastRet ? RED : DEF_FORE);
            else
                cmd = console::in::readline("%<Litchi%<> ",
                                            LIGHT_MAGENTA, lastRet ? RED : DEF_FORE);
            if (*cmd && cmd[0]) lastRet = parseCmd(cmd, tf);
        }
    }
}
