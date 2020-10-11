//
// Created by Bugen Zhao on 2020/3/27.
//

#include <include/stdio.hh>
#include <kernel/monitor.hh>
#include <kernel/version.h>
#include <kernel/system.hh>
#include <include/string.hh>
#include <kernel/kdebug.hh>
#include <kernel/vmem.hh>
#include "console.hh"
#include "kdebug.hh"
#include "system.hh"
#include "vmem.hh"
#include "task.hh"

using namespace console::out;
extern uint8_t embUserElf[];

namespace monitor {
    // 16 args at most, with command name
    constexpr int MAX_ARGS = 16;
    using FuncType = Result(int argc, char **argv, trap::Frame *tf);

    struct Command {
        const char *cmd;
        const char *desc;
        FuncType *func;
        bool hide = false;

        bool operator<(const Command &rhs) const {
            return str::cmp(cmd, rhs.cmd) < 0;
        }
    };
}

namespace monitor {
    FuncType help;

    // echo some colorful words
    Result echo(int argc, char **argv, trap::Frame *tf) {
        for (int i = 1; i < argc; ++i) {
            console::out::print("%<%s ", (i + str::count(argv[i])) % 15 + BLUE, argv[i]);
        }
        console::out::print("\n");
        return Result::ok;
    }

    // display litchi
    Result uname(int argc, char **argv, trap::Frame *tf) {
        if (argc >= 2 && str::cmp(argv[1], "-a") == 0)
            console::out::print("Litchi v%s by BugenZhao\n", LITCHI_VERSION_TIME);
        else console::out::print("Litchi\n");
        return Result::ok;
    }

    // print backtrace
    Result backtrace(int argc, char **argv, trap::Frame *tf) {
        kdebug::backtrace();
        return Result::ok;
    }

    // show vm mapping
    Result vmshow(int argc, char **argv, trap::Frame *tf) {
        if (argc <= 1) return Result::invalidArg;
        void *beginV = (void *) (str::toLong(argv[1], 0));
        void *endV = (argc >= 3) ? (void *) (str::toLong(argv[2], 0)) : beginV;
        vmem::utils::show(vmem::kernelPageDir, beginV, endV);
        return Result::ok;
    }

    // dump virtual memory
    Result vmdumpv(int argc, char **argv, trap::Frame *tf) {
        if (argc <= 1) return Result::invalidArg;
        void *beginV = (void *) (str::toLong(argv[1], 0));
        void *endV = (argc >= 3) ? (void *) (str::toLong(argv[2], 0)) : beginV;
        vmem::utils::dumpV(vmem::kernelPageDir, beginV, endV);
        return Result::ok;
    }

    // dump physical memory
    Result vmdumpp(int argc, char **argv, trap::Frame *tf) {
        if (argc <= 1) return Result::invalidArg;
        physaddr_t beginP = (str::toLong(argv[1], 0));
        physaddr_t endP = (argc >= 3) ? (str::toLong(argv[2], 0)) : beginP;
        vmem::utils::dumpP(vmem::kernelPageDir, beginP, endP);
        return Result::ok;
    }

    // run user task from embedded ELF
    Result runtask(int argc, char **argv, trap::Frame *tf) {
        using namespace task;
        static bool started = false;

        auto[task, r] = Task::create(embUserElf, TaskType::user,
                                     (char *) EMBUSER_ELF + BINARY_DIR_OFFSET);
        started = true;
        task->run(true);
    }

    // USER TASK DEBUG: continue
    Result cont(int, char **, trap::Frame *tf) {
        if (tf) {
            tf->eflags &= ~FL_TF;   // clear Trap Flag (single-step)
            tf->pop();
        } else {
            return Result::noSuchTask;
        }
    }

    // USER TASK DEBUG: single instruction
    Result si(int, char **, trap::Frame *tf) {
        if (tf) {
            tf->eflags |= FL_TF;    // set Trap Flag (single-step)
            tf->pop();
        } else {
            return Result::noSuchTask;
        }
    }

    Result fuck(int, char **, trap::Frame *) {
        return Result::fuck;
    }

    Result page(int, char **, trap::Frame *) {
        auto count = vmem::PageInfo::freeCount();
        print("%d free pages (%d KB, %d MB)\n", count, count << 2, count << 2 >> 10);
        return Result::ok;
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
            {
                    .cmd  = "si",
                    .desc = "Single-Instruction debug the task",
                    .func = si
            },
            {
                    .cmd  = "fuck",
                    .func = fuck,
                    .hide = true
            },
            {
                    .cmd  = "page",
                    .desc = "Check page statistics",
                    .func = page
            }
    };

    Result help(int argc, char **argv, trap::Frame *tf) {
        for (size_t i = 0; i < ARRAY_SIZE(commands); ++i) {
            if (!commands[i].hide)
                console::out::print("%<%8s%<: %s\n", WHITE, commands[i].cmd, DEF_FORE, commands[i].desc);
        }
        return Result::ok;
    }
}

namespace monitor {
    Result parseCmd(char *cmd, trap::Frame *tf) {
        char *argv[MAX_ARGS + 2];
        int argc = str::splitWs(cmd, argv, MAX_ARGS + 2);

        if (argc == MAX_ARGS + 1) return Result::tooManyArgs;
        else if (argc == -1) return Result::badSyntax;

        for (size_t i = 0; i < ARRAY_SIZE(commands); ++i) {
            if (str::cmpCase(argv[0], commands[i].cmd) == 0) {
                return commands[i].func(argc, argv, tf);
            }
        }
        return Result::badCommand;
    }

    /// The entry of the kernel monitor
    /// \param tf trap frame of current task
    /// \return nothing
    [[noreturn]] int main(trap::Frame *tf) {
        char *cmd;
        auto lastRet = Result::ok;
        console::out::print("\n");
        while (true) {
            auto color = lastRet != Result::ok ? RED : DEF_FORE;
            // show current task if needed
            if (task::Task::current != nullptr)
                cmd = console::in::readline("%<Litchi%<[%08x]%<> ",
                                            LIGHT_MAGENTA, WHITE, task::Task::current->id, color);
            else
                cmd = console::in::readline("%<Litchi%<> ",
                                            LIGHT_MAGENTA, color);

            if (*cmd && cmd[0]) lastRet = parseCmd(cmd, tf);

            if (lastRet != Result::ok) console::err::print("%s: %r\n", cmd, lastRet);
        }
    }
}
