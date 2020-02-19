%include	"pm.inc"

	org	0x100
	jmp	Entry

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
PMMessage:		db	"Protected Mode entered", 0
TMMessage:		db	"Test memory...", 0
TLMessage:		db	"Test long content...OK", 0
LongMessage:		
db "                         __  _  _         _      _                              "
db "                        / / (_)| |_  ___ | |__  (_)                             "
db "                       / /  | || __|/ __|| '_ \ | |                             "
db "                      / /___| || |_| (__ | | | || |                             "
db "                      \____/|_| \__|\___||_| |_||_|  Loader                     "
times	160 		db " "
times	10		db	"Litchi Long Message Test "
			db	0

OffsetPMMessage		equ	PMMessage-$$		; PMMessage 相对 Data Section 偏移
OffsetTMMessage		equ	TMMessage-$$
OffsetTLMessage		equ	TLMessage-$$
OffsetLongMessage	equ	LongMessage-$$
DataLen			equ	$ - LABEL_DATA
; END of [SECTION .data1]

[SECTION .s16]
[BITS	16]
HelloMessage:		db	"Hello, Litchi!", 0x0d, 0x0a
HelloMessageEnd:
AboutMessage:		db	"This is the loader of Bugen's tiny OS -- Litchi OS.", 0x0d, 0x0a
			db	"Version v0.1.0, Build 200219", 0x0d, 0x0a
			db	0x0d, 0x0a
			db	"(C) BugenZhao 2020"
AboutMessageEnd:

DispStr:
	; begin in di, length in si, row:col in dx, color in bl
	mov	ax, ds
	mov	es, ax					; es
	mov	bp, di					; es:bp -> string address
	mov	cx, si					; string length 
	mov	ax, 0x1301				; ah=0x13 (write string)
	mov	bh, 0x00				; bh=0x00 (page), bl=?? (color)
	int	10h
	ret

Entry:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h
.Hello:
	mov	di, HelloMessage
	mov	si, HelloMessageEnd - HelloMessage
	mov	dx, 0x0400				; row:col
	mov	bl, 0x0e				; color
	call	DispStr

	mov	di, AboutMessage
	mov	si, AboutMessageEnd - AboutMessage
	mov	dx, 0x0600				; row:col
	mov	bl, 0x0e				; color
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
	jmp	PMEntry

PMDispStr:
	; vram_offset in edi, message_offset in esi, color in ah
	jmp	.DispTest
.DispLoop:
	mov	[gs:edi], ax		; ah 颜色，al 字，写入显存
.DispUpdate:
	add	edi, 2
.DispTest:
	lodsb				; 加载 DS:(E)SI 的字节到 AL，即 PMMessage, SI 递增
	test	al, al			; '\0'
	jne	.DispLoop
	ret

PMEntry:
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子(目的)
	mov	ax, SelectorData
	mov	ds, ax
	mov	ax, SelectorTest
	mov	es, ax
	; ds: Data, gs: Video, es: Test
	mov	edi, (80 * 12 + 0) * 2	; 定位显存行列
	mov	esi, OffsetPMMessage	; PMMessage 偏移，用于段寻址
	; esi: Data, edi: Video
	mov	ah, 0Ch			; 0000: 黑底    1100: 红字
	call	PMDispStr

TestMemory:
	mov	edi, (80 * 13 + 0) * 2
	mov	esi, OffsetTMMessage
	mov	ah, 0Bh
	call	PMDispStr
TestWrite:				; 尝试读写 5MiB 处的内存
	xor	esi, esi
	mov	dl, 'O'
	mov	dh, 'K'
	mov	[es:esi], dx
	; (gdb) x/2cb 0x500000
	; 0x500000:       79 'O'  75 'K'
TestRead:
	mov	edi, (80 * 13 + 20) * 2
	mov	ah, 0Bh
	mov	al, [es:esi]
	mov	[gs:edi], ax
	mov	al, [es:esi + 1]
	mov	[gs:edi + 2], ax

TestLongContent:
	mov	edi, (80 * 16 + 0) * 2
	mov	esi, OffsetLongMessage
	mov	ah, 0Ah
	call	PMDispStr
	mov	edi, (80 * 14 + 0) * 2
	mov	esi, OffsetTLMessage
	mov	ah, 0Ah
	call	PMDispStr

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
