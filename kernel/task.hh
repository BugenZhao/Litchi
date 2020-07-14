//
// Created by Bugen Zhao on 7/13/20.
//

#ifndef LITCHI_TASK_HH
#define LITCHI_TASK_HH

#include <include/task.hh>
#include <include/result.hh>
#include <tuple>
#include "trap.hh"

namespace task {
    struct Task {
        static constexpr size_t maxCount = 0x100;
        static Task *array;
        static Task *freeList;
        static Task *current;

        Task *nextFree;
        taskid_t id;
        taskid_t parentId;
        TaskStatus status;
        TaskType type;

        trap::Frame trapFrame;      // task status when trap into the kernel
        pde_t *pageDir;

        static std::tuple<Task *, Result> alloc(taskid_t parentId);

        static std::tuple<Task *, Result> create(uint8_t *binary, TaskType type);

        [[noreturn]] void run();

        void free();

        void destroy(bool fromKernel);

    private:
        Result setupMemory();

        void loadElf(uint8_t *binary);

        void regionAlloc(void *va, size_t len);
    };

    void allocArray();

    void init();

    void initPerCpu();
}

#endif //LITCHI_TASK_HH
