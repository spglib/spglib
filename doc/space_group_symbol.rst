Space-group symbol found by spglib
-----------------------------------

There are equivalent space-group symbols for a space-group type
according to the choices of axes and origin. Spglib returns one of
them without any rule except that base centring is always
set as **C-centring**.

In symmetry search, an input cell is transformed to a conventional
unit by change of basis and origin shift. The results are given as
``transformation_matrix`` and ``origin_shift`` which are obtained by
calling :ref:`api_spg_get_dataset`.

The list of the symbols at the standard settings is shown below in
the python scripting form. This list may be used to convert a space-group
symbol to that at the standard setting using the space-group-type number.

::

   symbols_at_standard_settings = [
       "",
       "P1",         # 1
       "P-1",        # 2
       "P2",         # 3
       "P2_1",       # 4
       "C2",         # 5
       "Pm",         # 6
       "Pc",         # 7
       "Cm",         # 8
       "Cc",         # 9
       "P2/m",       # 10
       "P2_1/m",     # 11
       "C2/m",       # 12
       "P2/c",       # 13
       "P2_1/c",     # 14
       "C2/c",       # 15
       "P222",       # 16
       "P222_1",     # 17
       "P2_12_12",   # 18
       "P2_12_12_1", # 19
       "C222_1",     # 20
       "C222",       # 21
       "F222",       # 22
       "I222",       # 23
       "I2_12_12_1", # 24
       "Pmm2",       # 25
       "Pmc2_1",     # 26
       "Pcc2",       # 27
       "Pma2",       # 28
       "Pca2_1",     # 29
       "Pnc2",       # 30
       "Pmn2_1",     # 31
       "Pba2",       # 32
       "Pna2_1",     # 33
       "Pnn2",       # 34
       "Cmm2",       # 35
       "Cmc2_1",     # 36
       "Ccc2",       # 37
       "Amm2",       # 38
       "Aem2",       # 39
       "Ama2",       # 40
       "Aea2",       # 41
       "Fmm2",       # 42
       "Fdd2",       # 43
       "Imm2",       # 44
       "Iba2",       # 45
       "Ima2",       # 46
       "Pmmm",       # 47
       "Pnnn",       # 48
       "Pccm",       # 49
       "Pban",       # 50
       "Pmma",       # 51
       "Pnna",       # 52
       "Pmna",       # 53
       "Pcca",       # 54
       "Pbam",       # 55
       "Pccn",       # 56
       "Pbcm",       # 57
       "Pnnm",       # 58
       "Pmmn",       # 59
       "Pbcn",       # 60
       "Pbca",       # 61
       "Pnma",       # 62
       "Cmcm",       # 63
       "Cmce",       # 64
       "Cmmm",       # 65
       "Cccm",       # 66
       "Cmme",       # 67
       "Ccce",       # 68
       "Fmmm",       # 69
       "Fddd",       # 70
       "Immm",       # 71
       "Ibam",       # 72
       "Ibca",       # 73
       "Imma",       # 74
       "P4",         # 75
       "P4_1",       # 76
       "P4_2",       # 77
       "P4_3",       # 78
       "I4",         # 79
       "I4_1",       # 80
       "P-4",        # 81
       "I-4",        # 82
       "P4/m",       # 83
       "P4_2/m",     # 84
       "P4/n",       # 85
       "P4_2/n",     # 86
       "I4/m",       # 87
       "I4_1/a",     # 88
       "P422",       # 89
       "P42_12",     # 90
       "P4_122",     # 91
       "P4_12_12",   # 92
       "P4_222",     # 93
       "P4_22_12",   # 94
       "P4_322",     # 95
       "P4_32_12",   # 96
       "I422",       # 97
       "I4_122",     # 98
       "P4mm",       # 99
       "P4bm",       # 100
       "P4_2cm",     # 101
       "P4_2nm",     # 102
       "P4cc",       # 103
       "P4nc",       # 104
       "P4_2mc",     # 105
       "P4_2bc",     # 106
       "I4mm",       # 107
       "I4cm",       # 108
       "I4_1md",     # 109
       "I4_1cd",     # 110
       "P-42m",      # 111
       "P-42c",      # 112
       "P-42_1m",    # 113
       "P-42_1c",    # 114
       "P-4m2",      # 115
       "P-4c2",      # 116
       "P-4b2",      # 117
       "P-4n2",      # 118
       "I-4m2",      # 119
       "I-4c2",      # 120
       "I-42m",      # 121
       "I-42d",      # 122
       "P4/mmm",     # 123
       "P4/mcc",     # 124
       "P4/nbm",     # 125
       "P4/nnc",     # 126
       "P4/mbm",     # 127
       "P4/mnc",     # 128
       "P4/nmm",     # 129
       "P4/ncc",     # 130
       "P4_2/mmc",   # 131
       "P4_2/mcm",   # 132
       "P4_2/nbc",   # 133
       "P4_2/nnm",   # 134
       "P4_2/mbc",   # 135
       "P4_2/mnm",   # 136
       "P4_2/nmc",   # 137
       "P4_2/ncm",   # 138
       "I4/mmm",     # 139
       "I4/mcm",     # 140
       "I4_1/amd",   # 141
       "I4_1/acd",   # 142
       "P3",         # 143
       "P3_1",       # 144
       "P3_2",       # 145
       "R3",         # 146
       "P-3",        # 147
       "R-3",        # 148
       "P312",       # 149
       "P321",       # 150
       "P3_112",     # 151
       "P3_121",     # 152
       "P3_212",     # 153
       "P3_221",     # 154
       "R32",        # 155
       "P3m1",       # 156
       "P31m",       # 157
       "P3c1",       # 158
       "P31c",       # 159
       "R3m",        # 160
       "R3c",        # 161
       "P-31m",      # 162
       "P-31c",      # 163
       "P-3m1",      # 164
       "P-3c1",      # 165
       "R-3m",       # 166
       "R-3c",       # 167
       "P6",         # 168
       "P6_1",       # 169
       "P6_5",       # 170
       "P6_2",       # 171
       "P6_4",       # 172
       "P6_3",       # 173
       "P-6",        # 174
       "P6/m",       # 175
       "P6_3/m",     # 176
       "P622",       # 177
       "P6_122",     # 178
       "P6_522",     # 179
       "P6_222",     # 180
       "P6_422",     # 181
       "P6_322",     # 182
       "P6mm",       # 183
       "P6cc",       # 184
       "P6_3cm",     # 185
       "P6_3mc",     # 186
       "P-6m2",      # 187
       "P-6c2",      # 188
       "P-62m",      # 189
       "P-62c",      # 190
       "P6/mmm",     # 191
       "P6/mcc",     # 192
       "P6_3/mcm",   # 193
       "P6_3/mmc",   # 194
       "P23",        # 195
       "F23",        # 196
       "I23",        # 197
       "P2_13",      # 198
       "I2_13",      # 199
       "Pm3",        # 200
       "Pn3",        # 201
       "Fm3",        # 202
       "Fd3",        # 203
       "Im3",        # 204
       "Pa3",        # 205
       "Ia3",        # 206
       "P432",       # 207
       "P4_232",     # 208
       "F432",       # 209
       "F4_132",     # 210
       "I432",       # 211
       "P4_332",     # 212
       "P4_132",     # 213
       "I4_132",     # 214
       "P-43m",      # 215
       "F-43m",      # 216
       "I-43m",      # 217
       "P-43n",      # 218
       "F-43c",      # 219
       "I-43d",      # 220
       "Pm-3m",      # 221
       "Pn-3n",      # 222
       "Pm-3n",      # 223
       "Pn-3m",      # 224
       "Fm-3m",      # 225
       "Fm-3c",      # 226
       "Fd-3m",      # 227
       "Fd-3c",      # 228
       "Im-3m",      # 229
       "Ia-3d"]      # 230


.. |sflogo| image:: http://sflogo.sourceforge.net/sflogo.php?group_id=161614&type=1
            :target: http://sourceforge.net

|sflogo|
