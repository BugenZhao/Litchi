[BITS	32]
section .text
[GLOBAL	start]
[EXTERN	kernel_entry]
start:
	call	kernel_entry
