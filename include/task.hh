//
// Created by Bugen Zhao on 7/13/20.
//

#ifndef LITCHI_INCLUDE_TASK_HH
#define LITCHI_INCLUDE_TASK_HH

namespace task {
    using taskid_t = int32_t;

    enum struct TaskStatus : int {
        free = 0, dying, ready, running, waiting
    };

    enum struct TaskType : int {
        user = 0
    };
}

#endif //LITCHI_INCLUDE_TASK_HH
