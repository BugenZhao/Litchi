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
;                              段基址,       段界限     , 属性
LABEL_GDT:	   Descriptor       0,                0, 0           ; 空描述符
LABEL_DESC_CODE32: Descriptor       0, SegCode32Len - 1, DA_C + DA_32; 非一致代码段
LABEL_DESC_VIDEO:  Descriptor 0B8000h,           0ffffh, DA_DRW	     ; 显存首地址
; GDT 结束

GdtLen		equ	$ - LABEL_GDT	; GDT长度
GdtPtr		dw	GdtLen - 1	; GDT界限
		dd	0		; GDT基地址

; GDT 选择子
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
; END of [SECTION .gdt]

[SECTION .s16]
[BITS	16]

DispStr:
	mov	ax, 0x0003
	int	0x10; clear screen
	mov	ax, BootMessage
	mov	bp, ax			; string address
	mov	cx, 14+2+14		; string length
	mov	ax, 0x1301		; ah=0x13 (write string)
	mov	bx, 0x000d		; bh=0x00 (page), bl=0x0d (color)
	mov	dl, 0			; row & col
	int	0x10
	ret

BootMessage:            
	db 	"Hello, Litchi!"
	db 	0x0d			; cl
	db 	0x0a			; rf
	db 	"BugenZhao 2020"
Entry:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h
	call	DispStr

	; 初始化 32 位代码段描述符
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE32
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah

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

	; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; 真正进入保护模式
	jmp	dword SelectorCode32:0	; 执行这一句会把 SelectorCode32 装入 cs,
					; 并跳转到 Code32Selector:0  处
; END of [SECTION .s16]

[SECTION .s32]; 32 位代码段. 由实模式跳入.
[BITS	32]

LABEL_SEG_CODE32:
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子(目的)

	mov	edi, (80 * 11 + 40) * 2	; 屏幕第 11 行, 第 40 列。
	mov	ah, 0Ch			; 0000: 黑底    1100: 红字
	mov	al, 'P'
	mov	[gs:edi], ax

Fin:
	; 到此停止
	hlt
	jmp	Fin

SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]
