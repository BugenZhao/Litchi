//
// Created by Bugen Zhao on 7/13/20.
//

#include <user/stdlib.hh>

extern void umain(int argc, char **argv);

extern "C" {
void libmain(int argc, char **argv) {
    umain(argc, argv);
    syscall(ksyscall::SyscallType::exit);
}
}