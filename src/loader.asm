	org	0x100
	jmp	Entry

%include	"pm.inc"
%include	"fat12.inc"

BaseOfKernel		equ	08000h		; KERNEL 段地址
OffsetOfKernel		equ	0h		; KERNEL 偏移地址
KernelFileName		db	"KERNEL  LIT", 0
wRootDirSizeForLoop	dw	RootDirSectors	; Root Directory 占用的扇区数，
						; 在循环中会递减至零.
wSectorNo		dw	0		; 要读取的扇区号
bOdd			db	0		; 奇数还是偶数

[SECTION .gdt]
; GDT
;                      段基址(稍后初始化),       段界限     , 属性
LABEL_GDT:	   Descriptor        0,                0, 0           		; 空描述符
LABEL_DESC_NORMAL: Descriptor	     0,		  0ffffh, DA_DRW		; Normal, 用于返回实模式
LABEL_DESC_CODE32: Descriptor        0, SegCode32Len - 1, DA_C + DA_32		; 非一致 32 位代码段
;LABEL_DESC_CODE16: Descriptor	     0,           0ffffh, DA_C			; 非一致 16 位代码段
LABEL_DESC_DATA:   Descriptor	     0,      DataLen - 1, DA_DRW		; Data
LABEL_DESC_STACK:  Descriptor	     0,	      TopOfStack, DA_DRWA + DA_32	; Stack
LABEL_DESC_TEST:   Descriptor 0500000h,		  0ffffh, DA_DRW		; Test
LABEL_DESC_VIDEO:  Descriptor  0B8000h,           0ffffh, DA_DRW	    	; 显存首地址
; GDT 结束

GdtLen			equ	$ - LABEL_GDT	; GDT长度
GdtPtr			dw	GdtLen - 1	; GDT界限
			dd	0		; GDT基地址(稍后初始化)

; GDT 选择子
SelectorNormal		equ	LABEL_DESC_NORMAL	- LABEL_GDT
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT
;SelectorCode16		equ	LABEL_DESC_CODE16	- LABEL_GDT
SelectorData		equ	LABEL_DESC_DATA		- LABEL_GDT
SelectorStack		equ	LABEL_DESC_STACK	- LABEL_GDT
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
times	160 		db	" "
times	10		db	"Litchi Long Message Test "
			db	0
TSMessage:		db	"Test stack...", 0

OffsetPMMessage		equ	PMMessage - $$		; PMMessage 相对 Data Section 偏移
OffsetTMMessage		equ	TMMessage - $$
OffsetTLMessage		equ	TLMessage - $$
OffsetLongMessage	equ	LongMessage - $$
OffsetTSMessge		equ	TSMessage - $$

DataLen			equ	$ - LABEL_DATA
; END of [SECTION .data1]



; 全局堆栈段
[SECTION .gs]
ALIGN	32
[BITS	32]
LABEL_STACK:
times	512		db	0

TopOfStack		equ	$ - LABEL_STACK - 1
; END of [SECTION .gs]



[SECTION .s16]
[BITS	16]
HelloMessage:		db	"Hello, Litchi!", 0x0d, 0x0a
HelloMessageEnd:
AboutMessage:		db	"This is the loader of Bugen's tiny OS -- Litchi OS.", 0x0d, 0x0a
			db	"Version v0.1.0, Build 200219", 0x0d, 0x0a
			db	0x0d, 0x0a
			db	"(C) BugenZhao 2020"
AboutMessageEnd:

EarlyMessages:
MessageLength		equ	16
MessageTable:
NoLoaderMessage		db	"No kernel      X"
KernelFoundMessage	db	"Kernel found  ! "
ReadyMessage		db	"Kernel loaded ! "
EmptyKernelMessage	db	"Empty Kernel   X"

EarlyDispStr:
	; message_index in di, row:col in dx
	push	dx					; 坑啊
	mov	ax, ds
	mov	es, ax					; es
	mov	ax, MessageLength
	mov	cx, ax					; calculate string length
	mul	di					; dx:ax
	add	ax, MessageTable
	mov	bp, ax					; es:bp -> string address
	mov	ax, 0x1301				; ah=0x13 (write string)
	mov	bx, 0x000e				; bh=0x00 (page), bl=0x0e (color)
	pop	dx
	int	10h
	ret

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

;	找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
;	需要注意的是, 中间需要读 FAT 的扇区到 es:bx 处, 所以函数一开始保存了 es 和 bx
GetFATEntry:
	push	es
	push	bx
	push	ax
	mov	ax, BaseOfKernel; `.
	sub	ax, 0100h	;  | 在 BaseOfKernel 后面留出 4K 空间用于存放 FAT
	mov	es, ax		; /
	pop	ax
	mov	byte [bOdd], 0
	mov	bx, 3
	mul	bx					; dx:ax = ax * 3
	mov	bx, 2
	div	bx					; dx:ax / 2  ==>  ax <- 商, dx <- 余数
	cmp	dx, 0
	jz	LABEL_EVEN
	mov	byte [bOdd], 1
LABEL_EVEN:;偶数
	; 现在 ax 中是 FATEntry 在 FAT 中的偏移量,下面来
	; 计算 FATEntry 在哪个扇区中(FAT占用不止一个扇区)
	xor	dx, dx			
	mov	bx, [BPB_BytsPerSec]
	div	bx ; dx:ax / BPB_BytsPerSec
		   ;  ax <- 商 (FATEntry 所在的扇区相对于 FAT 的扇区号)
		   ;  dx <- 余数 (FATEntry 在扇区内的偏移)。
	push	dx
	mov	bx, 0 ; bx <- 0 于是, es:bx = (BaseOfKernel - 100):00
	add	ax, SectorNoOfFAT1 ; 此句之后的 ax 就是 FATEntry 所在的扇区号
	mov	cl, 2
	call	ReadSector ; 读取 FATEntry 所在的扇区, 一次读两个, 避免在边界
			   ; 发生错误, 因为一个 FATEntry 可能跨越两个扇区
	pop	dx
	add	bx, dx
	mov	ax, [es:bx]
	cmp	byte [bOdd], 1
	jnz	LABEL_EVEN_2
	shr	ax, 4
LABEL_EVEN_2:
	and	ax, 0FFFh

LABEL_GET_FAT_ENRY_OK:

	pop	bx
	pop	es
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

.RootSearchStart:
	xor	ah, ah
	xor	dl, dl
	int	13h					; 软驱复位

	mov	word [wSectorNo], SectorNoOfRootDirectory
	jmp	.RootSearchTest

.RootSearchLoop:
; outer loop (sector)
	; start_sector in di, size in cl, buf in es:bx
	mov	ax, BaseOfKernel
	mov	es, ax
	mov	ax, OffsetOfKernel
	mov	bx, ax					; 临时把根目录文件信息放在这里
	mov	ax, [wSectorNo]				; Sector 序号 初始(0)
	mov	cl, 1
	call	ReadSector				; 读取根目录的一个 Sector
	mov	si, KernelFileName
	mov	di, OffsetOfKernel
	cld						; clear df flag
	mov	dx, 10h					; 该扇区条目计数器

.SectorSearchStart:
; middle loop (entry)
	cmp	dx, 0
	je	.NextSector				; 该扇区已经搜索完?
	dec	dx
	mov	cx, 11					; 文件名比较计数器
.CompareFileName:
; inner loop (filename)
	cmp	cx, 0
	je	.FileNameFound				; 字符全部匹配?
	dec	cx
	lodsb						; [ds:si] -> al; 由于 df=0, 递减 si 
	cmp 	al, byte [es:di]
	jne	.FileNameDifferent
	inc	di
	jmp	.CompareFileName
.FileNameDifferent:
	and	di, 0ffe0h				; di 指向条目开头
	add	di, 20h					; 下一个条目
	mov	si, KernelFileName
	jmp	.SectorSearchStart

.RootSearchUpdate:
.NextSector:
	add	word [wSectorNo], 1
	dec	word [wRootDirSizeForLoop]

.RootSearchTest:
	cmp	word [wRootDirSizeForLoop], 0
	je	.NoKernel
	jmp	.RootSearchLoop


.Result:
.FileNameFound:
	push	di
	push	es
	mov	di, 1					; "Kernel found"
	mov	dx, 0x0128				; row:col
	call	EarlyDispStr
	pop	es
	pop	di
.LoadFile:
	mov	ax, RootDirSectors
	and	di, 0FFE0h
	add	di, 01Ah				; Kernel 的首个 Sector 地址
	mov	cx, word [es:di]
	cmp	cx, 0					; Kernel 为空文件
	je	.EmptyKernel
	push	cx					; 保存此 Sector 在 FAT 中的序号
	add	cx, ax
	add	cx, DeltaSectorNo			; cl <- Kernel.BIN的起始扇区号(0-based)
	mov	ax, BaseOfKernel
	mov	es, ax					; es <- BaseOfKernel
	mov	bx, OffsetOfKernel			; bx <- OffsetOfKernel
	mov	ax, cx					; ax <- Sector 号
.LoadLoop:
	mov	cl, 1
	call	ReadSector
	pop	ax					; 取出此 Sector 在 FAT 中的序号
	call	GetFATEntry
	cmp	ax, 0FFFh				; 最后一个簇
	jz	.FileLoaded
	push	ax					; 保存 Sector 在 FAT 中的序号
	mov	dx, RootDirSectors
	add	ax, dx
	add	ax, DeltaSectorNo
	add	bx, [BPB_BytsPerSec]
	jmp	.LoadLoop
.FileLoaded:
	mov	di, 2					; "Ready"
	mov	dx, 0x0228				; row:col
	call	EarlyDispStr
	jmp	PreparePM				; 准备进入保护模式！！！

.NoKernel:
	mov	di, 0					; "No kernel"
	mov	dx, 0x0128				; row:col
	call	EarlyDispStr
	jmp	Fin
.EmptyKernel:
	mov	di, 3					; "Empty kernel"
	mov	dx, 0x0138				; row:col
	call	EarlyDispStr
	jmp	Fin

PreparePM:
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
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_DATA
	mov	word [LABEL_DESC_DATA + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_DATA + 4], al
	mov	byte [LABEL_DESC_DATA + 7], ah

	; 初始化 Stack 段描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_STACK
	mov	word [LABEL_DESC_STACK + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_STACK + 4], al
	mov	byte [LABEL_DESC_STACK + 7], ah

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
					; 并跳转到 SelectorCode32:0  处



[SECTION .s32]
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
	mov	gs, ax			; 初始化各选择子，cs已为代码段
	mov	ax, SelectorData
	mov	ds, ax
	mov	ax, SelectorTest
	mov	es, ax
	mov	ax, SelectorStack
	mov	ss, ax
	; ds: Data, gs: Video, es: Test, ss: Stack
	mov	edi, (80 * 12 + 0) * 2	; 定位显存行列
	mov	esi, OffsetPMMessage	; PMMessage 偏移，用于段寻址
	; esi: Data, edi: Video
	mov	ah, 0Ch			; 0000: 黑底    1100: 红字
	call	PMDispStr

.TestMemory:
	mov	edi, (80 * 13 + 0) * 2
	mov	esi, OffsetTMMessage
	mov	ah, 0Bh
	call	PMDispStr
.TestWrite:				; 尝试读写 5MiB 处的内存
	xor	esi, esi
	mov	dx, 0x4b4f
	mov	[es:esi], dx
	; (gdb) x/2cb 0x500000
	; 0x500000:       79 'O'  75 'K'
.TestRead:
	mov	edi, (80 * 13 + 20) * 2
	mov	ah, 0Bh			; color
	mov	al, [es:esi]
	mov	[gs:edi], ax
	mov	al, [es:esi + 1]
	mov	[gs:edi + 2], ax

.TestLongContent:
	mov	edi, (80 * 16 + 0) * 2
	mov	esi, OffsetLongMessage
	mov	ah, 0Ah
	call	PMDispStr
	mov	edi, (80 * 14 + 0) * 2
	mov	esi, OffsetTLMessage
	mov	ah, 0Ah
	call	PMDispStr

.TestStack:
	mov	edi, (80 * 13 + 30) *2
	mov	esi, OffsetTSMessge
	mov	ah, 09h
	call	PMDispStr
	mov	ax, 0x094f		
	mov	dx, 0x094b
	push	dx
	push	ax
	pop	dx
	pop	ax
	mov	edi, (80 * 13 + 48) * 2
	mov	[gs:edi], dx
	mov	[gs:edi + 2], ax

Fin:
	hlt
	jmp	Fin

SegCode32Len	equ	$ - LABEL_SEG_CODE32
; END of [SECTION .s32]
