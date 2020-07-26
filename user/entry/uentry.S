#include <include/mmu.h>
#include <include/memlayout.h>

.text
.globl _ustart
_ustart:
	// See if we were started with arguments on the stack
	cmpl $USTACKTOP, %esp
	jne args_exist

	// If not, push dummy argc/argv arguments.
	// This happens when we are loaded by the kernel,
	// because the kernel does not know about passing arguments.
	pushl $0
	pushl $0

args_exist:
	call umain

bad:
	ud2
	jmp bad
