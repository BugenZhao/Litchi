	org	0x7c00
	jmp 	LABEL_ENTRY			; 保证到这里有 3 个字节

%include	"fat12.inc"

BaseOfLoader		equ	0x9000		; Loader 加载基址
OffsetOfLoader		equ	0x100		; Loader 加载偏移

BaseOfStack		equ	0x7c00		; 栈底，向低地址生长

wRootDirSizeForLoop	dw	RootDirSectors	; Root Directory 占用的扇区数，
						; 在循环中会递减至零.
wSectorNo		dw	0		; 要读取的扇区号
bOdd			db	0		; 奇数还是偶数

LoaderFileName		db	"LOADER  LIT", 0
MessageLength		equ	12
MessageTable:
NoLoaderMessage		db	"No loader  X"
LoaderFoundMessage	db	"Loader found"
ReadyMessage		db	"Boot ready !"
HelloMessage		db	"Litchi Boot "


;	找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
;	需要注意的是, 中间需要读 FAT 的扇区到 es:bx 处, 所以函数一开始保存了 es 和 bx
GetFATEntry:
	push	es
	push	bx
	push	ax
	mov	ax, BaseOfLoader; `.
	sub	ax, 0100h	;  | 在 BaseOfLoader 后面留出 4K 空间用于存放 FAT
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
	mov	bx, 0 ; bx <- 0 于是, es:bx = (BaseOfLoader - 100):00
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
	mov	bx, 0x000d				; bh=0x00 (page), bl=0x0d (color)
	pop	dx
	int	10h
	ret



LABEL_ENTRY:
	; 初始化堆栈
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack

.ClearScreen:
	mov	ax, 0x0003
	int	10h					; clear screen

	mov	di, 3					; "Litchi Boot"
	mov	dx, 0x0000				; row:col
	call	EarlyDispStr

.RootSearchStart:
	xor	ah, ah
	xor	dl, dl
	int	13h					; 软驱复位

	mov	word [wSectorNo], SectorNoOfRootDirectory
	jmp	.RootSearchTest

.RootSearchLoop:
; outer loop (sector)
	; start_sector in di, size in cl, buf in es:bx
	mov	ax, BaseOfLoader
	mov	es, ax
	mov	ax, OffsetOfLoader
	mov	bx, ax					; 临时把根目录文件信息放在这里
	mov	ax, [wSectorNo]				; Sector 序号 初始(0)
	mov	cl, 1
	call	ReadSector				; 读取根目录的一个 Sector
	mov	si, LoaderFileName
	mov	di, OffsetOfLoader
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
	mov	si, LoaderFileName
	jmp	.SectorSearchStart

.RootSearchUpdate:
.NextSector:
	add	word [wSectorNo], 1
	dec	word [wRootDirSizeForLoop]

.RootSearchTest:
	cmp	word [wRootDirSizeForLoop], 0
	je	.NoLoader
	jmp	.RootSearchLoop


.Result:
.FileNameFound:
	push	di
	push	es
	mov	di, 1					; "Loader found"
	mov	dx, 0x0100				; row:col
	call	EarlyDispStr
	pop	es
	pop	di
.LoadFile:
	mov	ax, RootDirSectors
	and	di, 0FFE0h
	add	di, 01Ah				; Loader 的首个 Sector 地址
	mov	cx, word [es:di]
	cmp	cx, 0					; Loader 为空文件
	je	.NoLoader
	push	cx					; 保存此 Sector 在 FAT 中的序号
	add	cx, ax
	add	cx, DeltaSectorNo			; cl <- LOADER.BIN的起始扇区号(0-based)
	mov	ax, BaseOfLoader
	mov	es, ax					; es <- BaseOfLoader
	mov	bx, OffsetOfLoader			; bx <- OffsetOfLoader
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
	mov	dx, 0x0200				; row:col
	call	EarlyDispStr
	jmp	BaseOfLoader:OffsetOfLoader		; GO TO LOADER !!!

.NoLoader:
	mov	di, 0					; "No loader"
	mov	dx, 0x010C				; row:col
	call	EarlyDispStr
Fin:
	hlt
	jmp	Fin
