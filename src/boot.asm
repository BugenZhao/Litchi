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

wRootDirSizeForLoop	dw	RootDirSectors	; Root Directory 占用的扇区数，
						; 在循环中会递减至零.
wSectorNo		dw	0		; 要读取的扇区号
bOdd			db	0		; 奇数还是偶数

LoaderFileName		db	"LOADER  LIT", 0

MessageBegin:
HelloMessage:		db	"Hello, Litchi!", 0x0d, 0x0a, "BugenZhao 2020", 0x0d, 0x0a
BootMessage:		db	"Booting...", 0x0d, 0x0a
NoLoaderMessage:	db	"Fatal: no loader.", 0x0d, 0x0a
LoaderFoundMessage:	db	"Loader found!", 0x0d, 0x0a
MessageEnd:



ClearScreen:
	mov	ax, 0x0003
	int	10h					; clear screen
	ret

DispStr:
	; message in di, end in si, row in dh
	mov	ax, ds
	mov	es, ax
	mov	bp, di					; es:bp -> string address
	mov	cx, si
	sub	cx, di					; calculate string length
	mov	ax, 0x1301				; ah=0x13 (write string)
	mov	bx, 0x000d				; bh=0x00 (page), bl=0x0d (color)
	mov	dl, 0					; col = 0
	int	10h
	ret

ReadSector:
	; start_sector in di, size in cl, buf in es:bx
	; 扇区号 x 除以 18（每个磁道的扇区），商 q，余 r
	; 柱面 q >> 1, 磁头 q & 1, 起始扇区 r + 1
	push	bp
	mov	bp, sp					; 保存栈指针
	sub	esp, 2					; 在栈上开辟 2 个字节
	mov	byte [bp-2], cl
	mov	ax, di					; 构造被除数
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


LABEL_ENTRY:
	; 初始化堆栈
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack

	call	ClearScreen
	
	mov	di, HelloMessage			; string
	mov	si, NoLoaderMessage			; string end
	mov	dh, 0					; row
	call	DispStr

	xor	ah, ah
	xor	dl, dl
	int	13h					; 软驱复位

	mov	word [wSectorNo], SectorNoOfRootDirectory
	jmp	.RootSearchTest

.RootSearchLoop:
	; start_sector in di, size in cl, buf in es:bx
	mov	ax, BaseOfLoader
	mov	es, ax
	mov	ax, OffsetOfLoader
	mov	bx, ax					; 临时把根目录文件信息放在这里
	mov	di, [wSectorNo]
	mov	cl, 1
	call	ReadSector				; 读取根目录

	mov	si, LoaderFileName
	mov	di, OffsetOfLoader
	cld
	mov	dx, 10h					; 该扇区条目计数器
.RootSearchStart:
	cmp	dx, 0
	je	.NextSector				; 该扇区已经搜索完
	dec	dx
	mov	cx, 11					; 文件名比较计数器
.CompareFileName:
	cmp	cx, 0
	je	.FileNameFound				; 字符全部匹配
	dec	cx
	lodsb						; [ds:si] -> al
	cmp 	al, byte [es:di]
	jne	.FileNameDifferent
	inc	di
	jmp	.CompareFileName
.FileNameDifferent:
	and	di, 0ffe0h				; di 指向条目开头
	add	di, 20h					; 下一个条目
	mov	si, LoaderFileName
	jmp	.RootSearchStart

.NextSector:
	add	word [wSectorNo], 1

.RootSearchUpdate:
	dec	word [wRootDirSizeForLoop]
.RootSearchTest:
	cmp	word [wRootDirSizeForLoop], 0
	je	.NoLoader
	jmp	.RootSearchLoop

.NoLoader:
	mov	di, NoLoaderMessage
	mov	si, LoaderFoundMessage
	mov	dh, 8
	call	DispStr
	jmp	Fin

.FileNameFound:
	mov	di, LoaderFoundMessage
	mov	si, MessageEnd
	mov	dh, 9					; row
	call	DispStr
	jmp	Fin

Fin:
	hlt
	jmp	Fin
