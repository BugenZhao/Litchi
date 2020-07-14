//
// Created by Bugen Zhao on 7/13/20.
//

#ifndef LITCHI_INCLUDE_RESULT_HH
#define LITCHI_INCLUDE_RESULT_HH

enum struct Result : int {
    ok = 0,
    unknownError = 1,
    invalidArg,
    noMemory,
    noFreeTask,
    fuck,
    tooManyArgs,
    badSyntax,
    badCommand,
    noSuchTask,
};

#endif //LITCHI_INCLUDE_RESULT_HH
