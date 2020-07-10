//
// Created by Bugen Zhao on 7/10/20.
//

#ifndef LITCHI_CSTDOUT_H
#define LITCHI_CSTDOUT_H

#ifdef __cplusplus
extern "C" {
#endif
int consolePrintFmtC(const char *fmt, ...);
void consoleErrorPrintFmtC(const char *fmt, ...);
#ifdef __cplusplus
}
#endif

#endif //LITCHI_CSTDOUT_H
