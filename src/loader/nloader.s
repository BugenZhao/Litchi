MBOOT_HEADER_MAGIC	equ	0x1BADB002				; Magic number
MBOOT_PAGE_ALIGN	equ	1 << 0					; 引导模块 4KB 对齐
MBOOT_MEM_INFO		equ	1 << 1					; 需要内存信息
MBOOT_HEADER_FLAGS	equ	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM		equ	-(MBOOT_HEADER_MAGIC+MBOOT_HEADER_FLAGS); Checksum 保证和为 0


[BITS	32]
section	.text

dd	MBOOT_HEADER_MAGIC
dd	MBOOT_HEADER_FLAGS
dd	MBOOT_CHECKSUM

[GLOBAL	start]		; 入口
[GLOBAL	glb_mboot_ptr]	; 暴露 grub 汇报的内存信息
[EXTERN	kernel_entry]


start:

	; 关中断
	cli

	mov	esp, STACK_TOP
	mov	ebp, 0				; 初始化帧指针
	and	esp, 0xfffffff0			; 栈顶对齐
	mov	[glb_mboot_ptr], ebx		; 暴露 grub 汇报的内存信息

	call	kernel_entry

Fin:
	hlt
	jmp	Fin


section	.bss
stack:
	resb	32768
glb_mboot_ptr:
	resb	4

STACK_TOP	equ	$ - stack - 1

; section	.data
