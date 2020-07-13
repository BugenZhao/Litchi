#include <include/memlayout.h>
# IRQ general handler, call trap()

.globl irq_handler_common
irq_handler_common:
    pushl %ds           # save tf, see trap::Frame
    pushl %es
    pushal
    movw $GD_KD, %ax    # set kernel data segment
    movw %ax, %ds
    movw %ax, %es
    pushl %esp          # above esp => the trap frame
    call trap           # will not return
    

# IRQ handlers

# irq_3:
#    [pushl $0]         # CPU may push a error code, push this 0 (or not) to keep the same format
#     pushl $3          # IRQ number, for common handler use
#     jmp irq_handler_common

.globl irq_0
irq_0:
    pushl $0
    pushl $0
    jmp irq_handler_common

.globl irq_1
irq_1:
    pushl $0
    pushl $1
    jmp irq_handler_common

.globl irq_2
irq_2:
    pushl $0
    pushl $2
    jmp irq_handler_common

.globl irq_3
irq_3:
    pushl $0
    pushl $3
    jmp irq_handler_common

.globl irq_4
irq_4:
    pushl $0
    pushl $4
    jmp irq_handler_common

.globl irq_5
irq_5:
    pushl $0
    pushl $5
    jmp irq_handler_common

.globl irq_6
irq_6:
    pushl $0
    pushl $6
    jmp irq_handler_common

.globl irq_7
irq_7:
    pushl $0
    pushl $7
    jmp irq_handler_common

.globl irq_8
irq_8:
    pushl $8
    jmp irq_handler_common

.globl irq_9
irq_9:
    pushl $0
    pushl $9
    jmp irq_handler_common

.globl irq_10
irq_10:
    pushl $10
    jmp irq_handler_common

.globl irq_11
irq_11:
    pushl $11
    jmp irq_handler_common

.globl irq_12
irq_12:
    pushl $12
    jmp irq_handler_common

.globl irq_13
irq_13:
    pushl $13
    jmp irq_handler_common

.globl irq_14
irq_14:
    pushl $14
    jmp irq_handler_common

.globl irq_15
irq_15:
    pushl $0
    pushl $15
    jmp irq_handler_common

.globl irq_16
irq_16:
    pushl $0
    pushl $16
    jmp irq_handler_common

.globl irq_17
irq_17:
    pushl $17
    jmp irq_handler_common

.globl irq_18
irq_18:
    pushl $0
    pushl $18
    jmp irq_handler_common

.globl irq_19
irq_19:
    pushl $0
    pushl $19
    jmp irq_handler_common

.globl irq_20
irq_20:
    pushl $0
    pushl $20
    jmp irq_handler_common

.globl irq_21
irq_21:
    pushl $0
    pushl $21
    jmp irq_handler_common

.globl irq_22
irq_22:
    pushl $0
    pushl $22
    jmp irq_handler_common

.globl irq_23
irq_23:
    pushl $0
    pushl $23
    jmp irq_handler_common

.globl irq_24
irq_24:
    pushl $0
    pushl $24
    jmp irq_handler_common

.globl irq_25
irq_25:
    pushl $0
    pushl $25
    jmp irq_handler_common

.globl irq_26
irq_26:
    pushl $0
    pushl $26
    jmp irq_handler_common

.globl irq_27
irq_27:
    pushl $0
    pushl $27
    jmp irq_handler_common

.globl irq_28
irq_28:
    pushl $0
    pushl $28
    jmp irq_handler_common

.globl irq_29
irq_29:
    pushl $0
    pushl $29
    jmp irq_handler_common

.globl irq_30
irq_30:
    pushl $0
    pushl $30
    jmp irq_handler_common

.globl irq_31
irq_31:
    pushl $0
    pushl $31
    jmp irq_handler_common

.globl irq_32
irq_32:
    pushl $0
    pushl $32
    jmp irq_handler_common

.globl irq_33
irq_33:
    pushl $0
    pushl $33
    jmp irq_handler_common

.globl irq_34
irq_34:
    pushl $0
    pushl $34
    jmp irq_handler_common

.globl irq_35
irq_35:
    pushl $0
    pushl $35
    jmp irq_handler_common

.globl irq_36
irq_36:
    pushl $0
    pushl $36
    jmp irq_handler_common

.globl irq_37
irq_37:
    pushl $0
    pushl $37
    jmp irq_handler_common

.globl irq_38
irq_38:
    pushl $0
    pushl $38
    jmp irq_handler_common

.globl irq_39
irq_39:
    pushl $0
    pushl $39
    jmp irq_handler_common

.globl irq_40
irq_40:
    pushl $0
    pushl $40
    jmp irq_handler_common

.globl irq_41
irq_41:
    pushl $0
    pushl $41
    jmp irq_handler_common

.globl irq_42
irq_42:
    pushl $0
    pushl $42
    jmp irq_handler_common

.globl irq_43
irq_43:
    pushl $0
    pushl $43
    jmp irq_handler_common

.globl irq_44
irq_44:
    pushl $0
    pushl $44
    jmp irq_handler_common

.globl irq_45
irq_45:
    pushl $0
    pushl $45
    jmp irq_handler_common

.globl irq_46
irq_46:
    pushl $0
    pushl $46
    jmp irq_handler_common

.globl irq_47
irq_47:
    pushl $0
    pushl $47
    jmp irq_handler_common

.globl irq_48
irq_48:
    pushl $0
    pushl $48
    jmp irq_handler_common

.globl irq_49
irq_49:
    pushl $0
    pushl $49
    jmp irq_handler_common

.globl irq_50
irq_50:
    pushl $0
    pushl $50
    jmp irq_handler_common

.globl irq_51
irq_51:
    pushl $0
    pushl $51
    jmp irq_handler_common

.globl irq_52
irq_52:
    pushl $0
    pushl $52
    jmp irq_handler_common

.globl irq_53
irq_53:
    pushl $0
    pushl $53
    jmp irq_handler_common

.globl irq_54
irq_54:
    pushl $0
    pushl $54
    jmp irq_handler_common

.globl irq_55
irq_55:
    pushl $0
    pushl $55
    jmp irq_handler_common

.globl irq_56
irq_56:
    pushl $0
    pushl $56
    jmp irq_handler_common

.globl irq_57
irq_57:
    pushl $0
    pushl $57
    jmp irq_handler_common

.globl irq_58
irq_58:
    pushl $0
    pushl $58
    jmp irq_handler_common

.globl irq_59
irq_59:
    pushl $0
    pushl $59
    jmp irq_handler_common

.globl irq_60
irq_60:
    pushl $0
    pushl $60
    jmp irq_handler_common

.globl irq_61
irq_61:
    pushl $0
    pushl $61
    jmp irq_handler_common

.globl irq_62
irq_62:
    pushl $0
    pushl $62
    jmp irq_handler_common

.globl irq_63
irq_63:
    pushl $0
    pushl $63
    jmp irq_handler_common

.globl irq_64
irq_64:
    pushl $0
    pushl $64
    jmp irq_handler_common

.globl irq_65
irq_65:
    pushl $0
    pushl $65
    jmp irq_handler_common

.globl irq_66
irq_66:
    pushl $0
    pushl $66
    jmp irq_handler_common

.globl irq_67
irq_67:
    pushl $0
    pushl $67
    jmp irq_handler_common

.globl irq_68
irq_68:
    pushl $0
    pushl $68
    jmp irq_handler_common

.globl irq_69
irq_69:
    pushl $0
    pushl $69
    jmp irq_handler_common

.globl irq_70
irq_70:
    pushl $0
    pushl $70
    jmp irq_handler_common

.globl irq_71
irq_71:
    pushl $0
    pushl $71
    jmp irq_handler_common

.globl irq_72
irq_72:
    pushl $0
    pushl $72
    jmp irq_handler_common

.globl irq_73
irq_73:
    pushl $0
    pushl $73
    jmp irq_handler_common

.globl irq_74
irq_74:
    pushl $0
    pushl $74
    jmp irq_handler_common

.globl irq_75
irq_75:
    pushl $0
    pushl $75
    jmp irq_handler_common

.globl irq_76
irq_76:
    pushl $0
    pushl $76
    jmp irq_handler_common

.globl irq_77
irq_77:
    pushl $0
    pushl $77
    jmp irq_handler_common

.globl irq_78
irq_78:
    pushl $0
    pushl $78
    jmp irq_handler_common

.globl irq_79
irq_79:
    pushl $0
    pushl $79
    jmp irq_handler_common

.globl irq_80
irq_80:
    pushl $0
    pushl $80
    jmp irq_handler_common

.globl irq_81
irq_81:
    pushl $0
    pushl $81
    jmp irq_handler_common

.globl irq_82
irq_82:
    pushl $0
    pushl $82
    jmp irq_handler_common

.globl irq_83
irq_83:
    pushl $0
    pushl $83
    jmp irq_handler_common

.globl irq_84
irq_84:
    pushl $0
    pushl $84
    jmp irq_handler_common

.globl irq_85
irq_85:
    pushl $0
    pushl $85
    jmp irq_handler_common

.globl irq_86
irq_86:
    pushl $0
    pushl $86
    jmp irq_handler_common

.globl irq_87
irq_87:
    pushl $0
    pushl $87
    jmp irq_handler_common

.globl irq_88
irq_88:
    pushl $0
    pushl $88
    jmp irq_handler_common

.globl irq_89
irq_89:
    pushl $0
    pushl $89
    jmp irq_handler_common

.globl irq_90
irq_90:
    pushl $0
    pushl $90
    jmp irq_handler_common

.globl irq_91
irq_91:
    pushl $0
    pushl $91
    jmp irq_handler_common

.globl irq_92
irq_92:
    pushl $0
    pushl $92
    jmp irq_handler_common

.globl irq_93
irq_93:
    pushl $0
    pushl $93
    jmp irq_handler_common

.globl irq_94
irq_94:
    pushl $0
    pushl $94
    jmp irq_handler_common

.globl irq_95
irq_95:
    pushl $0
    pushl $95
    jmp irq_handler_common

.globl irq_96
irq_96:
    pushl $0
    pushl $96
    jmp irq_handler_common

.globl irq_97
irq_97:
    pushl $0
    pushl $97
    jmp irq_handler_common

.globl irq_98
irq_98:
    pushl $0
    pushl $98
    jmp irq_handler_common

.globl irq_99
irq_99:
    pushl $0
    pushl $99
    jmp irq_handler_common

.globl irq_100
irq_100:
    pushl $0
    pushl $100
    jmp irq_handler_common

.globl irq_101
irq_101:
    pushl $0
    pushl $101
    jmp irq_handler_common

.globl irq_102
irq_102:
    pushl $0
    pushl $102
    jmp irq_handler_common

.globl irq_103
irq_103:
    pushl $0
    pushl $103
    jmp irq_handler_common

.globl irq_104
irq_104:
    pushl $0
    pushl $104
    jmp irq_handler_common

.globl irq_105
irq_105:
    pushl $0
    pushl $105
    jmp irq_handler_common

.globl irq_106
irq_106:
    pushl $0
    pushl $106
    jmp irq_handler_common

.globl irq_107
irq_107:
    pushl $0
    pushl $107
    jmp irq_handler_common

.globl irq_108
irq_108:
    pushl $0
    pushl $108
    jmp irq_handler_common

.globl irq_109
irq_109:
    pushl $0
    pushl $109
    jmp irq_handler_common

.globl irq_110
irq_110:
    pushl $0
    pushl $110
    jmp irq_handler_common

.globl irq_111
irq_111:
    pushl $0
    pushl $111
    jmp irq_handler_common

.globl irq_112
irq_112:
    pushl $0
    pushl $112
    jmp irq_handler_common

.globl irq_113
irq_113:
    pushl $0
    pushl $113
    jmp irq_handler_common

.globl irq_114
irq_114:
    pushl $0
    pushl $114
    jmp irq_handler_common

.globl irq_115
irq_115:
    pushl $0
    pushl $115
    jmp irq_handler_common

.globl irq_116
irq_116:
    pushl $0
    pushl $116
    jmp irq_handler_common

.globl irq_117
irq_117:
    pushl $0
    pushl $117
    jmp irq_handler_common

.globl irq_118
irq_118:
    pushl $0
    pushl $118
    jmp irq_handler_common

.globl irq_119
irq_119:
    pushl $0
    pushl $119
    jmp irq_handler_common

.globl irq_120
irq_120:
    pushl $0
    pushl $120
    jmp irq_handler_common

.globl irq_121
irq_121:
    pushl $0
    pushl $121
    jmp irq_handler_common

.globl irq_122
irq_122:
    pushl $0
    pushl $122
    jmp irq_handler_common

.globl irq_123
irq_123:
    pushl $0
    pushl $123
    jmp irq_handler_common

.globl irq_124
irq_124:
    pushl $0
    pushl $124
    jmp irq_handler_common

.globl irq_125
irq_125:
    pushl $0
    pushl $125
    jmp irq_handler_common

.globl irq_126
irq_126:
    pushl $0
    pushl $126
    jmp irq_handler_common

.globl irq_127
irq_127:
    pushl $0
    pushl $127
    jmp irq_handler_common

.globl irq_128
irq_128:
    pushl $0
    pushl $128
    jmp irq_handler_common

.globl irq_129
irq_129:
    pushl $0
    pushl $129
    jmp irq_handler_common

.globl irq_130
irq_130:
    pushl $0
    pushl $130
    jmp irq_handler_common

.globl irq_131
irq_131:
    pushl $0
    pushl $131
    jmp irq_handler_common

.globl irq_132
irq_132:
    pushl $0
    pushl $132
    jmp irq_handler_common

.globl irq_133
irq_133:
    pushl $0
    pushl $133
    jmp irq_handler_common

.globl irq_134
irq_134:
    pushl $0
    pushl $134
    jmp irq_handler_common

.globl irq_135
irq_135:
    pushl $0
    pushl $135
    jmp irq_handler_common

.globl irq_136
irq_136:
    pushl $0
    pushl $136
    jmp irq_handler_common

.globl irq_137
irq_137:
    pushl $0
    pushl $137
    jmp irq_handler_common

.globl irq_138
irq_138:
    pushl $0
    pushl $138
    jmp irq_handler_common

.globl irq_139
irq_139:
    pushl $0
    pushl $139
    jmp irq_handler_common

.globl irq_140
irq_140:
    pushl $0
    pushl $140
    jmp irq_handler_common

.globl irq_141
irq_141:
    pushl $0
    pushl $141
    jmp irq_handler_common

.globl irq_142
irq_142:
    pushl $0
    pushl $142
    jmp irq_handler_common

.globl irq_143
irq_143:
    pushl $0
    pushl $143
    jmp irq_handler_common

.globl irq_144
irq_144:
    pushl $0
    pushl $144
    jmp irq_handler_common

.globl irq_145
irq_145:
    pushl $0
    pushl $145
    jmp irq_handler_common

.globl irq_146
irq_146:
    pushl $0
    pushl $146
    jmp irq_handler_common

.globl irq_147
irq_147:
    pushl $0
    pushl $147
    jmp irq_handler_common

.globl irq_148
irq_148:
    pushl $0
    pushl $148
    jmp irq_handler_common

.globl irq_149
irq_149:
    pushl $0
    pushl $149
    jmp irq_handler_common

.globl irq_150
irq_150:
    pushl $0
    pushl $150
    jmp irq_handler_common

.globl irq_151
irq_151:
    pushl $0
    pushl $151
    jmp irq_handler_common

.globl irq_152
irq_152:
    pushl $0
    pushl $152
    jmp irq_handler_common

.globl irq_153
irq_153:
    pushl $0
    pushl $153
    jmp irq_handler_common

.globl irq_154
irq_154:
    pushl $0
    pushl $154
    jmp irq_handler_common

.globl irq_155
irq_155:
    pushl $0
    pushl $155
    jmp irq_handler_common

.globl irq_156
irq_156:
    pushl $0
    pushl $156
    jmp irq_handler_common

.globl irq_157
irq_157:
    pushl $0
    pushl $157
    jmp irq_handler_common

.globl irq_158
irq_158:
    pushl $0
    pushl $158
    jmp irq_handler_common

.globl irq_159
irq_159:
    pushl $0
    pushl $159
    jmp irq_handler_common

.globl irq_160
irq_160:
    pushl $0
    pushl $160
    jmp irq_handler_common

.globl irq_161
irq_161:
    pushl $0
    pushl $161
    jmp irq_handler_common

.globl irq_162
irq_162:
    pushl $0
    pushl $162
    jmp irq_handler_common

.globl irq_163
irq_163:
    pushl $0
    pushl $163
    jmp irq_handler_common

.globl irq_164
irq_164:
    pushl $0
    pushl $164
    jmp irq_handler_common

.globl irq_165
irq_165:
    pushl $0
    pushl $165
    jmp irq_handler_common

.globl irq_166
irq_166:
    pushl $0
    pushl $166
    jmp irq_handler_common

.globl irq_167
irq_167:
    pushl $0
    pushl $167
    jmp irq_handler_common

.globl irq_168
irq_168:
    pushl $0
    pushl $168
    jmp irq_handler_common

.globl irq_169
irq_169:
    pushl $0
    pushl $169
    jmp irq_handler_common

.globl irq_170
irq_170:
    pushl $0
    pushl $170
    jmp irq_handler_common

.globl irq_171
irq_171:
    pushl $0
    pushl $171
    jmp irq_handler_common

.globl irq_172
irq_172:
    pushl $0
    pushl $172
    jmp irq_handler_common

.globl irq_173
irq_173:
    pushl $0
    pushl $173
    jmp irq_handler_common

.globl irq_174
irq_174:
    pushl $0
    pushl $174
    jmp irq_handler_common

.globl irq_175
irq_175:
    pushl $0
    pushl $175
    jmp irq_handler_common

.globl irq_176
irq_176:
    pushl $0
    pushl $176
    jmp irq_handler_common

.globl irq_177
irq_177:
    pushl $0
    pushl $177
    jmp irq_handler_common

.globl irq_178
irq_178:
    pushl $0
    pushl $178
    jmp irq_handler_common

.globl irq_179
irq_179:
    pushl $0
    pushl $179
    jmp irq_handler_common

.globl irq_180
irq_180:
    pushl $0
    pushl $180
    jmp irq_handler_common

.globl irq_181
irq_181:
    pushl $0
    pushl $181
    jmp irq_handler_common

.globl irq_182
irq_182:
    pushl $0
    pushl $182
    jmp irq_handler_common

.globl irq_183
irq_183:
    pushl $0
    pushl $183
    jmp irq_handler_common

.globl irq_184
irq_184:
    pushl $0
    pushl $184
    jmp irq_handler_common

.globl irq_185
irq_185:
    pushl $0
    pushl $185
    jmp irq_handler_common

.globl irq_186
irq_186:
    pushl $0
    pushl $186
    jmp irq_handler_common

.globl irq_187
irq_187:
    pushl $0
    pushl $187
    jmp irq_handler_common

.globl irq_188
irq_188:
    pushl $0
    pushl $188
    jmp irq_handler_common

.globl irq_189
irq_189:
    pushl $0
    pushl $189
    jmp irq_handler_common

.globl irq_190
irq_190:
    pushl $0
    pushl $190
    jmp irq_handler_common

.globl irq_191
irq_191:
    pushl $0
    pushl $191
    jmp irq_handler_common

.globl irq_192
irq_192:
    pushl $0
    pushl $192
    jmp irq_handler_common

.globl irq_193
irq_193:
    pushl $0
    pushl $193
    jmp irq_handler_common

.globl irq_194
irq_194:
    pushl $0
    pushl $194
    jmp irq_handler_common

.globl irq_195
irq_195:
    pushl $0
    pushl $195
    jmp irq_handler_common

.globl irq_196
irq_196:
    pushl $0
    pushl $196
    jmp irq_handler_common

.globl irq_197
irq_197:
    pushl $0
    pushl $197
    jmp irq_handler_common

.globl irq_198
irq_198:
    pushl $0
    pushl $198
    jmp irq_handler_common

.globl irq_199
irq_199:
    pushl $0
    pushl $199
    jmp irq_handler_common

.globl irq_200
irq_200:
    pushl $0
    pushl $200
    jmp irq_handler_common

.globl irq_201
irq_201:
    pushl $0
    pushl $201
    jmp irq_handler_common

.globl irq_202
irq_202:
    pushl $0
    pushl $202
    jmp irq_handler_common

.globl irq_203
irq_203:
    pushl $0
    pushl $203
    jmp irq_handler_common

.globl irq_204
irq_204:
    pushl $0
    pushl $204
    jmp irq_handler_common

.globl irq_205
irq_205:
    pushl $0
    pushl $205
    jmp irq_handler_common

.globl irq_206
irq_206:
    pushl $0
    pushl $206
    jmp irq_handler_common

.globl irq_207
irq_207:
    pushl $0
    pushl $207
    jmp irq_handler_common

.globl irq_208
irq_208:
    pushl $0
    pushl $208
    jmp irq_handler_common

.globl irq_209
irq_209:
    pushl $0
    pushl $209
    jmp irq_handler_common

.globl irq_210
irq_210:
    pushl $0
    pushl $210
    jmp irq_handler_common

.globl irq_211
irq_211:
    pushl $0
    pushl $211
    jmp irq_handler_common

.globl irq_212
irq_212:
    pushl $0
    pushl $212
    jmp irq_handler_common

.globl irq_213
irq_213:
    pushl $0
    pushl $213
    jmp irq_handler_common

.globl irq_214
irq_214:
    pushl $0
    pushl $214
    jmp irq_handler_common

.globl irq_215
irq_215:
    pushl $0
    pushl $215
    jmp irq_handler_common

.globl irq_216
irq_216:
    pushl $0
    pushl $216
    jmp irq_handler_common

.globl irq_217
irq_217:
    pushl $0
    pushl $217
    jmp irq_handler_common

.globl irq_218
irq_218:
    pushl $0
    pushl $218
    jmp irq_handler_common

.globl irq_219
irq_219:
    pushl $0
    pushl $219
    jmp irq_handler_common

.globl irq_220
irq_220:
    pushl $0
    pushl $220
    jmp irq_handler_common

.globl irq_221
irq_221:
    pushl $0
    pushl $221
    jmp irq_handler_common

.globl irq_222
irq_222:
    pushl $0
    pushl $222
    jmp irq_handler_common

.globl irq_223
irq_223:
    pushl $0
    pushl $223
    jmp irq_handler_common

.globl irq_224
irq_224:
    pushl $0
    pushl $224
    jmp irq_handler_common

.globl irq_225
irq_225:
    pushl $0
    pushl $225
    jmp irq_handler_common

.globl irq_226
irq_226:
    pushl $0
    pushl $226
    jmp irq_handler_common

.globl irq_227
irq_227:
    pushl $0
    pushl $227
    jmp irq_handler_common

.globl irq_228
irq_228:
    pushl $0
    pushl $228
    jmp irq_handler_common

.globl irq_229
irq_229:
    pushl $0
    pushl $229
    jmp irq_handler_common

.globl irq_230
irq_230:
    pushl $0
    pushl $230
    jmp irq_handler_common

.globl irq_231
irq_231:
    pushl $0
    pushl $231
    jmp irq_handler_common

.globl irq_232
irq_232:
    pushl $0
    pushl $232
    jmp irq_handler_common

.globl irq_233
irq_233:
    pushl $0
    pushl $233
    jmp irq_handler_common

.globl irq_234
irq_234:
    pushl $0
    pushl $234
    jmp irq_handler_common

.globl irq_235
irq_235:
    pushl $0
    pushl $235
    jmp irq_handler_common

.globl irq_236
irq_236:
    pushl $0
    pushl $236
    jmp irq_handler_common

.globl irq_237
irq_237:
    pushl $0
    pushl $237
    jmp irq_handler_common

.globl irq_238
irq_238:
    pushl $0
    pushl $238
    jmp irq_handler_common

.globl irq_239
irq_239:
    pushl $0
    pushl $239
    jmp irq_handler_common

.globl irq_240
irq_240:
    pushl $0
    pushl $240
    jmp irq_handler_common

.globl irq_241
irq_241:
    pushl $0
    pushl $241
    jmp irq_handler_common

.globl irq_242
irq_242:
    pushl $0
    pushl $242
    jmp irq_handler_common

.globl irq_243
irq_243:
    pushl $0
    pushl $243
    jmp irq_handler_common

.globl irq_244
irq_244:
    pushl $0
    pushl $244
    jmp irq_handler_common

.globl irq_245
irq_245:
    pushl $0
    pushl $245
    jmp irq_handler_common

.globl irq_246
irq_246:
    pushl $0
    pushl $246
    jmp irq_handler_common

.globl irq_247
irq_247:
    pushl $0
    pushl $247
    jmp irq_handler_common

.globl irq_248
irq_248:
    pushl $0
    pushl $248
    jmp irq_handler_common

.globl irq_249
irq_249:
    pushl $0
    pushl $249
    jmp irq_handler_common

.globl irq_250
irq_250:
    pushl $0
    pushl $250
    jmp irq_handler_common

.globl irq_251
irq_251:
    pushl $0
    pushl $251
    jmp irq_handler_common

.globl irq_252
irq_252:
    pushl $0
    pushl $252
    jmp irq_handler_common

.globl irq_253
irq_253:
    pushl $0
    pushl $253
    jmp irq_handler_common

.globl irq_254
irq_254:
    pushl $0
    pushl $254
    jmp irq_handler_common

.globl irq_255
irq_255:
    pushl $0
    pushl $255
    jmp irq_handler_common


# Interrupt Vector Table

.data
.globl ivt
ivt:
    .long irq_0
    .long irq_1
    .long irq_2
    .long irq_3
    .long irq_4
    .long irq_5
    .long irq_6
    .long irq_7
    .long irq_8
    .long irq_9
    .long irq_10
    .long irq_11
    .long irq_12
    .long irq_13
    .long irq_14
    .long irq_15
    .long irq_16
    .long irq_17
    .long irq_18
    .long irq_19
    .long irq_20
    .long irq_21
    .long irq_22
    .long irq_23
    .long irq_24
    .long irq_25
    .long irq_26
    .long irq_27
    .long irq_28
    .long irq_29
    .long irq_30
    .long irq_31
    .long irq_32
    .long irq_33
    .long irq_34
    .long irq_35
    .long irq_36
    .long irq_37
    .long irq_38
    .long irq_39
    .long irq_40
    .long irq_41
    .long irq_42
    .long irq_43
    .long irq_44
    .long irq_45
    .long irq_46
    .long irq_47
    .long irq_48
    .long irq_49
    .long irq_50
    .long irq_51
    .long irq_52
    .long irq_53
    .long irq_54
    .long irq_55
    .long irq_56
    .long irq_57
    .long irq_58
    .long irq_59
    .long irq_60
    .long irq_61
    .long irq_62
    .long irq_63
    .long irq_64
    .long irq_65
    .long irq_66
    .long irq_67
    .long irq_68
    .long irq_69
    .long irq_70
    .long irq_71
    .long irq_72
    .long irq_73
    .long irq_74
    .long irq_75
    .long irq_76
    .long irq_77
    .long irq_78
    .long irq_79
    .long irq_80
    .long irq_81
    .long irq_82
    .long irq_83
    .long irq_84
    .long irq_85
    .long irq_86
    .long irq_87
    .long irq_88
    .long irq_89
    .long irq_90
    .long irq_91
    .long irq_92
    .long irq_93
    .long irq_94
    .long irq_95
    .long irq_96
    .long irq_97
    .long irq_98
    .long irq_99
    .long irq_100
    .long irq_101
    .long irq_102
    .long irq_103
    .long irq_104
    .long irq_105
    .long irq_106
    .long irq_107
    .long irq_108
    .long irq_109
    .long irq_110
    .long irq_111
    .long irq_112
    .long irq_113
    .long irq_114
    .long irq_115
    .long irq_116
    .long irq_117
    .long irq_118
    .long irq_119
    .long irq_120
    .long irq_121
    .long irq_122
    .long irq_123
    .long irq_124
    .long irq_125
    .long irq_126
    .long irq_127
    .long irq_128
    .long irq_129
    .long irq_130
    .long irq_131
    .long irq_132
    .long irq_133
    .long irq_134
    .long irq_135
    .long irq_136
    .long irq_137
    .long irq_138
    .long irq_139
    .long irq_140
    .long irq_141
    .long irq_142
    .long irq_143
    .long irq_144
    .long irq_145
    .long irq_146
    .long irq_147
    .long irq_148
    .long irq_149
    .long irq_150
    .long irq_151
    .long irq_152
    .long irq_153
    .long irq_154
    .long irq_155
    .long irq_156
    .long irq_157
    .long irq_158
    .long irq_159
    .long irq_160
    .long irq_161
    .long irq_162
    .long irq_163
    .long irq_164
    .long irq_165
    .long irq_166
    .long irq_167
    .long irq_168
    .long irq_169
    .long irq_170
    .long irq_171
    .long irq_172
    .long irq_173
    .long irq_174
    .long irq_175
    .long irq_176
    .long irq_177
    .long irq_178
    .long irq_179
    .long irq_180
    .long irq_181
    .long irq_182
    .long irq_183
    .long irq_184
    .long irq_185
    .long irq_186
    .long irq_187
    .long irq_188
    .long irq_189
    .long irq_190
    .long irq_191
    .long irq_192
    .long irq_193
    .long irq_194
    .long irq_195
    .long irq_196
    .long irq_197
    .long irq_198
    .long irq_199
    .long irq_200
    .long irq_201
    .long irq_202
    .long irq_203
    .long irq_204
    .long irq_205
    .long irq_206
    .long irq_207
    .long irq_208
    .long irq_209
    .long irq_210
    .long irq_211
    .long irq_212
    .long irq_213
    .long irq_214
    .long irq_215
    .long irq_216
    .long irq_217
    .long irq_218
    .long irq_219
    .long irq_220
    .long irq_221
    .long irq_222
    .long irq_223
    .long irq_224
    .long irq_225
    .long irq_226
    .long irq_227
    .long irq_228
    .long irq_229
    .long irq_230
    .long irq_231
    .long irq_232
    .long irq_233
    .long irq_234
    .long irq_235
    .long irq_236
    .long irq_237
    .long irq_238
    .long irq_239
    .long irq_240
    .long irq_241
    .long irq_242
    .long irq_243
    .long irq_244
    .long irq_245
    .long irq_246
    .long irq_247
    .long irq_248
    .long irq_249
    .long irq_250
    .long irq_251
    .long irq_252
    .long irq_253
    .long irq_254
    .long irq_255
