//
// Created by Bugen Zhao on 7/13/20.
//

extern void umain(int argc, char **argv);

extern "C" {
void libmain(int argc, char **argv) {
    umain(argc, argv);
}
}