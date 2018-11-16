

const char *ctype_name[cMAX]=
{
    "cNONE", "cSTAIR", "cCAVE", "cWATER", "cSTATUE", "cGRAVE", "cDOCK",
    "cUNDEF", "cPUSH_WAIT", "cPUSH_HEAVY", "cPUSH_HW", "cL_STATUE", "cR_STATUE",
    "cWALKSLOW", "cCVUP", "cCVDOWN", "cCVLEFT", "cCVRIGHT", "cSWIMWARP", "cDIVEWARP",
    "cLADDERORGRAPPLE", "cTRIGNOFLAG", "cTRIGFLAG", "cWINGAME", "cSLASH", "cSLASHITEM",
    "cPUSH_HEAVY2", "cPUSH_HW2", "cPOUND", "cHSGRAB", "cHSBRIDGE", "cDAMAGE1",
    "cDAMAGE2", "cDAMAGE3", "cDAMAGE4", "cC_STATUE", "cTRAP_H", "cTRAP_V", "cTRAP_4",
    "cTRAP_LR", "cTRAP_UD", "cPIT", "cGRAPPLEONLY", "cOVERHEAD", "cNOFLYZONE", "cMIRROR",
    "cMIRRORSLASH", "cMIRRORBACKSLASH", "cMAGICPRISM", "cMAGICPRISM4",
    "cMAGICSPONGE", "cCAVE2", "cEYEBALL_A", "cEYEBALL_B", "cNOJUMPZONE", "cBUSH",
    "cFLOWERS", "cTALLGRASS", "cSHALLOWWATER", "cLOCKBLOCK", "cLOCKBLOCK2",
    "cBOSSLOCKBLOCK", "cBOSSLOCKBLOCK2", "cLADDERONLY", "cBSGRAVE",
    "cCHEST", "cCHEST2", "cLOCKEDCHEST", "cLOCKEDCHEST2", "cBOSSCHEST", "cBOSSCHEST2",
    "cRESET", "cSAVE", "cSAVE2", "cCAVEB", "cCAVEC", "cCAVED",
    "cSTAIRB", "cSTAIRC", "cSTAIRD", "cPITB", "cPITC", "cPITD",
    "cCAVE2B", "cCAVE2C", "cCAVE2D", "cSWIMWARPB", "cSWIMWARPC", "cSWIMWARPD",
    "cDIVEWARPB", "cDIVEWARPC", "cDIVEWARPD", "cSTAIRR", "cPITR",
    "cAWARPA", "cAWARPB", "cAWARPC", "cAWARPD", "cAWARPR",
    "cSWARPA", "cSWARPB", "cSWARPC", "cSWARPD", "cSWARPR", "cSTRIGNOFLAG", "cSTRIGFLAG",
    "cSTEP", "cSTEPSAME", "cSTEPALL", "cSTEPCOPY", "cNOENEMY", "cBLOCKARROW1", "cBLOCKARROW2",
    "cBLOCKARROW3", "cBLOCKBRANG1", "cBLOCKBRANG2", "cBLOCKBRANG3", "cBLOCKSBEAM", "cBLOCKALL",
    "cBLOCKFIREBALL", "cDAMAGE5", "cDAMAGE6", "cDAMAGE7", "cCHANGE", "cSPINTILE1", "cSPINTILE2",
    "cSCREENFREEZE", "cSCREENFREEZEFF", "cNOGROUNDENEMY", "cSLASHNEXT", "cSLASHNEXTITEM", "cBUSHNEXT"
    "cSLASHTOUCHY", "cSLASHITEMTOUCHY", "cBUSHTOUCHY", "cFLOWERSTOUCHY", "cTALLGRASSTOUCHY",
    "cSLASHNEXTTOUCHY", "cSLASHNEXTITEMTOUCHY", "cBUSHNEXTTOUCHY", "cEYEBALL_4", "cTALLGRASSNEXT",
    "cSCRIPT1", "cSCRIPT2", "cSCRIPT3", "cSCRIPT4", "cSCRIPT5",
    "cSCRIPT6", "cSCRIPT7", "cSCRIPT8", "cSCRIPT9", "cSCRIPT10",
    "cSCRIPT11", "cSCRIPT12", "cSCRIPT13", "cSCRIPT14", "cSCRIPT15",
    "cSCRIPT16", "cSCRIPT17", "cSCRIPT18", "cSCRIPT19", "cSCRIPT20"
    
};	     

weapon types (wMax)
com bo types (cMAX)


combo flags:
const char *flag_string[MAXFLAGS] =
{
    "  0 (None)",
    "  1 Push Block (Vertical, Trigger)",
    "  2 Push Block (4-Way, Trigger)",
    "  3 Whistle Trigger",
    "  4 Burn Trigger (Any)",
    "  5 Arrow Trigger (Any)",
    "  6 Bomb Trigger (Any)",
    "  7 Fairy Ring (Life)",
    "  8 Raft Path",
    "  9 Armos -> Secret",
    " 10 Armos/Chest -> Item",
    " 11 Bomb (Super)",
    " 12 Raft Branch",
    " 13 Dive -> Item",
    " 14 Lens Marker",
    " 15 Zelda (Win Game)",
    " 16 Secret Tile 0",
    " 17 Secret Tile 1",
    " 18 Secret Tile 2",
    " 19 Secret Tile 3",
    " 20 Secret Tile 4",
    " 21 Secret Tile 5",
    " 22 Secret Tile 6",
    " 23 Secret Tile 7",
    " 24 Secret Tile 8",
    " 25 Secret Tile 9",
    " 26 Secret Tile 10",
    " 27 Secret Tile 11",
    " 28 Secret Tile 12",
    " 29 Secret Tile 13",
    " 30 Secret Tile 14",
    " 31 Secret Tile 15",
    " 32 Trap (Horizontal, Line of Sight)",
    " 33 Trap (Vertical, Line of Sight)",
    " 34 Trap (4-Way, Line of Sight)",
    " 35 Trap (Horizontal, Constant)",
    " 36 Trap (Vertical, Constant)",
    " 37 Enemy 0",
    " 38 Enemy 1",
    " 39 Enemy 2",
    " 40 Enemy 3",
    " 41 Enemy 4",
    " 42 Enemy 5",
    " 43 Enemy 6",
    " 44 Enemy 7",
    " 45 Enemy 8",
    " 46 Enemy 9",
    " 47 Push Block (Horiz, Once, Trigger)",
    " 48 Push Block (Up, Once, Trigger)",
    " 49 Push Block (Down, Once, Trigger)",
    " 50 Push Block (Left, Once, Trigger)",
    " 51 Push Block (Right, Once, Trigger)",
    " 52 Push Block (Vert, Once)",
    " 53 Push Block (Horizontal, Once)",
    " 54 Push Block (4-Way, Once)",
    " 55 Push Block (Up, Once)",
    " 56 Push Block (Down, Once)",
    " 57 Push Block (Left, Once)",
    " 58 Push Block (Right, Once)",
    " 59 Push Block (Vertical, Many)",
    " 60 Push Block (Horizontal, Many)",
    " 61 Push Block (4-Way, Many)",
    " 62 Push Block (Up, Many)",
    " 63 Push Block (Down, Many)",
    " 64 Push Block (Left, Many)",
    " 65 Push Block (Right, Many)",
    " 66 Block Trigger",
    " 67 No Push Blocks",
    " 68 Boomerang Trigger (Any)",
    " 69 Boomerang Trigger (Magic +)",
    " 70 Boomerang Trigger (Fire)",
    " 71 Arrow Trigger (Silver +)",
    " 72 Arrow Trigger (Golden)",
    " 73 Burn Trigger (Red Candle +)",
    " 74 Burn Trigger (Wand Fire)",
    " 75 Burn Trigger (Din's Fire)",
    " 76 Magic Trigger (Wand)",
    " 77 Magic Trigger (Reflected)",
    " 78 Fireball Trigger (Reflected)",
    " 79 Sword Trigger (Any)",
    " 80 Sword Trigger (White +)",
    " 81 Sword Trigger (Magic +)",
    " 82 Sword Trigger (Master)",
    " 83 Sword Beam Trigger (Any)",
    " 84 Sword Beam Trigger (White +)",
    " 85 Sword Beam Trigger (Magic +)",
    " 86 Sword Beam Trigger (Master)",
    " 87 Hookshot Trigger",
    " 88 Wand Trigger",
    " 89 Hammer Trigger",
    " 90 Strike Trigger",
    " 91 Block Hole (Block -> Next)",
    " 92 Fairy Ring (Magic)",
    " 93 Fairy Ring (All)",
    " 94 Trigger -> Self Only",
    " 95 Trigger -> Self, Secret Tiles",
    " 96 No Enemies",
    " 97 No Ground Enemies",
    " 98 General Purpose 1 (Scripts)",
    " 99 General Purpose 2 (Scripts)",
    "100 General Purpose 3 (Scripts)",
    "101 General Purpose 4 (Scripts)",
    "102 General Purpose 5 (Scripts)",
    "103 Raft Bounce",
     "104 Pushed",
    "105 General Purpose 6 (Scripts)",
    "106 General Purpose 7 (Scripts)",
    "107 General Purpose 8 (Scripts)",
    "108 General Purpose 9 (Scripts)",
    "109 General Purpose 10 (Scripts)",
    "110 General Purpose 11 (Scripts)",
    "111 General Purpose 12 (Scripts)",
    "112 General Purpose 13 (Scripts)",
    "113 General Purpose 14 (Scripts)",
    "114 General Purpose 15 (Scripts)",
    "115 General Purpose 16 (Scripts)",
    "116 General Purpose 17 (Scripts)",
    "117 General Purpose 18 (Scripts)",
    "118 General Purpose 19 (Scripts)",
    "119 General Purpose 20 (Scripts)",
    "120 Pit or Hole (Scripted)",
    "121 Pit or Hole, Fall Down Floor (Scripted)",
    "122 Fire or Lava (Scripted)",
    "123 Ice (Scripted)",
    "124 Ice, Damaging (Scripted)",
    "125 Damage-1 (Scripted)",
    "126 Damage-2 (Scripted)",
    "127 Damage-4 (Scripted)",
    "128 Damage-8 (Scripted)",
    "119 Damage-16 (Scripted)",
    "130 Damage-32 (Scripted)",
    "131 Freeze Screen (Unimplemented)",
    "132 Freeze Screen, Except FFCs (Unimplemented)",
    "133 Freeze FFCs Only (Unimplemented)",
    "134 Trigger LW_SCRIPT1 (Unimplemented)",
    "135 Trigger LW_SCRIPT2 (Unimplemented)",
    "136 Trigger LW_SCRIPT3 (Unimplemented)",
    "137 Trigger LW_SCRIPT4 (Unimplemented)",
    "138 Trigger LW_SCRIPT5 (Unimplemented)",
    "139 Trigger LW_SCRIPT6 (Unimplemented)",
    "140 Trigger LW_SCRIPT7 (Unimplemented)",
    "141 Trigger LW_SCRIPT8 (Unimplemented)",
    "142 Trigger LW_SCRIPT9 (Unimplemented)",
    "143 Trigger LW_SCRIPT10 (Unimplemented)",
    "144 Dig Spot (Scripted)",
    "145 Dig Spot, Next (Scripted)",
    "146 Dig Spot, Special Item (Scripted)",
    "147 Pot, Slashable (Scripted)",
    "148 Pot, Liftable (Scripted)",
    "149 Pot, Slash or Lift (Scripted)",
    "150 Rock, Lift Normal (Scripted)",
    "151 Rock, Lift Heavy (Scripted)",
    "152 Dropset Item (Scripted)",
    "153 Special Item (Scripted)",
    "154 Drop Key (Scripted)",
    "155 Drop level-Specific Key (Scripted)",
    "156 Drop Compass (Scripted)",
    "157 Drop Map (Scripted)",
    "158 Drop Bosskey (Scripted)",
    "159 Spawn NPC (Scripted)",
    "160 SwitchHook Spot (Scripted)",
    "161 mf161",
    "162 mf162","163 mf163","164 mf164","165 mf165","166 mf166","167 mf167","168 mf168","169 mf169",
    "170 mf170","171 mf171","172 mf172","173 mf173","174 mf174","175 mf175","176 mf176","177 mf177","178 mf178","179 mf179",
    "180 mf180","181 mf181","182 mf182","183 mf183","184 mf184","185 mf185","186 mf186","187 mf187","188 mf188","189 mf189",
    "190 mf190","191 mf191","192 mf192","193 mf193","194 mf194","195 mf195","196 mf196","197 mf197","198 mf198","199 mf199",
    "200 mf200","201 mf201","202 mf202","203 mf203","204 mf204","205 mf205","206 mf206","207 mf207","208 mf208","209 mf209",
    "210 mf210","211 mf211","212 mf212","213 mf213","214 mf214","215 mf215","216 mf216","217 mf217","218 mf218","219 mf219",
    "220 mf220","221 mf221","222 mf222","223 mf223","224 mf224","225 mf225","226 mf226","227 mf227","228 mf228","229 mf229",
    "230 mf230","231 mf231","232 mf232","233 mf233","234 mf234","235 mf235","236 mf236","237 mf237","238 mf238","239 mf239",
    "240 mf240","241 mf241","242 mf242","243 mf243","244 mf244","245 mf245","246 mf246","247 mf247","248 mf248","249 mf249",
    "250 mf250","251 mf251","252 mf252","253 mf253","254 mf254",
    "255 Extended (Extended Flag Editor)"
};

const char *roomtype_string[MAXROOMTYPES] =
{
    "(None)","Special Item","Pay for Info","Secret Money","Gamble",
    "Door Repair","Red Potion or Heart Container","Feed the Goriya","Level 9 Entrance",
    "Potion Shop","Shop","More Bombs","Leave Money or Life","10 Rupees",
    "3-Stair Warp","Ganon","Zelda", "-<item pond>", "1/2 Magic Upgrade", "Learn Slash", "More Arrows","Take One Item"
};


const char *walkmisc7_string[e7tEATHURT+1] =
{
    "None", "Temp. Jinx", "Perm. Jinx", "Cure Jinx", "Lose Magic", "Lose Rupees", "Drunk", "Eat (Items)", "Eat (Magic)", "Eat (Rupees)", "Eat (Damage)"
};

const char *walkmisc9_string[e9tARMOS+1] =
{
    "Normal", "Rope", "Vire", "Pols Voice", "Armos"
};

"ic_89",
"ic_90",
"ic_91",
"ic_92",
"ic_93",
"ic_94",
"ic_95",
"ic_96",
"ic_97",
"ic_98",
"ic_99",
"ic_100",
"ic_101",
"ic_102",
"ic_103",
"ic_104",
"ic_105",
"ic_106",
"ic_107",
"ic_108",
"ic_109",
"ic_111",
"ic_112",
"ic_113",
"ic_114",
"ic_115",
"ic_116",
"ic_117",
"ic_118",
"ic_119",
"ic_120",
"ic_121",
"ic_122",
"ic_123",
"ic_124",
"ic_125",
"ic_126",
"ic_127",
"ic_128",
"ic_129",
"ic_130",
"ic_131",
"ic_132",
"ic_133",
"ic_134",
"ic_135",
"ic_136",
"ic_137",
"ic_138",
"ic_139",
"ic_140",
"ic_141",
"ic_142",
"ic_143",
"ic_144",
"ic_145",
"ic_146",
"ic_147",
"ic_148",
"ic_149",
"ic_150",
"ic_151",
"ic_152",
"ic_153",
"ic_154",
"ic_155",
"ic_156",
"ic_157",
"ic_158",
"ic_159",
"ic_160",
"ic_161",
"ic_162",
"ic_163",
"ic_164",
"ic_165",
"ic_166",
"ic_167",
"ic_168",
"ic_169",
"ic_170",
"ic_171",
"ic_172",
"ic_173",
"ic_174",
"ic_175",
"ic_176",
"ic_177",
"ic_178",
"ic_179",
"ic_180",
"ic_181",
"ic_182",
"ic_183",
"ic_184",
"ic_185",
"ic_186",
"ic_187",
"ic_188",
"ic_189",
"ic_190",
"ic_191",
"ic_192",
"ic_193",
"ic_194",
"ic_195",
"ic_196",
"ic_197",
"ic_198",
"ic_199",
"ic_200",
"ic_201",
"ic_202",
"ic_203",
"ic_204",
"ic_205",
"ic_206",
"ic_207",
"ic_208",
"ic_209",
"ic_210",
"ic_211",
"ic_212",
"ic_213",
"ic_214",
"ic_215",
"ic_216",
"ic_217",
"ic_218",
"ic_219",
"ic_220",
"ic_221",
"ic_222",
"ic_223",
"ic_224",
"ic_225",
"ic_226",
"ic_227",
"ic_228",
"ic_229",
"ic_230",
"ic_231",
"ic_232",
"ic_233",
"ic_234",
"ic_235",
"ic_236",
"ic_237",
"ic_238",
"ic_239",
"ic_240",
"ic_241",
"ic_242",
"ic_243",
"ic_244",
"ic_245",
"ic_246",
"ic_247",
"ic_248",
"ic_249",
"ic_250",
"ic_251",
"ic_252",
"ic_253",
"ic_254",
"ic_255",
"ic_256",
"ic_257",
"ic_258",
"ic_259",
"ic_260",
"ic_261",
"ic_262",
"ic_263",
"ic_264",
"ic_265",
"ic_266",
"ic_267",
"ic_267",
"ic_269",
