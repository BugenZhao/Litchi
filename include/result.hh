//
// Created by Bugen Zhao on 7/13/20.
//

#ifndef LITCHI_INCLUDE_RESULT_HH
#define LITCHI_INCLUDE_RESULT_HH

enum struct Result : int {
    ok = 0,
    unknownError = 1,
    invalidArgument,
    noMemory,
    noFreeTask,
    fuck,
    MAX_ERROR
};

#endif //LITCHI_INCLUDE_RESULT_HH
