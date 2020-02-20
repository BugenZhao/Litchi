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
BPB_TotSec32		DD 	0		; 如果 wTotalSectorCount 是 0 由这个值记录扇区数
BS_DrvNum		DB 	0		; 中断 13 的驱动器号
BS_Reserved1		DB 	0		; 未使用
BS_BootSig		DB 	29h		; 扩展引导标记 (29h)
BS_VolID		DD 	0xffffffff	; 卷序列号
BS_VolLab		DB 	"BZLITCHIOS "	; 卷标, 必须 11 个字节
BS_FileSysType		DB 	"FAT12   "	; 文件系统类型, 必须 8个字节  


RootDirSectors		equ	14		; 根据根目录 224 项得到
SectorNoOfRootDirectory	equ	19		; 根目录开始扇区号，第二个 FAT 表
SectorNoOfFAT1		equ	1		; FAT1 的第一个扇区号 = BPB_RsvdSecCnt
DeltaSectorNo		equ	17		; DeltaSectorNo = BPB_RsvdSecCnt + (BPB_NumFATs * FATSz) - 2
						; 文件的开始Sector号 = DirEntry中的开始Sector号 + 根目录占用Sector数目 + DeltaSectorNo



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
