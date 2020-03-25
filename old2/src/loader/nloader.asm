;
; Created by Bugen Zhao on 2020/2/20.
;

MBOOT_HEADER_MAGIC	equ	0x1BADB002				; Magic number
MBOOT_PAGE_ALIGN	equ	1 << 0					; 引导模块 4KB 对齐
MBOOT_MEM_INFO		equ	1 << 1					; 需要内存信息
MBOOT_HEADER_FLAGS	equ	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM		equ	-(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS); Checksum 保证和为 0

; https://wiki.osdev.org/Grub_Error_13
; 使用独立的 section 保证内核较大时 multiboot header 也能位于第一个 8KB 内
section .__mbHeader
align 0x4

dd	MBOOT_HEADER_MAGIC
dd	MBOOT_HEADER_FLAGS
dd	MBOOT_CHECKSUM


[BITS	32]
section	.text


[GLOBAL	start]		; 入口
[GLOBAL	glb_mboot_ptr]	; 暴露 grub 汇报的内存信息
[EXTERN	kernel_entry]


start:

	cli					; 关中断

	mov	esp, STACK_TOP
	mov	ebp, 0				; 初始化帧指针
	and	esp, 0xfffffff0			; 栈顶对齐
	mov	[glb_mboot_ptr], ebx		; 暴露 grub 汇报的内存信息

	call	kernel_entry

fin:
	hlt
	jmp	fin


section	.bss
stack:
	resb	32768
glb_mboot_ptr:
	resb	4

STACK_TOP	equ	$ - stack - 1
