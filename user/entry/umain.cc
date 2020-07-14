//
// Created by Bugen Zhao on 7/13/20.
//

#include <user/stdlib.hh>

extern int main(int argc, char **argv);

extern "C" {
void libmain(int argc, char **argv) {
    main(argc, argv);
    syscall(ksyscall::SyscallType::exit);
}
}