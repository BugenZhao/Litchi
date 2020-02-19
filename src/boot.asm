%include	"pm.inc"

	org	0x7c00
	jmp	Entry

; Standard FAT12 floppy code
	DB	0x90
	DB	"BZLITCHI"		; 启动扇区名称（8字节）
	DW	512			; 每个扇区（sector）大小（必须512字节）
	DB	1			; 簇（cluster）大小（必须为1个扇区）
	DW	1			; FAT起始位置（一般为第一个扇区）
	DB	2			; FAT个数（必须为2）
	DW	224			; 根目录大小（一般为224项）
	DW	2880			; 该磁盘大小（必须为2880扇区1440*1024/512）
	DB	0xf0			; 磁盘类型（必须为0xf0）
	DW	9			; FAT的长度（必??9扇区）
	DW	18			; 一个磁道（track）有几个扇区（必须为18）
	DW	2			; 磁头数（必??2）
	DD	0			; 不使用分区，必须是0
	DD	2880			; 重写一次磁盘大小
	DB	0,0,0x29		; 意义不明（固定）
	DD	0xffffffff		; （可能是）卷标号码
	DB	"BZLITCHIOS "		; 磁盘的名称（11字，不足填空格）
	DB	"FAT12   "		; 磁盘格式名称（8字，不足填空格）
	RESB	18			; 先空出18字节

[SECTION .gdt]
; GDT
;                               段基址,       段界限     , 属性
LABEL_GDT:	   Descriptor        0,                0, 0           		; 空描述符
LABEL_DESC_NORMAL: Descriptor	     0,		  0ffffh, DA_DRW		; Normal, 用于返回实模式
LABEL_DESC_CODE32: Descriptor        0, SegCode32Len - 1, DA_C + DA_32		; 非一致 32 位代码段
;LABEL_DESC_CODE16: Descriptor	     0,           0ffffh, DA_C			; 非一致 16 位代码段
LABEL_DESC_DATA:   Descriptor	     0,      DataLen - 1, DA_DRW		; Data
LABEL_DESC_TEST:   Descriptor 0500000h,		  0ffffh, DA_DRW		; Test
LABEL_DESC_VIDEO:  Descriptor  0B8000h,           0ffffh, DA_DRW	    	; 显存首地址
; GDT 结束

GdtLen			equ	$ - LABEL_GDT	; GDT长度
GdtPtr			dw	GdtLen - 1	; GDT界限
			dd	0		; GDT基地址

; GDT 选择子
SelectorNormal		equ	LABEL_DESC_NORMAL	- LABEL_GDT
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT
;SelectorCode16		equ	LABEL_DESC_CODE16	- LABEL_GDT
SelectorData		equ	LABEL_DESC_DATA		- LABEL_GDT
SelectorTest		equ	LABEL_DESC_TEST		- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
; END of [SECTION .gdt]

[SECTION .data1]
ALIGN	32
[BITS	32]
LABEL_DATA:
PMMessage:		db	"Protected Mode", 0
OffsetPMMessage		equ	PMMessage-$$		; PMMessage 相对 Data Section 偏移
DataLen			equ	$ - LABEL_DATA
; END of [SECTION .data1]

[SECTION .s16]
[BITS	16]
HelloMessage:		db	"Hello, Litchi!", 0x0d, 0x0a, "BugenZhao 2020", 0
HelloMessageEnd:

DispStr:
	mov	ax, 0x0003
	int	0x10					; clear screen
	mov	ax, HelloMessage
	mov	bp, ax					; string address
	mov	cx, HelloMessageEnd - HelloMessage	; string length
	mov	ax, 0x1301				; ah=0x13 (write string)
	mov	bx, 0x000d				; bh=0x00 (page), bl=0x0d (color)
	mov	dl, 0					; row & col
	int	0x10
	ret

Entry:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h
	call	DispStr

	; 初始化 32 位代码段描述符，将 32 位代码的物理地址填写到描述符中
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE32
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah

	; 初始化 Data 段描述符
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_DATA
	mov	word [LABEL_DESC_DATA + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_DATA + 4], al
	mov	byte [LABEL_DESC_DATA + 7], ah

	; 为加载 GDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_GDT		; eax <- gdt 基地址
	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

	; 加载 GDTR
	lgdt	[GdtPtr]

	; 关中断
	cli

	; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

	; 设置 cr0 第 0 位为 1（保护模式）
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; 真正进入保护模式
	jmp	dword SelectorCode32:0	; 执行这一句会把 SelectorCode32 装入 cs,
					; 并跳转到 Code32Selector:0  处

; LABEL_REAL_ENTRY:		; 回到实模式
; 	mov	ax, cs
; 	mov	ds, ax
; 	mov	es, ax
; 	mov	ss, ax

; 	; mov	sp, [SPValueInRealMode]

; 	; 打开地址线A20
; 	in	al, 92h	
; 	and	al, 11111101b
; 	out	92h, al

; 	; 打开中断
; 	sti

; 	mov	ax, 4c00h	; `.
; 	int	21h		; /  回到 DOS

; END of [SECTION .s16]

[SECTION .s32]; 32 位代码段. 由实模式跳入.
[BITS	32]

LABEL_SEG_CODE32:
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子(目的)
	mov	ax, SelectorData
	mov	ds, ax
	mov	ax, SelectorTest
	mov	es, ax
	; ds: Data, gs: Video, es: Test
	mov	edi, (80 * 3 + 0) * 2	; 定位显存行列
	mov	esi, OffsetPMMessage	; PMMessage 偏移，用于段寻址
	; esi: Data, edi: Video
	mov	ah, 0Ch			; 0000: 黑底    1100: 红字
	jmp	LoopTest
Loop:
	mov	[gs:edi], ax		; ah 颜色，al 字，写入显存
LoopUpdate:
	add	edi, 2
LoopTest:
	lodsb				; 加载 DS:(E)SI 的字节到 AL，即 PMMessage
	test	al, al
	jne	Loop

TestWrite:				; 尝试读写 5MiB 处的内存
	xor	esi, esi
	mov	dl, 'O'
	mov	dh, 'K'
	mov	[es:esi], dx
	; (gdb) x/2cb 0x500000
	; 0x500000:       79 'O'  75 'K'
TestRead:
	mov	edi, (80 * 5 + 0) * 2
	mov	ah, 0Bh
	mov	al, [es:esi]
	mov	[gs:edi], ax
	mov	al, [es:esi + 1]
	mov	[gs:edi + 2], ax

Fin:
	hlt
	jmp	Fin

SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]


; [SECTION .s16code]
; ALIGN	32
; [BITS	16]
; LABEL_SEG_CODE16:
; 	; 准备跳回实模式
; 	mov	ax, SelectorNormal
; 	mov	ds, ax
; 	mov	es, ax
; 	mov	fs, ax
; 	mov	gs, ax
; 	mov	ss, ax

; 	; 恢复 cr0
; 	mov	eax, cr0
; 	and	al, 11111110b
; 	mov	cr0, eax

; LABEL_GO_BACK_TO_REAL:
; 	jmp	0:LABEL_REAL_ENTRY	; 段地址会在程序开始处被设置成正确的值

; Code16Len	equ	$ - LABEL_SEG_CODE16
