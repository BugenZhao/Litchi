//
// Created by Bugen Zhao on 7/13/20.
//

#ifndef LITCHI_TASK_HH
#define LITCHI_TASK_HH

#include <include/task.hh>
#include <include/trap.hh>
#include <include/result.hh>
#include <tuple>

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

        Result setupMemory();
        void free();
    };

    void allocArray();

    void init();

    void initPerCpu();
}

#endif //LITCHI_TASK_HH
