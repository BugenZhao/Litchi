//
// Created by Bugen Zhao on 7/13/20.
//

#include <user/include/stdlib.hh>

extern int main(int argc, char **argv);

extern "C" {
void umain(int argc, char **argv) {
    main(argc, argv);
    syscall(ksyscall::Num::exit);
}
}