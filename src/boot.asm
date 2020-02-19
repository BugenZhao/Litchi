	org	0x7c00
	jmp 	LABEL_ENTRY			; 保证到这里有 3 个字节

; Standard FAT12 floppy code
BS_OEMName		DB 	'BZLITCHI'	; OEM String, 必须 8 个字节
BPB_BytsPerSec		DW 	512		; 每扇区字节数
BPB_SecPerClus		DB 	1		; 每簇多少扇区
BPB_RsvdSecCnt		DW 	1		; Boot 记录占用多少扇区
BPB_NumFATs		DB 	2		; 共有多少 FAT 表
BPB_RootEntCnt		DW 	224		; 根目录文件数最大值
BPB_TotSec16		DW 	2880		; 逻辑扇区总数
BPB_Media		DB 	0xF0		; 媒体描述符
BPB_FATSz16		DW 	9		; 每FAT扇区数
BPB_SecPerTrk		DW 	18		; 每磁道扇区数
BPB_NumHeads		DW 	2		; 磁头数(面数)
BPB_HiddSec		DD 	0		; 隐藏扇区数
BPB_TotSec32		DD 	2880		; 如果 wTotalSectorCount 是 0 由这个值记录扇区数
BS_DrvNum		DB 	0		; 中断 13 的驱动器号
BS_Reserved1		DB 	0		; 未使用
BS_BootSig		DB 	29h		; 扩展引导标记 (29h)
BS_VolID		DD 	0xffffffff	; 卷序列号
BS_VolLab		DB 	"BZLITCHIOS "	; 卷标, 必须 11 个字节
BS_FileSysType		DB 	"FAT12   "	; 文件系统类型, 必须 8个字节  


BaseOfStack		equ	0x7c00		; 栈底，向低地址生长
BaseOfLoader		equ	0x9000		; Loader 加载基址
OffsetOfLoader		equ	0x100		; Loader 加载偏移
RootDirSectors		equ	14		; 根据根目录 224 项得到
SectorNoOfRootDirectory	equ	19		; 根目录开始扇区号，第二个 FAT 表
SectorNoOfFAT1		equ	1		; FAT1 的第一个扇区号 = BPB_RsvdSecCnt
DeltaSectorNo		equ	17		; DeltaSectorNo = BPB_RsvdSecCnt + (BPB_NumFATs * FATSz) - 2
						; 文件的开始Sector号 = DirEntry中的开始Sector号 + 根目录占用Sector数目 + DeltaSectorNo

wRootDirSizeForLoop	dw	RootDirSectors	; Root Directory 占用的扇区数，
						; 在循环中会递减至零.
wSectorNo		dw	0		; 要读取的扇区号
bOdd			db	0		; 奇数还是偶数

LoaderFileName		db	"LOADER  LIT", 0
MessageLength		equ	12
MessageTable:
; HelloMessage:		db	"Hello, Litchi!", 0x0d, 0x0a, "BugenZhao 2020", 0x0d, 0x0a
; HelloMessageEnd:
NoLoaderMessage		db	"No loader   "
LoaderFoundMessage	db	"Loader found"
ReadyMessage		db	"Ready       "
HelloMessage		db	"Bugen Litchi"



DispStr:
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

ReadSector:
	; start_sector in ax, size in cl, buf in es:bx
	; 扇区号 x 除以 18（每个磁道的扇区），商 q，余 r
	; 柱面 q >> 1, 磁头 q & 1, 起始扇区 r + 1
	push	bp
	mov	bp, sp					; 保存栈指针
	sub	esp, 2					; 在栈上开辟 2 个字节
	mov	byte [bp-2], cl
	push	bx
	mov	bl, [BPB_SecPerTrk]			; 构造除数 18
	div	bl					; ax/bl (8 bits), q in al, r in ah
	inc	ah					; r + 1
	mov	cl, ah					; cl -> 起始扇区号
	mov	dh, al					
	and	dh, 1					; dh -> 磁头
	mov	ch, al
	shr	ch, 1					; ch -> 柱面
	pop	bx
	mov	dl, [BS_DrvNum]				; dl -> 驱动器号
.DoRead:
	mov	al, byte [bp-2]				; al -> 要读扇区数
	mov	ah, 2					; 要读了！
	int	13h					; 读！错误时 CF 置 1
	jc	.DoRead					; 错误重试

	add	esp, 2
	pop	bp
	ret

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


LABEL_ENTRY:
	; 初始化堆栈
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack

ClearScreen:
	mov	ax, 0x0003
	int	10h					; clear screen

	mov	di, 3					; "Litchi"
	mov	dx, 0x0000				; row
	call	DispStr

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
.NoLoader:
	mov	di, 0					; "No loader"
	mov	dx, 0x0300				; col:row
	call	DispStr
	jmp	Fin

.FileNameFound:
	push	di
	push	es
	mov	di, 1					; "Loader found"
	mov	dx, 0x0400				; row:col
	call	DispStr
	pop	es
	pop	di
.LoadFile:
	mov	ax, RootDirSectors
	and	di, 0FFE0h
	add	di, 01Ah				; Loader 的首个 Sector 地址
	mov	cx, word [es:di]
	cmp	cx, 0					; Loader 为空
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
	cmp	ax, 0FFFh
	jz	.FileLoaded
	push	ax					; 保存 Sector 在 FAT 中的序号
	mov	dx, RootDirSectors
	add	ax, dx
	add	ax, DeltaSectorNo
	add	bx, [BPB_BytsPerSec]
	jmp	.LoadLoop

.FileLoaded:
	mov	di, 2					; "Ready"
	mov	dx, 0x0500				; row:col
	call	DispStr

	jmp	BaseOfLoader:OffsetOfLoader		; GO TO LOADER !!!
Fin:
	hlt
	jmp	Fin
