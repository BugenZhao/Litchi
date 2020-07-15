#include <include/memlayout.h>
# IRQ general handler, call trap()

#define PUSHA               \
    subq $120,%rsp;         \
    movq %rax,112(%rsp);    \
    movq %rbx,104(%rsp);    \
    movq %rcx,96(%rsp);     \
    movq %rdx,88(%rsp);     \
    movq %rbp,80(%rsp);     \
    movq %rdi,72(%rsp);     \
    movq %rsi,64(%rsp);     \
    movq %r8,56(%rsp);      \
    movq %r9,48(%rsp);      \
    movq %r10,40(%rsp);     \
    movq %r11,32(%rsp);     \
    movq %r12,24(%rsp);     \
    movq %r13,16(%rsp);     \
    movq %r14,8(%rsp);      \
    movq %r15,0(%rsp);

.globl irq_handler_common
irq_handler_common:
    subq $24, %rsp
    movq %rax, 0(%rsp)  # save %rax first
    xorq %rax, %rax
    movw %ds, %ax
    movq %rax, 16(%rsp) # save tf, see trap::Frame
    movw %es, %ax
    movq %rax, 8(%rsp)
    popq %rax           # restore %rax
    PUSHA
    xorq %rax, %rax
    movw $GD_KD, %ax    # set kernel data segment
    movw %ax, %ds
    movw %ax, %es
    movq %rsp, %rdi     # Frame => the first arg
    call trap           # will not return
    

# IRQ handlers

# irq_3:
#    [pushq $0]         # CPU may push a error code, push this 0 (or not) to keep the same format
#     pushq $3          # IRQ number, for common handler use
#     jmp irq_handler_common

.globl irq_0
irq_0:
    pushq $0
    pushq $0
    jmp irq_handler_common

.globl irq_1
irq_1:
    pushq $0
    pushq $1
    jmp irq_handler_common

.globl irq_2
irq_2:
    pushq $0
    pushq $2
    jmp irq_handler_common

.globl irq_3
irq_3:
    pushq $0
    pushq $3
    jmp irq_handler_common

.globl irq_4
irq_4:
    pushq $0
    pushq $4
    jmp irq_handler_common

.globl irq_5
irq_5:
    pushq $0
    pushq $5
    jmp irq_handler_common

.globl irq_6
irq_6:
    pushq $0
    pushq $6
    jmp irq_handler_common

.globl irq_7
irq_7:
    pushq $0
    pushq $7
    jmp irq_handler_common

.globl irq_8
irq_8:
    pushq $8
    jmp irq_handler_common

.globl irq_9
irq_9:
    pushq $0
    pushq $9
    jmp irq_handler_common

.globl irq_10
irq_10:
    pushq $10
    jmp irq_handler_common

.globl irq_11
irq_11:
    pushq $11
    jmp irq_handler_common

.globl irq_12
irq_12:
    pushq $12
    jmp irq_handler_common

.globl irq_13
irq_13:
    pushq $13
    jmp irq_handler_common

.globl irq_14
irq_14:
    pushq $14
    jmp irq_handler_common

.globl irq_15
irq_15:
    pushq $0
    pushq $15
    jmp irq_handler_common

.globl irq_16
irq_16:
    pushq $0
    pushq $16
    jmp irq_handler_common

.globl irq_17
irq_17:
    pushq $17
    jmp irq_handler_common

.globl irq_18
irq_18:
    pushq $0
    pushq $18
    jmp irq_handler_common

.globl irq_19
irq_19:
    pushq $0
    pushq $19
    jmp irq_handler_common

.globl irq_20
irq_20:
    pushq $0
    pushq $20
    jmp irq_handler_common

.globl irq_21
irq_21:
    pushq $0
    pushq $21
    jmp irq_handler_common

.globl irq_22
irq_22:
    pushq $0
    pushq $22
    jmp irq_handler_common

.globl irq_23
irq_23:
    pushq $0
    pushq $23
    jmp irq_handler_common

.globl irq_24
irq_24:
    pushq $0
    pushq $24
    jmp irq_handler_common

.globl irq_25
irq_25:
    pushq $0
    pushq $25
    jmp irq_handler_common

.globl irq_26
irq_26:
    pushq $0
    pushq $26
    jmp irq_handler_common

.globl irq_27
irq_27:
    pushq $0
    pushq $27
    jmp irq_handler_common

.globl irq_28
irq_28:
    pushq $0
    pushq $28
    jmp irq_handler_common

.globl irq_29
irq_29:
    pushq $0
    pushq $29
    jmp irq_handler_common

.globl irq_30
irq_30:
    pushq $0
    pushq $30
    jmp irq_handler_common

.globl irq_31
irq_31:
    pushq $0
    pushq $31
    jmp irq_handler_common

.globl irq_32
irq_32:
    pushq $0
    pushq $32
    jmp irq_handler_common

.globl irq_33
irq_33:
    pushq $0
    pushq $33
    jmp irq_handler_common

.globl irq_34
irq_34:
    pushq $0
    pushq $34
    jmp irq_handler_common

.globl irq_35
irq_35:
    pushq $0
    pushq $35
    jmp irq_handler_common

.globl irq_36
irq_36:
    pushq $0
    pushq $36
    jmp irq_handler_common

.globl irq_37
irq_37:
    pushq $0
    pushq $37
    jmp irq_handler_common

.globl irq_38
irq_38:
    pushq $0
    pushq $38
    jmp irq_handler_common

.globl irq_39
irq_39:
    pushq $0
    pushq $39
    jmp irq_handler_common

.globl irq_40
irq_40:
    pushq $0
    pushq $40
    jmp irq_handler_common

.globl irq_41
irq_41:
    pushq $0
    pushq $41
    jmp irq_handler_common

.globl irq_42
irq_42:
    pushq $0
    pushq $42
    jmp irq_handler_common

.globl irq_43
irq_43:
    pushq $0
    pushq $43
    jmp irq_handler_common

.globl irq_44
irq_44:
    pushq $0
    pushq $44
    jmp irq_handler_common

.globl irq_45
irq_45:
    pushq $0
    pushq $45
    jmp irq_handler_common

.globl irq_46
irq_46:
    pushq $0
    pushq $46
    jmp irq_handler_common

.globl irq_47
irq_47:
    pushq $0
    pushq $47
    jmp irq_handler_common

.globl irq_48
irq_48:
    pushq $0
    pushq $48
    jmp irq_handler_common

.globl irq_49
irq_49:
    pushq $0
    pushq $49
    jmp irq_handler_common

.globl irq_50
irq_50:
    pushq $0
    pushq $50
    jmp irq_handler_common

.globl irq_51
irq_51:
    pushq $0
    pushq $51
    jmp irq_handler_common

.globl irq_52
irq_52:
    pushq $0
    pushq $52
    jmp irq_handler_common

.globl irq_53
irq_53:
    pushq $0
    pushq $53
    jmp irq_handler_common

.globl irq_54
irq_54:
    pushq $0
    pushq $54
    jmp irq_handler_common

.globl irq_55
irq_55:
    pushq $0
    pushq $55
    jmp irq_handler_common

.globl irq_56
irq_56:
    pushq $0
    pushq $56
    jmp irq_handler_common

.globl irq_57
irq_57:
    pushq $0
    pushq $57
    jmp irq_handler_common

.globl irq_58
irq_58:
    pushq $0
    pushq $58
    jmp irq_handler_common

.globl irq_59
irq_59:
    pushq $0
    pushq $59
    jmp irq_handler_common

.globl irq_60
irq_60:
    pushq $0
    pushq $60
    jmp irq_handler_common

.globl irq_61
irq_61:
    pushq $0
    pushq $61
    jmp irq_handler_common

.globl irq_62
irq_62:
    pushq $0
    pushq $62
    jmp irq_handler_common

.globl irq_63
irq_63:
    pushq $0
    pushq $63
    jmp irq_handler_common

.globl irq_64
irq_64:
    pushq $0
    pushq $64
    jmp irq_handler_common

.globl irq_65
irq_65:
    pushq $0
    pushq $65
    jmp irq_handler_common

.globl irq_66
irq_66:
    pushq $0
    pushq $66
    jmp irq_handler_common

.globl irq_67
irq_67:
    pushq $0
    pushq $67
    jmp irq_handler_common

.globl irq_68
irq_68:
    pushq $0
    pushq $68
    jmp irq_handler_common

.globl irq_69
irq_69:
    pushq $0
    pushq $69
    jmp irq_handler_common

.globl irq_70
irq_70:
    pushq $0
    pushq $70
    jmp irq_handler_common

.globl irq_71
irq_71:
    pushq $0
    pushq $71
    jmp irq_handler_common

.globl irq_72
irq_72:
    pushq $0
    pushq $72
    jmp irq_handler_common

.globl irq_73
irq_73:
    pushq $0
    pushq $73
    jmp irq_handler_common

.globl irq_74
irq_74:
    pushq $0
    pushq $74
    jmp irq_handler_common

.globl irq_75
irq_75:
    pushq $0
    pushq $75
    jmp irq_handler_common

.globl irq_76
irq_76:
    pushq $0
    pushq $76
    jmp irq_handler_common

.globl irq_77
irq_77:
    pushq $0
    pushq $77
    jmp irq_handler_common

.globl irq_78
irq_78:
    pushq $0
    pushq $78
    jmp irq_handler_common

.globl irq_79
irq_79:
    pushq $0
    pushq $79
    jmp irq_handler_common

.globl irq_80
irq_80:
    pushq $0
    pushq $80
    jmp irq_handler_common

.globl irq_81
irq_81:
    pushq $0
    pushq $81
    jmp irq_handler_common

.globl irq_82
irq_82:
    pushq $0
    pushq $82
    jmp irq_handler_common

.globl irq_83
irq_83:
    pushq $0
    pushq $83
    jmp irq_handler_common

.globl irq_84
irq_84:
    pushq $0
    pushq $84
    jmp irq_handler_common

.globl irq_85
irq_85:
    pushq $0
    pushq $85
    jmp irq_handler_common

.globl irq_86
irq_86:
    pushq $0
    pushq $86
    jmp irq_handler_common

.globl irq_87
irq_87:
    pushq $0
    pushq $87
    jmp irq_handler_common

.globl irq_88
irq_88:
    pushq $0
    pushq $88
    jmp irq_handler_common

.globl irq_89
irq_89:
    pushq $0
    pushq $89
    jmp irq_handler_common

.globl irq_90
irq_90:
    pushq $0
    pushq $90
    jmp irq_handler_common

.globl irq_91
irq_91:
    pushq $0
    pushq $91
    jmp irq_handler_common

.globl irq_92
irq_92:
    pushq $0
    pushq $92
    jmp irq_handler_common

.globl irq_93
irq_93:
    pushq $0
    pushq $93
    jmp irq_handler_common

.globl irq_94
irq_94:
    pushq $0
    pushq $94
    jmp irq_handler_common

.globl irq_95
irq_95:
    pushq $0
    pushq $95
    jmp irq_handler_common

.globl irq_96
irq_96:
    pushq $0
    pushq $96
    jmp irq_handler_common

.globl irq_97
irq_97:
    pushq $0
    pushq $97
    jmp irq_handler_common

.globl irq_98
irq_98:
    pushq $0
    pushq $98
    jmp irq_handler_common

.globl irq_99
irq_99:
    pushq $0
    pushq $99
    jmp irq_handler_common

.globl irq_100
irq_100:
    pushq $0
    pushq $100
    jmp irq_handler_common

.globl irq_101
irq_101:
    pushq $0
    pushq $101
    jmp irq_handler_common

.globl irq_102
irq_102:
    pushq $0
    pushq $102
    jmp irq_handler_common

.globl irq_103
irq_103:
    pushq $0
    pushq $103
    jmp irq_handler_common

.globl irq_104
irq_104:
    pushq $0
    pushq $104
    jmp irq_handler_common

.globl irq_105
irq_105:
    pushq $0
    pushq $105
    jmp irq_handler_common

.globl irq_106
irq_106:
    pushq $0
    pushq $106
    jmp irq_handler_common

.globl irq_107
irq_107:
    pushq $0
    pushq $107
    jmp irq_handler_common

.globl irq_108
irq_108:
    pushq $0
    pushq $108
    jmp irq_handler_common

.globl irq_109
irq_109:
    pushq $0
    pushq $109
    jmp irq_handler_common

.globl irq_110
irq_110:
    pushq $0
    pushq $110
    jmp irq_handler_common

.globl irq_111
irq_111:
    pushq $0
    pushq $111
    jmp irq_handler_common

.globl irq_112
irq_112:
    pushq $0
    pushq $112
    jmp irq_handler_common

.globl irq_113
irq_113:
    pushq $0
    pushq $113
    jmp irq_handler_common

.globl irq_114
irq_114:
    pushq $0
    pushq $114
    jmp irq_handler_common

.globl irq_115
irq_115:
    pushq $0
    pushq $115
    jmp irq_handler_common

.globl irq_116
irq_116:
    pushq $0
    pushq $116
    jmp irq_handler_common

.globl irq_117
irq_117:
    pushq $0
    pushq $117
    jmp irq_handler_common

.globl irq_118
irq_118:
    pushq $0
    pushq $118
    jmp irq_handler_common

.globl irq_119
irq_119:
    pushq $0
    pushq $119
    jmp irq_handler_common

.globl irq_120
irq_120:
    pushq $0
    pushq $120
    jmp irq_handler_common

.globl irq_121
irq_121:
    pushq $0
    pushq $121
    jmp irq_handler_common

.globl irq_122
irq_122:
    pushq $0
    pushq $122
    jmp irq_handler_common

.globl irq_123
irq_123:
    pushq $0
    pushq $123
    jmp irq_handler_common

.globl irq_124
irq_124:
    pushq $0
    pushq $124
    jmp irq_handler_common

.globl irq_125
irq_125:
    pushq $0
    pushq $125
    jmp irq_handler_common

.globl irq_126
irq_126:
    pushq $0
    pushq $126
    jmp irq_handler_common

.globl irq_127
irq_127:
    pushq $0
    pushq $127
    jmp irq_handler_common

.globl irq_128
irq_128:
    pushq $0
    pushq $128
    jmp irq_handler_common

.globl irq_129
irq_129:
    pushq $0
    pushq $129
    jmp irq_handler_common

.globl irq_130
irq_130:
    pushq $0
    pushq $130
    jmp irq_handler_common

.globl irq_131
irq_131:
    pushq $0
    pushq $131
    jmp irq_handler_common

.globl irq_132
irq_132:
    pushq $0
    pushq $132
    jmp irq_handler_common

.globl irq_133
irq_133:
    pushq $0
    pushq $133
    jmp irq_handler_common

.globl irq_134
irq_134:
    pushq $0
    pushq $134
    jmp irq_handler_common

.globl irq_135
irq_135:
    pushq $0
    pushq $135
    jmp irq_handler_common

.globl irq_136
irq_136:
    pushq $0
    pushq $136
    jmp irq_handler_common

.globl irq_137
irq_137:
    pushq $0
    pushq $137
    jmp irq_handler_common

.globl irq_138
irq_138:
    pushq $0
    pushq $138
    jmp irq_handler_common

.globl irq_139
irq_139:
    pushq $0
    pushq $139
    jmp irq_handler_common

.globl irq_140
irq_140:
    pushq $0
    pushq $140
    jmp irq_handler_common

.globl irq_141
irq_141:
    pushq $0
    pushq $141
    jmp irq_handler_common

.globl irq_142
irq_142:
    pushq $0
    pushq $142
    jmp irq_handler_common

.globl irq_143
irq_143:
    pushq $0
    pushq $143
    jmp irq_handler_common

.globl irq_144
irq_144:
    pushq $0
    pushq $144
    jmp irq_handler_common

.globl irq_145
irq_145:
    pushq $0
    pushq $145
    jmp irq_handler_common

.globl irq_146
irq_146:
    pushq $0
    pushq $146
    jmp irq_handler_common

.globl irq_147
irq_147:
    pushq $0
    pushq $147
    jmp irq_handler_common

.globl irq_148
irq_148:
    pushq $0
    pushq $148
    jmp irq_handler_common

.globl irq_149
irq_149:
    pushq $0
    pushq $149
    jmp irq_handler_common

.globl irq_150
irq_150:
    pushq $0
    pushq $150
    jmp irq_handler_common

.globl irq_151
irq_151:
    pushq $0
    pushq $151
    jmp irq_handler_common

.globl irq_152
irq_152:
    pushq $0
    pushq $152
    jmp irq_handler_common

.globl irq_153
irq_153:
    pushq $0
    pushq $153
    jmp irq_handler_common

.globl irq_154
irq_154:
    pushq $0
    pushq $154
    jmp irq_handler_common

.globl irq_155
irq_155:
    pushq $0
    pushq $155
    jmp irq_handler_common

.globl irq_156
irq_156:
    pushq $0
    pushq $156
    jmp irq_handler_common

.globl irq_157
irq_157:
    pushq $0
    pushq $157
    jmp irq_handler_common

.globl irq_158
irq_158:
    pushq $0
    pushq $158
    jmp irq_handler_common

.globl irq_159
irq_159:
    pushq $0
    pushq $159
    jmp irq_handler_common

.globl irq_160
irq_160:
    pushq $0
    pushq $160
    jmp irq_handler_common

.globl irq_161
irq_161:
    pushq $0
    pushq $161
    jmp irq_handler_common

.globl irq_162
irq_162:
    pushq $0
    pushq $162
    jmp irq_handler_common

.globl irq_163
irq_163:
    pushq $0
    pushq $163
    jmp irq_handler_common

.globl irq_164
irq_164:
    pushq $0
    pushq $164
    jmp irq_handler_common

.globl irq_165
irq_165:
    pushq $0
    pushq $165
    jmp irq_handler_common

.globl irq_166
irq_166:
    pushq $0
    pushq $166
    jmp irq_handler_common

.globl irq_167
irq_167:
    pushq $0
    pushq $167
    jmp irq_handler_common

.globl irq_168
irq_168:
    pushq $0
    pushq $168
    jmp irq_handler_common

.globl irq_169
irq_169:
    pushq $0
    pushq $169
    jmp irq_handler_common

.globl irq_170
irq_170:
    pushq $0
    pushq $170
    jmp irq_handler_common

.globl irq_171
irq_171:
    pushq $0
    pushq $171
    jmp irq_handler_common

.globl irq_172
irq_172:
    pushq $0
    pushq $172
    jmp irq_handler_common

.globl irq_173
irq_173:
    pushq $0
    pushq $173
    jmp irq_handler_common

.globl irq_174
irq_174:
    pushq $0
    pushq $174
    jmp irq_handler_common

.globl irq_175
irq_175:
    pushq $0
    pushq $175
    jmp irq_handler_common

.globl irq_176
irq_176:
    pushq $0
    pushq $176
    jmp irq_handler_common

.globl irq_177
irq_177:
    pushq $0
    pushq $177
    jmp irq_handler_common

.globl irq_178
irq_178:
    pushq $0
    pushq $178
    jmp irq_handler_common

.globl irq_179
irq_179:
    pushq $0
    pushq $179
    jmp irq_handler_common

.globl irq_180
irq_180:
    pushq $0
    pushq $180
    jmp irq_handler_common

.globl irq_181
irq_181:
    pushq $0
    pushq $181
    jmp irq_handler_common

.globl irq_182
irq_182:
    pushq $0
    pushq $182
    jmp irq_handler_common

.globl irq_183
irq_183:
    pushq $0
    pushq $183
    jmp irq_handler_common

.globl irq_184
irq_184:
    pushq $0
    pushq $184
    jmp irq_handler_common

.globl irq_185
irq_185:
    pushq $0
    pushq $185
    jmp irq_handler_common

.globl irq_186
irq_186:
    pushq $0
    pushq $186
    jmp irq_handler_common

.globl irq_187
irq_187:
    pushq $0
    pushq $187
    jmp irq_handler_common

.globl irq_188
irq_188:
    pushq $0
    pushq $188
    jmp irq_handler_common

.globl irq_189
irq_189:
    pushq $0
    pushq $189
    jmp irq_handler_common

.globl irq_190
irq_190:
    pushq $0
    pushq $190
    jmp irq_handler_common

.globl irq_191
irq_191:
    pushq $0
    pushq $191
    jmp irq_handler_common

.globl irq_192
irq_192:
    pushq $0
    pushq $192
    jmp irq_handler_common

.globl irq_193
irq_193:
    pushq $0
    pushq $193
    jmp irq_handler_common

.globl irq_194
irq_194:
    pushq $0
    pushq $194
    jmp irq_handler_common

.globl irq_195
irq_195:
    pushq $0
    pushq $195
    jmp irq_handler_common

.globl irq_196
irq_196:
    pushq $0
    pushq $196
    jmp irq_handler_common

.globl irq_197
irq_197:
    pushq $0
    pushq $197
    jmp irq_handler_common

.globl irq_198
irq_198:
    pushq $0
    pushq $198
    jmp irq_handler_common

.globl irq_199
irq_199:
    pushq $0
    pushq $199
    jmp irq_handler_common

.globl irq_200
irq_200:
    pushq $0
    pushq $200
    jmp irq_handler_common

.globl irq_201
irq_201:
    pushq $0
    pushq $201
    jmp irq_handler_common

.globl irq_202
irq_202:
    pushq $0
    pushq $202
    jmp irq_handler_common

.globl irq_203
irq_203:
    pushq $0
    pushq $203
    jmp irq_handler_common

.globl irq_204
irq_204:
    pushq $0
    pushq $204
    jmp irq_handler_common

.globl irq_205
irq_205:
    pushq $0
    pushq $205
    jmp irq_handler_common

.globl irq_206
irq_206:
    pushq $0
    pushq $206
    jmp irq_handler_common

.globl irq_207
irq_207:
    pushq $0
    pushq $207
    jmp irq_handler_common

.globl irq_208
irq_208:
    pushq $0
    pushq $208
    jmp irq_handler_common

.globl irq_209
irq_209:
    pushq $0
    pushq $209
    jmp irq_handler_common

.globl irq_210
irq_210:
    pushq $0
    pushq $210
    jmp irq_handler_common

.globl irq_211
irq_211:
    pushq $0
    pushq $211
    jmp irq_handler_common

.globl irq_212
irq_212:
    pushq $0
    pushq $212
    jmp irq_handler_common

.globl irq_213
irq_213:
    pushq $0
    pushq $213
    jmp irq_handler_common

.globl irq_214
irq_214:
    pushq $0
    pushq $214
    jmp irq_handler_common

.globl irq_215
irq_215:
    pushq $0
    pushq $215
    jmp irq_handler_common

.globl irq_216
irq_216:
    pushq $0
    pushq $216
    jmp irq_handler_common

.globl irq_217
irq_217:
    pushq $0
    pushq $217
    jmp irq_handler_common

.globl irq_218
irq_218:
    pushq $0
    pushq $218
    jmp irq_handler_common

.globl irq_219
irq_219:
    pushq $0
    pushq $219
    jmp irq_handler_common

.globl irq_220
irq_220:
    pushq $0
    pushq $220
    jmp irq_handler_common

.globl irq_221
irq_221:
    pushq $0
    pushq $221
    jmp irq_handler_common

.globl irq_222
irq_222:
    pushq $0
    pushq $222
    jmp irq_handler_common

.globl irq_223
irq_223:
    pushq $0
    pushq $223
    jmp irq_handler_common

.globl irq_224
irq_224:
    pushq $0
    pushq $224
    jmp irq_handler_common

.globl irq_225
irq_225:
    pushq $0
    pushq $225
    jmp irq_handler_common

.globl irq_226
irq_226:
    pushq $0
    pushq $226
    jmp irq_handler_common

.globl irq_227
irq_227:
    pushq $0
    pushq $227
    jmp irq_handler_common

.globl irq_228
irq_228:
    pushq $0
    pushq $228
    jmp irq_handler_common

.globl irq_229
irq_229:
    pushq $0
    pushq $229
    jmp irq_handler_common

.globl irq_230
irq_230:
    pushq $0
    pushq $230
    jmp irq_handler_common

.globl irq_231
irq_231:
    pushq $0
    pushq $231
    jmp irq_handler_common

.globl irq_232
irq_232:
    pushq $0
    pushq $232
    jmp irq_handler_common

.globl irq_233
irq_233:
    pushq $0
    pushq $233
    jmp irq_handler_common

.globl irq_234
irq_234:
    pushq $0
    pushq $234
    jmp irq_handler_common

.globl irq_235
irq_235:
    pushq $0
    pushq $235
    jmp irq_handler_common

.globl irq_236
irq_236:
    pushq $0
    pushq $236
    jmp irq_handler_common

.globl irq_237
irq_237:
    pushq $0
    pushq $237
    jmp irq_handler_common

.globl irq_238
irq_238:
    pushq $0
    pushq $238
    jmp irq_handler_common

.globl irq_239
irq_239:
    pushq $0
    pushq $239
    jmp irq_handler_common

.globl irq_240
irq_240:
    pushq $0
    pushq $240
    jmp irq_handler_common

.globl irq_241
irq_241:
    pushq $0
    pushq $241
    jmp irq_handler_common

.globl irq_242
irq_242:
    pushq $0
    pushq $242
    jmp irq_handler_common

.globl irq_243
irq_243:
    pushq $0
    pushq $243
    jmp irq_handler_common

.globl irq_244
irq_244:
    pushq $0
    pushq $244
    jmp irq_handler_common

.globl irq_245
irq_245:
    pushq $0
    pushq $245
    jmp irq_handler_common

.globl irq_246
irq_246:
    pushq $0
    pushq $246
    jmp irq_handler_common

.globl irq_247
irq_247:
    pushq $0
    pushq $247
    jmp irq_handler_common

.globl irq_248
irq_248:
    pushq $0
    pushq $248
    jmp irq_handler_common

.globl irq_249
irq_249:
    pushq $0
    pushq $249
    jmp irq_handler_common

.globl irq_250
irq_250:
    pushq $0
    pushq $250
    jmp irq_handler_common

.globl irq_251
irq_251:
    pushq $0
    pushq $251
    jmp irq_handler_common

.globl irq_252
irq_252:
    pushq $0
    pushq $252
    jmp irq_handler_common

.globl irq_253
irq_253:
    pushq $0
    pushq $253
    jmp irq_handler_common

.globl irq_254
irq_254:
    pushq $0
    pushq $254
    jmp irq_handler_common

.globl irq_255
irq_255:
    pushq $0
    pushq $255
    jmp irq_handler_common


# Interrupt Vector Table

.data
.globl ivt
ivt:
    .quad irq_0
    .quad irq_1
    .quad irq_2
    .quad irq_3
    .quad irq_4
    .quad irq_5
    .quad irq_6
    .quad irq_7
    .quad irq_8
    .quad irq_9
    .quad irq_10
    .quad irq_11
    .quad irq_12
    .quad irq_13
    .quad irq_14
    .quad irq_15
    .quad irq_16
    .quad irq_17
    .quad irq_18
    .quad irq_19
    .quad irq_20
    .quad irq_21
    .quad irq_22
    .quad irq_23
    .quad irq_24
    .quad irq_25
    .quad irq_26
    .quad irq_27
    .quad irq_28
    .quad irq_29
    .quad irq_30
    .quad irq_31
    .quad irq_32
    .quad irq_33
    .quad irq_34
    .quad irq_35
    .quad irq_36
    .quad irq_37
    .quad irq_38
    .quad irq_39
    .quad irq_40
    .quad irq_41
    .quad irq_42
    .quad irq_43
    .quad irq_44
    .quad irq_45
    .quad irq_46
    .quad irq_47
    .quad irq_48
    .quad irq_49
    .quad irq_50
    .quad irq_51
    .quad irq_52
    .quad irq_53
    .quad irq_54
    .quad irq_55
    .quad irq_56
    .quad irq_57
    .quad irq_58
    .quad irq_59
    .quad irq_60
    .quad irq_61
    .quad irq_62
    .quad irq_63
    .quad irq_64
    .quad irq_65
    .quad irq_66
    .quad irq_67
    .quad irq_68
    .quad irq_69
    .quad irq_70
    .quad irq_71
    .quad irq_72
    .quad irq_73
    .quad irq_74
    .quad irq_75
    .quad irq_76
    .quad irq_77
    .quad irq_78
    .quad irq_79
    .quad irq_80
    .quad irq_81
    .quad irq_82
    .quad irq_83
    .quad irq_84
    .quad irq_85
    .quad irq_86
    .quad irq_87
    .quad irq_88
    .quad irq_89
    .quad irq_90
    .quad irq_91
    .quad irq_92
    .quad irq_93
    .quad irq_94
    .quad irq_95
    .quad irq_96
    .quad irq_97
    .quad irq_98
    .quad irq_99
    .quad irq_100
    .quad irq_101
    .quad irq_102
    .quad irq_103
    .quad irq_104
    .quad irq_105
    .quad irq_106
    .quad irq_107
    .quad irq_108
    .quad irq_109
    .quad irq_110
    .quad irq_111
    .quad irq_112
    .quad irq_113
    .quad irq_114
    .quad irq_115
    .quad irq_116
    .quad irq_117
    .quad irq_118
    .quad irq_119
    .quad irq_120
    .quad irq_121
    .quad irq_122
    .quad irq_123
    .quad irq_124
    .quad irq_125
    .quad irq_126
    .quad irq_127
    .quad irq_128
    .quad irq_129
    .quad irq_130
    .quad irq_131
    .quad irq_132
    .quad irq_133
    .quad irq_134
    .quad irq_135
    .quad irq_136
    .quad irq_137
    .quad irq_138
    .quad irq_139
    .quad irq_140
    .quad irq_141
    .quad irq_142
    .quad irq_143
    .quad irq_144
    .quad irq_145
    .quad irq_146
    .quad irq_147
    .quad irq_148
    .quad irq_149
    .quad irq_150
    .quad irq_151
    .quad irq_152
    .quad irq_153
    .quad irq_154
    .quad irq_155
    .quad irq_156
    .quad irq_157
    .quad irq_158
    .quad irq_159
    .quad irq_160
    .quad irq_161
    .quad irq_162
    .quad irq_163
    .quad irq_164
    .quad irq_165
    .quad irq_166
    .quad irq_167
    .quad irq_168
    .quad irq_169
    .quad irq_170
    .quad irq_171
    .quad irq_172
    .quad irq_173
    .quad irq_174
    .quad irq_175
    .quad irq_176
    .quad irq_177
    .quad irq_178
    .quad irq_179
    .quad irq_180
    .quad irq_181
    .quad irq_182
    .quad irq_183
    .quad irq_184
    .quad irq_185
    .quad irq_186
    .quad irq_187
    .quad irq_188
    .quad irq_189
    .quad irq_190
    .quad irq_191
    .quad irq_192
    .quad irq_193
    .quad irq_194
    .quad irq_195
    .quad irq_196
    .quad irq_197
    .quad irq_198
    .quad irq_199
    .quad irq_200
    .quad irq_201
    .quad irq_202
    .quad irq_203
    .quad irq_204
    .quad irq_205
    .quad irq_206
    .quad irq_207
    .quad irq_208
    .quad irq_209
    .quad irq_210
    .quad irq_211
    .quad irq_212
    .quad irq_213
    .quad irq_214
    .quad irq_215
    .quad irq_216
    .quad irq_217
    .quad irq_218
    .quad irq_219
    .quad irq_220
    .quad irq_221
    .quad irq_222
    .quad irq_223
    .quad irq_224
    .quad irq_225
    .quad irq_226
    .quad irq_227
    .quad irq_228
    .quad irq_229
    .quad irq_230
    .quad irq_231
    .quad irq_232
    .quad irq_233
    .quad irq_234
    .quad irq_235
    .quad irq_236
    .quad irq_237
    .quad irq_238
    .quad irq_239
    .quad irq_240
    .quad irq_241
    .quad irq_242
    .quad irq_243
    .quad irq_244
    .quad irq_245
    .quad irq_246
    .quad irq_247
    .quad irq_248
    .quad irq_249
    .quad irq_250
    .quad irq_251
    .quad irq_252
    .quad irq_253
    .quad irq_254
    .quad irq_255
