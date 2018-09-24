#ifndef __GTHREAD_HIDE_WIN32API
#define __GTHREAD_HIDE_WIN32API 1
#endif                            //prevent indirectly including windows.h

#include "precompiled.h" //always first

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ffscript.h"
#include "ffasm.h"
#include "zasm/command.h"

#include "zc_malloc.h"
//#include "ffasm.h"
#include "zquest.h"
#include "zsys.h"

#include <string>

#ifdef ALLEGRO_MACOSX
#define strnicmp strncasecmp
#endif

#ifdef ALLEGRO_MACOSX
#define strnicmp strncasecmp
#endif

#ifdef ALLEGRO_LINUX
#define strnicmp strncasecmp
#endif

#ifdef _MSC_VER
#define stricmp _stricmp
#define strnicmp _strnicmp
#endif

extern char *datapath, *temppath;

using namespace std;

// Returns the error message.
string const* parse_script_section(
	char* command_buf,
	char* arg1_buf,
	char* arg2_buf,
	ffscript& instruction);

script_variable variable_list[]=
{
    //name                id                maxcount       multiple
    { "D",                 D(0),                 8,             0 },
    { "A",                 A(0),                 2,             0 },
    { "DATA",              DATA,                 0,             0 },
    { "CSET",              FCSET,                0,             0 },
    { "DELAY",             DELAY,                0,             0 },
    { "X",                 FX,                   0,             0 },
    { "Y",                 FY,                   0,             0 },
    { "XD",                XD,                   0,             0 },
    { "YD",                YD,                   0,             0 },
    { "XD2",               XD2,                  0,             0 },
    { "YD2",               YD2,                  0,             0 },
    { "FLAG",              FLAG,                 0,             0 },
    { "WIDTH",             WIDTH,                0,             0 },
    { "HEIGHT",            HEIGHT,               0,             0 },
    { "LINK",              LINK,                 0,             0 },
    { "FFFLAGSD",          FFFLAGSD,             0,             0 },
    { "FFCWIDTH",          FFCWIDTH,             0,             0 },
    { "FFCHEIGHT",         FFCHEIGHT,            0,             0 },
    { "FFTWIDTH",          FFTWIDTH,             0,             0 },
    { "FFTHEIGHT",         FFTHEIGHT,            0,             0 },
    { "FFLINK",            FFLINK,               0,             0 },
    //{ "COMBOD",            COMBOD(0),          176,             3 },
    //{ "COMBOC",            COMBOC(0),          176,             3 },
    //{ "COMBOF",            COMBOF(0),          176,             3 },
    { "INPUTSTART",        INPUTSTART,           0,             0 },
    { "INPUTUP",           INPUTUP,              0,             0 },
    { "INPUTDOWN",         INPUTDOWN,            0,             0 },
    { "INPUTLEFT",         INPUTLEFT,            0,             0 },
    { "INPUTRIGHT",        INPUTRIGHT,           0,             0 },
    { "INPUTA",            INPUTA,               0,             0 },
    { "INPUTB",            INPUTB,               0,             0 },
    { "INPUTL",            INPUTL,               0,             0 },
    { "INPUTR",            INPUTR,               0,             0 },
    { "INPUTMOUSEX",       INPUTMOUSEX,          0,             0 },
    { "INPUTMOUSEY",       INPUTMOUSEY,          0,             0 },
    { "LINKX",             LINKX,                0,             0 },
    { "LINKY",             LINKY,                0,             0 },
    { "LINKZ",             LINKZ,                0,             0 },
    { "LINKJUMP",          LINKJUMP,             0,             0 },
    { "LINKDIR",           LINKDIR,              0,             0 },
    { "LINKHITDIR",        LINKHITDIR,           0,             0 },
    { "LINKHP",            LINKHP,               0,             0 },
    { "LINKMP",            LINKMP,               0,             0 },
    { "LINKMAXHP",         LINKMAXHP,            0,             0 },
    { "LINKMAXMP",         LINKMAXMP,            0,             0 },
    { "LINKACTION",        LINKACTION,           0,             0 },
    { "LINKHELD",          LINKHELD,             0,             0 },
    { "LINKITEMD",         LINKITEMD,            0,             0 },
    { "LINKSWORDJINX",     LINKSWORDJINX,        0,             0 },
    { "LINKITEMJINX",      LINKITEMJINX,         0,             0 },
    { "LINKDRUNK",         LINKDRUNK,            0,             0 },
    { "ITEMX",             ITEMX,                0,             0 },
    { "ITEMY",             ITEMY,                0,             0 },
    { "ITEMZ",             ITEMZ,                0,             0 },
    { "ITEMJUMP",          ITEMJUMP,             0,             0 },
    { "ITEMDRAWTYPE",      ITEMDRAWTYPE,         0,             0 },
    { "ITEMID",            ITEMID,               0,             0 },
    { "ITEMTILE",          ITEMTILE,             0,             0 },
    { "ITEMOTILE",         ITEMOTILE,            0,             0 },
    { "ITEMCSET",          ITEMCSET,             0,             0 },
    { "ITEMFLASHCSET",     ITEMFLASHCSET,        0,             0 },
    { "ITEMFRAMES",        ITEMFRAMES,           0,             0 },
    { "ITEMFRAME",         ITEMFRAME,            0,             0 },
    { "ITEMASPEED",        ITEMASPEED,           0,             0 },
    { "ITEMDELAY",         ITEMDELAY,            0,             0 },
    { "ITEMFLASH",         ITEMFLASH,            0,             0 },
    { "ITEMFLIP",          ITEMFLIP,             0,             0 },
    { "ITEMCOUNT",         ITEMCOUNT,            0,             0 },
    { "IDATAFAMILY",       IDATAFAMILY,          0,             0 },
    { "IDATALEVEL",        IDATALEVEL,           0,             0 },
    { "IDATAKEEP",         IDATAKEEP,            0,             0 },
    { "IDATAAMOUNT",       IDATAAMOUNT,          0,             0 },
    { "IDATASETMAX",       IDATASETMAX,          0,             0 },
    { "IDATAMAX",          IDATAMAX,             0,             0 },
    { "IDATACOUNTER",      IDATACOUNTER,         0,             0 },
    { "ITEMEXTEND",        ITEMEXTEND,           0,             0 },
    { "NPCX",              NPCX,                 0,             0 },
    { "NPCY",              NPCY,                 0,             0 },
    { "NPCZ",              NPCZ,                 0,             0 },
    { "NPCJUMP",           NPCJUMP,              0,             0 },
    { "NPCDIR",            NPCDIR,               0,             0 },
    { "NPCRATE",           NPCRATE,              0,             0 },
    { "NPCSTEP",           NPCSTEP,              0,             0 },
    { "NPCFRAMERATE",      NPCFRAMERATE,         0,             0 },
    { "NPCHALTRATE",       NPCHALTRATE,          0,             0 },
    { "NPCDRAWTYPE",       NPCDRAWTYPE,          0,             0 },
    { "NPCHP",             NPCHP,                0,             0 },
    { "NPCID",             NPCID,                0,             0 },
    { "NPCDP",             NPCDP,                0,             0 },
    { "NPCWDP",            NPCWDP,               0,             0 },
    { "NPCOTILE",          NPCOTILE,             0,             0 },
    { "NPCENEMY",          NPCENEMY,             0,             0 },
    { "NPCWEAPON",         NPCWEAPON,            0,             0 },
    { "NPCITEMSET",        NPCITEMSET,           0,             0 },
    { "NPCCSET",           NPCCSET,              0,             0 },
    { "NPCBOSSPAL",        NPCBOSSPAL,           0,             0 },
    { "NPCBGSFX",          NPCBGSFX,             0,             0 },
    { "NPCCOUNT",          NPCCOUNT,             0,             0 },
    { "GD",                GD(0),              256,             0 },
    { "SDD",               SDD,                  0,             0 },
    { "GDD",               GDD,                  0,             0 },
    { "SDDD",              SDDD,                 0,             0 },
    { "SCRDOORD",          SCRDOORD,             0,             0 },
    { "GAMEDEATHS",        GAMEDEATHS,           0,             0 },
    { "GAMECHEAT",         GAMECHEAT,            0,             0 },
    { "GAMETIME",          GAMETIME,             0,             0 },
    { "GAMEHASPLAYED",     GAMEHASPLAYED,        0,             0 },
    { "GAMETIMEVALID",     GAMETIMEVALID,        0,             0 },
    { "GAMEGUYCOUNT",      GAMEGUYCOUNT,         0,             0 },
    { "GAMECONTSCR",       GAMECONTSCR,          0,             0 },
    { "GAMECONTDMAP",      GAMECONTDMAP,         0,             0 },
    { "GAMECOUNTERD",      GAMECOUNTERD,         0,             0 },
    { "GAMEMCOUNTERD",     GAMEMCOUNTERD,        0,             0 },
    { "GAMEDCOUNTERD",     GAMEDCOUNTERD,        0,             0 },
    { "GAMEGENERICD",      GAMEGENERICD,         0,             0 },
    { "GAMEMISC",      GAMEMISC,         0,             0 },
    { "GAMEITEMSD",        GAMEITEMSD,           0,             0 },
    { "GAMELITEMSD",       GAMELITEMSD,          0,             0 },
    { "GAMELKEYSD",        GAMELKEYSD,           0,             0 },
    { "SCREENSTATED",      SCREENSTATED,         0,             0 },
    { "SCREENSTATEDD",     SCREENSTATEDD,        0,             0 },
    { "GAMEGUYCOUNTD",     GAMEGUYCOUNTD,        0,             0 },
    { "CURMAP",            CURMAP,               0,             0 },
    { "CURSCR",            CURSCR,               0,             0 },
    { "CURDSCR",           CURDSCR,              0,             0 },
    { "CURDMAP",           CURDMAP,              0,             0 },
    { "COMBODD",           COMBODD,              0,             0 },
    { "COMBOCD",           COMBOCD,              0,             0 },
    { "COMBOFD",           COMBOFD,              0,             0 },
    { "COMBOTD",           COMBOTD,              0,             0 },
    { "COMBOID",           COMBOID,              0,             0 },
    { "COMBOSD",           COMBOSD,              0,             0 },
    { "REFITEMCLASS",      REFITEMCLASS,         0,             0 },
    { "REFITEM",           REFITEM,              0,             0 },
    { "REFFFC",            REFFFC,               0,             0 },
    { "REFLWPN",           REFLWPN,              0,             0 },
    { "REFEWPN",           REFEWPN,              0,             0 },
    { "REFLWPNCLASS",      REFLWPNCLASS,         0,             0 },
    { "REFEWPNCLASS",      REFEWPNCLASS,         0,             0 },
    { "REFNPC",            REFNPC,               0,             0 },
    { "REFNPCCLASS",       REFNPCCLASS,          0,             0 },
    { "LWPNX",             LWPNX,                0,             0 },
    { "LWPNY",             LWPNY,                0,             0 },
    { "LWPNZ",             LWPNZ,                0,             0 },
    { "LWPNJUMP",          LWPNJUMP,             0,             0 },
    { "LWPNDIR",           LWPNDIR,              0,             0 },
    { "LWPNSTEP",          LWPNSTEP,             0,             0 },
    { "LWPNANGULAR",       LWPNANGULAR,          0,             0 },
    { "LWPNANGLE",         LWPNANGLE,            0,             0 },
    { "LWPNDRAWTYPE",      LWPNDRAWTYPE,         0,             0 },
    { "LWPNPOWER",         LWPNPOWER,            0,             0 },
    { "LWPNDEAD",          LWPNDEAD,             0,             0 },
    { "LWPNID",            LWPNID,               0,             0 },
    { "LWPNTILE",          LWPNTILE,             0,             0 },
    { "LWPNCSET",          LWPNCSET,             0,             0 },
    { "LWPNFLASHCSET",     LWPNFLASHCSET,        0,             0 },
    { "LWPNFRAMES",        LWPNFRAMES,           0,             0 },
    { "LWPNFRAME",         LWPNFRAME,            0,             0 },
    { "LWPNASPEED",        LWPNASPEED,           0,             0 },
    { "LWPNFLASH",         LWPNFLASH,            0,             0 },
    { "LWPNFLIP",          LWPNFLIP,             0,             0 },
    { "LWPNCOUNT",         LWPNCOUNT,            0,             0 },
    { "LWPNEXTEND",        LWPNEXTEND,           0,             0 },
    { "LWPNOTILE",         LWPNOTILE,            0,             0 },
    { "LWPNOCSET",         LWPNOCSET,            0,             0 },
    { "EWPNX",             EWPNX,                0,             0 },
    { "EWPNY",             EWPNY,                0,             0 },
    { "EWPNZ",             EWPNZ,                0,             0 },
    { "EWPNJUMP",          EWPNJUMP,             0,             0 },
    { "EWPNDIR",           EWPNDIR,              0,             0 },
    { "EWPNSTEP",          EWPNSTEP,             0,             0 },
    { "EWPNANGULAR",       EWPNANGULAR,          0,             0 },
    { "EWPNANGLE",         EWPNANGLE,            0,             0 },
    { "EWPNDRAWTYPE",      EWPNDRAWTYPE,         0,             0 },
    { "EWPNPOWER",         EWPNPOWER,            0,             0 },
    { "EWPNDEAD",          EWPNDEAD,             0,             0 },
    { "EWPNID",            EWPNID,               0,             0 },
    { "EWPNTILE",          EWPNTILE,             0,             0 },
    { "EWPNCSET",          EWPNCSET,             0,             0 },
    { "EWPNFLASHCSET",     EWPNFLASHCSET,        0,             0 },
    { "EWPNFRAMES",        EWPNFRAMES,           0,             0 },
    { "EWPNFRAME",         EWPNFRAME,            0,             0 },
    { "EWPNASPEED",        EWPNASPEED,           0,             0 },
    { "EWPNFLASH",         EWPNFLASH,            0,             0 },
    { "EWPNFLIP",          EWPNFLIP,             0,             0 },
    { "EWPNCOUNT",         EWPNCOUNT,            0,             0 },
    { "EWPNEXTEND",        EWPNEXTEND,           0,             0 },
    { "EWPNOTILE",         EWPNOTILE,            0,             0 },
    { "EWPNOCSET",         EWPNOCSET,            0,             0 },
    { "NPCEXTEND",         NPCEXTEND,            0,             0 },
    { "SP",                SP,                   0,             0 },
    { "SP",                SP,                   0,             0 },
    { "WAVY",              WAVY,                 0,             0 },
    { "QUAKE",             QUAKE,                0,             0 },
    { "IDATAUSESOUND",     IDATAUSESOUND,        0,             0 },
    { "INPUTMOUSEZ",       INPUTMOUSEZ,          0,             0 },
    { "INPUTMOUSEB",       INPUTMOUSEB,          0,             0 },
    { "COMBODDM",          COMBODDM,             0,             0 },
    { "COMBOCDM",           COMBOCDM,            0,             0 },
    { "COMBOFDM",           COMBOFDM,            0,             0 },
    { "COMBOTDM",           COMBOTDM,            0,             0 },
    { "COMBOIDM",           COMBOIDM,            0,             0 },
    { "COMBOSDM",           COMBOSDM,            0,             0 },
    { "SCRIPTRAM",          SCRIPTRAM,           0,             0 },
    { "GLOBALRAM",          GLOBALRAM,           0,             0 },
    { "SCRIPTRAMD",         SCRIPTRAMD,          0,             0 },
    { "GLOBALRAMD",         GLOBALRAMD,          0,             0 },
    { "LWPNHXOFS",          LWPNHXOFS,           0,             0 },
    { "LWPNHYOFS",          LWPNHYOFS,           0,             0 },
    { "LWPNXOFS",           LWPNXOFS,            0,             0 },
    { "LWPNYOFS",           LWPNYOFS,            0,             0 },
    { "LWPNZOFS",           LWPNZOFS,            0,             0 },
    { "LWPNHXSZ",           LWPNHXSZ,            0,             0 },
    { "LWPNHYSZ",           LWPNHYSZ,            0,             0 },
    { "LWPNHZSZ",           LWPNHZSZ,            0,             0 },
    { "EWPNHXOFS",          EWPNHXOFS,           0,             0 },
    { "EWPNHYOFS",          EWPNHYOFS,           0,             0 },
    { "EWPNXOFS",           EWPNXOFS,            0,             0 },
    { "EWPNYOFS",           EWPNYOFS,            0,             0 },
    { "EWPNZOFS",           EWPNZOFS,            0,             0 },
    { "EWPNHXSZ",           EWPNHXSZ,            0,             0 },
    { "EWPNHYSZ",           EWPNHYSZ,            0,             0 },
    { "EWPNHZSZ",           EWPNHZSZ,            0,             0 },
    { "NPCHXOFS",           NPCHXOFS,            0,             0 },
    { "NPCHYOFS",           NPCHYOFS,            0,             0 },
    { "NPCXOFS",            NPCXOFS,             0,             0 },
    { "NPCYOFS",            NPCYOFS,             0,             0 },
    { "NPCZOFS",            NPCZOFS,             0,             0 },
    { "NPCHXSZ",            NPCHXSZ,             0,             0 },
    { "NPCHYSZ",            NPCHYSZ,             0,             0 },
    { "NPCHZSZ",            NPCHZSZ,             0,             0 },
    { "ITEMHXOFS",          ITEMHXOFS,           0,             0 },
    { "ITEMHYOFS",          ITEMHYOFS,           0,             0 },
    { "ITEMXOFS",           ITEMXOFS,            0,             0 },
    { "ITEMYOFS",           ITEMYOFS,            0,             0 },
    { "ITEMZOFS",           ITEMZOFS,            0,             0 },
    { "ITEMHXSZ",           ITEMHXSZ,            0,             0 },
    { "ITEMHYSZ",           ITEMHYSZ,            0,             0 },
    { "ITEMHZSZ",           ITEMHZSZ,            0,             0 },
    { "LWPNTXSZ",           LWPNTXSZ,            0,             0 },
    { "LWPNTYSZ",           LWPNTYSZ,            0,             0 },
    { "EWPNTXSZ",           EWPNTXSZ,            0,             0 },
    { "EWPNTYSZ",           EWPNTYSZ,            0,             0 },
    { "NPCTXSZ",            NPCTXSZ,             0,             0 },
    { "NPCTYSZ",            NPCTYSZ,             0,             0 },
    { "ITEMTXSZ",           ITEMTXSZ,            0,             0 },
    { "ITEMTYSZ",           ITEMTYSZ,            0,             0 },
    { "LINKHXOFS",          LINKHXOFS,           0,             0 },
    { "LINKHYOFS",          LINKHYOFS,           0,             0 },
    { "LINKXOFS",           LINKXOFS,            0,             0 },
    { "LINKYOFS",           LINKYOFS,            0,             0 },
    { "LINKZOFS",           LINKZOFS,            0,             0 },
    { "LINKHXSZ",           LINKHXSZ,            0,             0 },
    { "LINKHYSZ",           LINKHYSZ,            0,             0 },
    { "LINKHZSZ",           LINKHZSZ,            0,             0 },
    { "LINKTXSZ",           LINKTXSZ,            0,             0 },
    { "LINKTYSZ",           LINKTYSZ,            0,             0 },
    { "NPCTILE",            NPCTILE,             0,             0 },
    { "LWPNBEHIND",         LWPNBEHIND,          0,             0 },
    { "EWPNBEHIND",         EWPNBEHIND,          0,             0 },
    { "SDDDD",              SDDDD,               0,             0 },
    { "CURLEVEL",           CURLEVEL,            0,             0 },
    { "ITEMPICKUP",         ITEMPICKUP,          0,             0 },
    { "INPUTMAP",           INPUTMAP,            0,             0 },
    { "LIT",                LIT,                 0,             0 },
    { "INPUTEX1",           INPUTEX1,            0,             0 },
    { "INPUTEX2",           INPUTEX2,            0,             0 },
    { "INPUTEX3",           INPUTEX3,            0,             0 },
    { "INPUTEX4",           INPUTEX4,            0,             0 },
    { "INPUTPRESSSTART",    INPUTPRESSSTART,     0,             0 },
    { "INPUTPRESSUP",       INPUTPRESSUP,        0,             0 },
    { "INPUTPRESSDOWN",     INPUTPRESSDOWN,      0,             0 },
    { "INPUTPRESSLEFT",     INPUTPRESSLEFT,      0,             0 },
    { "INPUTPRESSRIGHT",    INPUTPRESSRIGHT,     0,             0 },
    { "INPUTPRESSA",        INPUTPRESSA,         0,             0 },
    { "INPUTPRESSB",        INPUTPRESSB,         0,             0 },
    { "INPUTPRESSL",        INPUTPRESSL,         0,             0 },
    { "INPUTPRESSR",        INPUTPRESSR,         0,             0 },
    { "INPUTPRESSEX1",      INPUTPRESSEX1,       0,             0 },
    { "INPUTPRESSEX2",      INPUTPRESSEX2,       0,             0 },
    { "INPUTPRESSEX3",      INPUTPRESSEX3,       0,             0 },
    { "INPUTPRESSEX4",      INPUTPRESSEX4,       0,             0 },
    { "LWPNMISCD",          LWPNMISCD,           0,             0 },
    { "EWPNMISCD",          EWPNMISCD,           0,             0 },
    { "NPCMISCD",           NPCMISCD,            0,             0 },
    { "ITEMMISCD",          ITEMMISCD,           0,             0 },
    { "FFMISCD",            FFMISCD,             0,             0 },
    { "GETMIDI",            GETMIDI,             0,             0 },
    { "NPCHOMING",          NPCHOMING,           0,             0 },
    { "NPCDD",			  NPCDD,			   0,             0 },
    { "LINKEQUIP",		  LINKEQUIP,		   0,             0 },
    { "INPUTAXISUP",        INPUTAXISUP,         0,             0 },
    { "INPUTAXISDOWN",      INPUTAXISDOWN,       0,             0 },
    { "INPUTAXISLEFT",      INPUTAXISLEFT,       0,             0 },
    { "INPUTAXISRIGHT",     INPUTAXISRIGHT,      0,             0 },
    { "PRESSAXISUP",        INPUTPRESSAXISUP,    0,             0 },
    { "PRESSAXISDOWN",      INPUTPRESSAXISDOWN,  0,             0 },
    { "PRESSAXISLEFT",      INPUTPRESSAXISLEFT,  0,             0 },
    { "PRESSAXISRIGHT",     INPUTPRESSAXISRIGHT, 0,             0 },
    { "NPCTYPE",			  NPCTYPE,             0,             0 },
    { "FFSCRIPT",			  FFSCRIPT,            0,             0 },
    { "SCREENFLAGSD",       SCREENFLAGSD,        0,             0 },
    { "LINKINVIS",          LINKINVIS,           0,             0 },
    { "LINKINVINC",         LINKINVINC,          0,             0 },
    { "SCREENEFLAGSD",      SCREENEFLAGSD,       0,             0 },
    { "NPCMFLAGS",          NPCMFLAGS,           0,             0 },
    { "FFINITDD",           FFINITDD,            0,             0 },
    { "LINKMISCD",          LINKMISCD,           0,             0 },
    { "DMAPFLAGSD",         DMAPFLAGSD,          0,             0 },
    { "LWPNCOLLDET",        LWPNCOLLDET,         0,             0 },
    { "EWPNCOLLDET",        EWPNCOLLDET,         0,             0 },
    { "NPCCOLLDET",         NPCCOLLDET,          0,             0 },
    { "LINKLADDERX",        LINKLADDERX,         0,             0 },
    { "LINKLADDERY",        LINKLADDERY,         0,             0 },
    { "NPCSTUN",            NPCSTUN,             0,             0 },
    { "NPCDEFENSED",        NPCDEFENSED,         0,             0 },
    { "IDATAPOWER",         IDATAPOWER,          0,             0 },
    { "DMAPLEVELD",         DMAPLEVELD,          0,             0 },
    { "DMAPCOMPASSD",       DMAPCOMPASSD,        0,             0 },
    { "DMAPCONTINUED",      DMAPCONTINUED,       0,             0 },
    { "DMAPMIDID",          DMAPMIDID,           0,             0 },
    { "IDATAINITDD",        IDATAINITDD,         0,             0 },
    { "ROOMTYPE",           ROOMTYPE,            0,             0 },
    { "ROOMDATA",           ROOMDATA,            0,             0 },
    { "LINKTILE",           LINKTILE,            0,             0 },
    { "LINKFLIP",           LINKFLIP,            0,             0 },
    { "INPUTPRESSMAP",      INPUTPRESSMAP,       0,             0 },
    { "NPCHUNGER",          NPCHUNGER,           0,             0 },
    { "GAMESTANDALONE",     GAMESTANDALONE,      0,             0 },
    { "GAMEENTRSCR",        GAMEENTRSCR,         0,             0 },
    { "GAMEENTRDMAP",       GAMEENTRDMAP,        0,             0 },
    { "GAMECLICKFREEZE",    GAMECLICKFREEZE,     0,             0 },
    { "PUSHBLOCKX",         PUSHBLOCKX,          0,             0 },
    { "PUSHBLOCKY",         PUSHBLOCKY,          0,             0 },
    { "PUSHBLOCKCOMBO",     PUSHBLOCKCOMBO,      0,             0 },
    { "PUSHBLOCKCSET",      PUSHBLOCKCSET,       0,             0 },
    { "UNDERCOMBO",         UNDERCOMBO,          0,             0 },
    { "UNDERCSET",          UNDERCSET,           0,             0 },
    { "DMAPOFFSET",         DMAPOFFSET,          0,             0 },
    { "DMAPMAP",            DMAPMAP,             0,             0 },
    { "__RESERVED_FOR_GAMETHROTTLE",         __RESERVED_FOR_GAMETHROTTLE,            0,             0 },
	{ "REFMAPDATA",       REFMAPDATA,          0,             0 },
	{ "REFSCREENDATA",       REFSCREENDATA,          0,             0 },
	{ "REFCOMBODATA",       REFCOMBODATA,          0,             0 },
	{ "REFSPRITEDATA",       REFSPRITEDATA,          0,             0 },
	{ "REFBITMAP",       REFBITMAP,          0,             0 },
	{ "REFDMAPDATA",         REFDMAPDATA,            0,             0 },
	{ "REFSHOPDATA",         REFSHOPDATA,            0,             0 },
	{ "REFMSGDATA",         REFMSGDATA,            0,             0 },
	{ "REFUNTYPED",         REFUNTYPED,            0,             0 },
	{ "REFDROPS",         REFDROPS,            0,             0 },
	{ "REFPONDS",         REFPONDS,            0,             0 },
	{ "REFWARPRINGS",         REFWARPRINGS,            0,             0 },
	{ "REFDOORS",         REFDOORS,            0,             0 },
	{ "REFUICOLOURS",         REFUICOLOURS,            0,             0 },
	{ "REFRGB",         REFRGB,            0,             0 },
	{ "REFPALETTE",         REFPALETTE,            0,             0 },
	{ "REFTUNES",         REFTUNES,            0,             0 },
	{ "REFPALCYCLE",         REFPALCYCLE,            0,             0 },
	{ "REFGAMEDATA",         REFGAMEDATA,            0,             0 },
	{ "REFCHEATS",         REFCHEATS,            0,             0 },
	{ "IDATAMAGICTIMER",         IDATAMAGICTIMER,            0,             0 },
	{ "IDATALTM",         IDATALTM,            0,             0 },
	{ "IDATASCRIPT",         IDATASCRIPT,            0,             0 },
	{ "IDATAPSCRIPT",         IDATAPSCRIPT,            0,             0 },
	{ "IDATAMAGCOST",         IDATAMAGCOST,            0,             0 },
	{ "IDATAMINHEARTS",         IDATAMINHEARTS,            0,             0 },
	{ "IDATATILE",         IDATATILE,            0,             0 },
	{ "IDATAMISC",         IDATAMISC,            0,             0 },
	{ "IDATACSET",         IDATACSET,            0,             0 },
	{ "IDATAFRAMES",         IDATAFRAMES,            0,             0 },
	{ "IDATAASPEED",         IDATAASPEED,            0,             0 },
	{ "IDATADELAY",         IDATADELAY,            0,             0 },
	{ "IDATACOMBINE",         IDATACOMBINE,            0,             0 },
	{ "IDATADOWNGRADE",         IDATADOWNGRADE,            0,             0 },
	{ "IDATAPSTRING",         IDATAPSTRING,            0,             0 },
	{ "RESVD0023",         RESVD023,            0,             0 },
	{ "IDATAKEEPOLD",         IDATAKEEPOLD,            0,             0 },
	{ "IDATARUPEECOST",         IDATARUPEECOST,            0,             0 },
	{ "IDATAEDIBLE",         IDATAEDIBLE,            0,             0 },
	{ "IDATAFLAGUNUSED",         IDATAFLAGUNUSED,            0,             0 },
	{ "IDATAGAINLOWER",         IDATAGAINLOWER,            0,             0 },
	{ "RESVD0024",         RESVD024,            0,             0 },
	{ "RESVD0025",         RESVD025,            0,             0 },
	{ "RESVD0026",         RESVD026,            0,             0 },
	{ "IDATAID",         IDATAID,            0,             0 },
    { "__RESERVED_FOR_LINKEXTEND",         __RESERVED_FOR_LINKEXTEND,            0,             0 },
	{ "NPCSCRDEFENSED",        NPCSCRDEFENSED,         0,             0 },
	{ "__RESERVED_FOR_SETLINKTILE",              __RESERVED_FOR_SETLINKTILE,                 0,             0 },
	{ "__RESERVED_FOR_SETLINKEXTEND",           __RESERVED_FOR_SETLINKEXTEND,            0,             0 },
	{ "__RESERVED_FOR_SIDEWARPSFX",           __RESERVED_FOR_SIDEWARPSFX,            0,             0 },
	{ "__RESERVED_FOR_PITWARPSFX",           __RESERVED_FOR_PITWARPSFX,            0,             0 },
	{ "__RESERVED_FOR_SIDEWARPVISUAL",           __RESERVED_FOR_SIDEWARPVISUAL,            0,             0 },
	{ "__RESERVED_FOR_PITWARPVISUAL",           __RESERVED_FOR_PITWARPVISUAL,            0,             0 },
	{ "GAMESETA",           GAMESETA,            0,             0 },
	{ "GAMESETB",           GAMESETB,            0,             0 },
	{ "SETITEMSLOT",           SETITEMSLOT,            0,             0 },
	{ "LINKITEMB",           LINKITEMB,            0,             0 },
	{ "LINKITEMA",           LINKITEMA,            0,             0 },
	{ "__RESERVED_FOR_LINKWALKTILE",           __RESERVED_FOR_LINKWALKTILE,            0,             0 }, //Walk sprite
	{ "__RESERVED_FOR_LINKFLOATTILE",           __RESERVED_FOR_LINKFLOATTILE,            0,             0 }, //float sprite
	{ "__RESERVED_FOR_LINKSWIMTILE",           __RESERVED_FOR_LINKSWIMTILE,            0,             0 }, //swim sprite
	{ "__RESERVED_FOR_LINKDIVETILE",           __RESERVED_FOR_LINKDIVETILE,            0,             0 }, //dive sprite
	{ "__RESERVED_FOR_LINKSLASHTILE",           __RESERVED_FOR_LINKSLASHTILE,            0,             0 }, //slash sprite
	{ "__RESERVED_FOR_LINKJUMPTILE",           __RESERVED_FOR_LINKJUMPTILE,            0,             0 }, //jump sprite
	{ "__RESERVED_FOR_LINKCHARGETILE",           __RESERVED_FOR_LINKCHARGETILE,            0,             0 }, //charge sprite
	{ "__RESERVED_FOR_LINKSTABTILE",           __RESERVED_FOR_LINKSTABTILE,            0,             0 }, //stab sprite
	{ "__RESERVED_FOR_LINKCASTTILE",           __RESERVED_FOR_LINKCASTTILE,            0,             0 }, //casting sprite
	{ "__RESERVED_FOR_LINKHOLD1LTILE",           __RESERVED_FOR_LINKHOLD1LTILE,            0,             0 }, //hold1land sprite
	{ "__RESERVED_FOR_LINKHOLD2LTILE",           __RESERVED_FOR_LINKHOLD2LTILE,            0,             0 }, //hold2land sprite
	{ "__RESERVED_FOR_LINKHOLD1WTILE",           __RESERVED_FOR_LINKHOLD1WTILE,            0,             0 }, //hold1water sprite
	{ "__RESERVED_FOR_LINKHOLD2WTILE",           __RESERVED_FOR_LINKHOLD2WTILE,            0,             0 }, //hold2water sprite
	{ "__RESERVED_FOR_LINKPOUNDTILE",           __RESERVED_FOR_LINKPOUNDTILE,            0,             0 }, //hammer pound sprite
	{ "__RESERVED_FOR_LINKSWIMSPD",           __RESERVED_FOR_LINKSWIMSPD,            0,             0 },
	{ "__RESERVED_FOR_LINKWALKANMSPD",           __RESERVED_FOR_LINKWALKANMSPD,            0,             0 },
	{ "__RESERVED_FOR_LINKANIMTYPE",           __RESERVED_FOR_LINKANIMTYPE,            0,             0 },
	{ "LINKINVFRAME",           LINKINVFRAME,            0,             0 },
	{ "LINKCANFLICKER",           LINKCANFLICKER,            0,             0 },
	{ "LINKHURTSFX",           LINKHURTSFX,            0,             0 },
	{ "NOACTIVESUBSC",           NOACTIVESUBSC,            0,             0 },
	{ "LWPNRANGE",         LWPNRANGE,            0,             0 },
	{ "ZELDAVERSION",         ZELDAVERSION,            0,             0 },
	{ "ZELDABUILD",         ZELDABUILD,            0,             0 },
	{ "ZELDABETA",         ZELDABETA,            0,             0 },
	{ "NPCINVINC",         NPCINVINC,            0,             0 },
	{ "NPCSUPERMAN",         NPCSUPERMAN,            0,             0 },
	{ "NPCHASITEM",         NPCHASITEM,            0,             0 },
	{ "NPCRINGLEAD",         NPCRINGLEAD,            0,             0 },
	{ "IDATAFRAME",         IDATAFRAME,            0,             0 },
	{ "ITEMACLK",         ITEMACLK,            0,             0 },
	{ "FFCID",         FFCID,            0,             0 },
	{ "IDATAATTRIB",         IDATAATTRIB,            0,             0 },
	{ "IDATASPRITE",         IDATASPRITE,            0,             0 },
	{ "IDATAFLAGS",         IDATAFLAGS,            0,             0 },
	{ "DMAPLEVELPAL",	DMAPLEVELPAL,          0,             0 },
	{ "__RESERVED_FOR_ITEMPTR",         __RESERVED_FOR_ITEMPTR,          0,             0 },
	{ "__RESERVED_FOR_NPCPTR",         __RESERVED_FOR_NPCPTR,          0,             0 },
	{ "__RESERVED_FOR_LWPNPTR",         __RESERVED_FOR_LWPNPTR,          0,             0 },
	{ "__RESERVED_FOR_EWPNPTR",         __RESERVED_FOR_EWPNPTR,          0,             0 },
	{ "SETSCREENDOOR",           SETSCREENDOOR,            0,             0 },
	{ "SETSCREENENEMY",           SETSCREENENEMY,            0,             0 },
	{ "GAMEMAXMAPS",          GAMEMAXMAPS,              0,             0 },
	{ "CREATELWPNDX", CREATELWPNDX, 0, 0 },
	{ "__RESERVED_FOR_SCREENFLAG",     __RESERVED_FOR_SCREENFLAG,        0,             0 },
	{ "BUTTONPRESS",	BUTTONPRESS,        0,             0 },
	{ "BUTTONINPUT",	BUTTONINPUT,        0,             0 },
	{ "BUTTONHELD",		BUTTONHELD,        0,             0 },
	{ "KEYPRESS",		KEYPRESS,        0,             0 },
	{ "READKEY",		READKEY,        0,             0 },
	{ "JOYPADPRESS",	JOYPADPRESS,        0,             0 },
	{ "DISABLEDITEM",	DISABLEDITEM,            0,             0 },
	{ "LINKDIAG",           LINKDIAG,            0,             0 },
	{ "LINKBIGHITBOX",           LINKBIGHITBOX,            0,             0 },
	{ "LINKEATEN", LINKEATEN, 0, 0 },
	{ "__RESERVED_FOR_LINKRETSQUARE", __RESERVED_FOR_LINKRETSQUARE, 0, 0 },
	{ "__RESERVED_FOR_LINKWARPSOUND", __RESERVED_FOR_LINKWARPSOUND, 0, 0 },
	{ "__RESERVED_FOR_PLAYPITWARPSFX", __RESERVED_FOR_PLAYPITWARPSFX, 0, 0 },
	{ "__RESERVED_FOR_WARPEFFECT", __RESERVED_FOR_WARPEFFECT, 0, 0 },
	{ "__RESERVED_FOR_PLAYWARPSOUND", __RESERVED_FOR_PLAYWARPSOUND, 0, 0 },
	{ "LINKUSINGITEM", LINKUSINGITEM, 0, 0 },
	{ "LINKUSINGITEMA", LINKUSINGITEMA, 0, 0 },
	{ "LINKUSINGITEMB", LINKUSINGITEMB, 0, 0 },
	//    { "DMAPLEVELPAL",         DMAPLEVELPAL,          0,             0 },
	//{ "LINKZHEIGHT",           LINKZHEIGHT,            0,             0 },
	    //{ "ITEMINDEX",         ITEMINDEX,          0,             0 },
	    //{ "LWPNINDEX",         LWPNINDEX,          0,             0 },
	    //{ "EWPNINDEX",         EWPNINDEX,          0,             0 },
	    //{ "NPCINDEX",         NPCINDEX,          0,             0 },
	    //TABLE END
	{ "IDATAUSEWPN", IDATAUSEWPN, 0, 0 }, //UseWeapon
	{ "IDATAUSEDEF", IDATAUSEDEF, 0, 0 }, //UseDefense
	{ "IDATAWRANGE", IDATAWRANGE, 0, 0 }, //Range
	{ "IDATAUSEMVT", IDATAUSEMVT, 0, 0 }, //Movement[]
	{ "IDATADURATION", IDATADURATION, 0, 0 }, //Duration
	
	{ "IDATADUPLICATES", IDATADUPLICATES, 0, 0 }, //Duplicates
	  { "IDATADRAWLAYER", IDATADRAWLAYER, 0, 0 }, //DrawLayer
	  { "IDATACOLLECTFLAGS", IDATACOLLECTFLAGS, 0, 0 }, //CollectFlags
	  { "IDATAWEAPONSCRIPT", IDATAWEAPONSCRIPT, 0, 0 }, //WeaponScript
	  { "IDATAMISCD", IDATAMISCD, 0, 0 }, //WeaponMisc[32]
	  { "IDATAWEAPHXOFS", IDATAWEAPHXOFS, 0, 0 }, //WeaponHitXOffset
	  { "IDATAWEAPHYOFS", IDATAWEAPHYOFS, 0, 0 }, //WeaponHitYOffset
	  { "IDATAWEAPHXSZ", IDATAWEAPHYSZ, 0, 0 }, //WeaponHitWidth
	  { "IDATAWEAPHYSZ", IDATAWEAPHYSZ, 0, 0 }, //WeaponHitHeight
	  { "IDATAWEAPHZSZ", IDATAWEAPHZSZ, 0, 0 }, //WeaponHitZHeight
	  { "IDATAWEAPXOFS", IDATAWEAPXOFS, 0, 0 }, //WeaponDrawXOffset
	  { "IDATAWEAPYOFS", IDATAWEAPYOFS, 0, 0 }, //WeaponDrawYOffset
	  { "IDATAWEAPZOFS", IDATAWEAPZOFS, 0, 0 }, //WeaponDrawZOffset
	  { "IDATAWPNINITD", IDATAWPNINITD, 0, 0 }, //WeaponD[8]
	  
	  { "NPCWEAPSPRITE", NPCWEAPSPRITE, 0, 0 }, //WeaponSprite
	  
	  { "DEBUGREFFFC", DEBUGREFFFC, 0, 0 }, //REFFFC
	  { "DEBUGREFITEM", DEBUGREFITEM, 0, 0 }, //REFITEM
	  { "DEBUGREFNPC", DEBUGREFNPC, 0, 0 }, //REFNPC
	  { "DEBUGREFITEMDATA", DEBUGREFITEMDATA, 0, 0 }, //REFITEMCLASS
	   { "DEBUGREFLWEAPON", DEBUGREFLWEAPON, 0, 0 }, //REFLWPN
	    { "DEBUGREFEWEAPON", DEBUGREFEWEAPON, 0, 0 }, //REFEWPN
	    { "DEBUGSP", DEBUGSP, 0, 0 }, //SP
	    { "DEBUGGDR", DEBUGGDR, 0, 0 }, //GDR[256]
	  { "SCREENWIDTH",              SCREENWIDTH,                 0,             0 },
	{ "SCREENHEIGHT",              SCREENHEIGHT,                 0,             0 },
	{ "SCREENVIEWX",              SCREENVIEWX,                 0,             0 },
	{ "SCREENVIEWY",              SCREENVIEWY,                 0,             0 },
	{ "SCREENGUY",              SCREENGUY,                 0,             0 },
	{ "SCREENSTRING",              SCREENSTRING,                 0,             0 },
	{ "SCREENROOM",              SCREENROOM,                 0,             0 },
	{ "SCREENENTX",              SCREENENTX,                 0,             0 },
	{ "SCREENENTY",              SCREENENTY,                 0,             0 },
	{ "SCREENITEM",              SCREENITEM,                 0,             0 },
	{ "SCREENUNDCMB",              SCREENUNDCMB,                 0,             0 },
	{ "SCREENUNDCST",              SCREENUNDCST,                 0,             0 },
	{ "SCREENCATCH",              SCREENCATCH,                 0,             0 },
	{ "SETSCREENLAYOP",              SETSCREENLAYOP,                 0,             0 },
	{ "SETSCREENSECCMB",              SETSCREENSECCMB,                 0,             0 },
	{ "SETSCREENSECCST",              SETSCREENSECCST,                 0,             0 },
	{ "SETSCREENSECFLG",              SETSCREENSECFLG,                 0,             0 },
	{ "SETSCREENLAYMAP",              SETSCREENLAYMAP,                 0,             0 },
	{ "SETSCREENLAYSCR",              SETSCREENLAYSCR,                 0,             0 },
	{ "SETSCREENPATH",              SETSCREENPATH,                 0,             0 },
	{ "SETSCREENWARPRX",              SETSCREENWARPRX,                 0,             0 },
	{ "SETSCREENWARPRY",              SETSCREENWARPRY,                 0,             0 },
	{"GAMENUMMESSAGES", GAMENUMMESSAGES, 0, 0 },
	{"GAMESUBSCHEIGHT", GAMESUBSCHEIGHT, 0, 0 },
	{"GAMEPLAYFIELDOFS", GAMEPLAYFIELDOFS, 0, 0 },
	{"PASSSUBOFS", PASSSUBOFS, 0, 0 },
	

	//NPCData
	{"SETNPCDATASCRIPTDEF", SETNPCDATASCRIPTDEF, 0, 0 },
	{"SETNPCDATADEFENSE", SETNPCDATADEFENSE, 0, 0 },
	{"SETNPCDATASIZEFLAG", SETNPCDATASIZEFLAG, 0, 0 },
	{"SETNPCDATAATTRIBUTE", SETNPCDATAATTRIBUTE, 0, 0 },
	
	{"SCDBLOCKWEAPON", SCDBLOCKWEAPON, 0, 0 },
	{"SCDSTRIKEWEAPONS", SCDSTRIKEWEAPONS, 0, 0 },
	{"SCDEXPANSION", SCDEXPANSION, 0, 0 },
	{"SETGAMEOVERELEMENT", SETGAMEOVERELEMENT, 0, 0 },
	{"SETGAMEOVERSTRING", SETGAMEOVERSTRING, 0, 0 },
	{"MOUSEARR", MOUSEARR, 0, 0 },
	
	{"IDATAOVERRIDEFLWEAP", IDATAOVERRIDEFLWEAP, 0, 0 },
	{"IDATATILEHWEAP", IDATATILEHWEAP, 0, 0 },
	{"IDATATILEWWEAP", IDATATILEWWEAP, 0, 0 },
	{"IDATAHZSZWEAP", IDATAHZSZWEAP, 0, 0 },
	{"IDATAHYSZWEAP", IDATAHYSZWEAP, 0, 0 },
	{"IDATAHXSZWEAP", IDATAHXSZWEAP, 0, 0 },
	{"IDATADYOFSWEAP", IDATADYOFSWEAP, 0, 0 },
	{"IDATADXOFSWEAP", IDATADXOFSWEAP, 0, 0 },
	{"IDATAHYOFSWEAP", IDATAHYOFSWEAP, 0, 0 },
	{"IDATAHXOFSWEAP", IDATAHXOFSWEAP, 0, 0 },
	{"IDATAOVERRIDEFL", IDATAOVERRIDEFL, 0, 0 },
	{"IDATAPICKUP", IDATAPICKUP, 0, 0 },
	{"IDATATILEH", IDATATILEH, 0, 0 },
	{"IDATATILEW", IDATATILEW, 0, 0 },
	{"IDATAHZSZ", IDATAHZSZ, 0, 0 },
	{"IDATAHYSZ", IDATAHYSZ, 0, 0 },
	{"IDATAHXSZ", IDATAHXSZ, 0, 0 },
	{"IDATADYOFS", IDATADYOFS, 0, 0 },
	{"IDATADXOFS", IDATADXOFS, 0, 0 },
	{"IDATAHYOFS", IDATAHYOFS, 0, 0 },
	{"IDATAHXOFS", IDATAHXOFS, 0, 0 },
	//spritedata sd->
	{"SPRITEDATATILE", SPRITEDATATILE, 0, 0 },
	{"SPRITEDATAMISC", SPRITEDATAMISC, 0, 0 },
	{"SPRITEDATACSETS", SPRITEDATACSETS, 0, 0 },
	{"SPRITEDATAFRAMES", SPRITEDATAFRAMES, 0, 0 },
	{"SPRITEDATASPEED", SPRITEDATASPEED, 0, 0 },
	{"SPRITEDATATYPE", SPRITEDATATYPE, 0, 0 },
	
	//npcdata nd->
	{"NPCDATATILE", NPCDATATILE, 0, 0 },
	{"NPCDATAHEIGHT", NPCDATAHEIGHT, 0, 0 },
	{"NPCDATAFLAGS", NPCDATAFLAGS, 0, 0 },
	{"NPCDATAFLAGS2", NPCDATAFLAGS2, 0, 0 },
	{"NPCDATAWIDTH", NPCDATAWIDTH, 0, 0 },
	{"NPCDATAHITSFX", NPCDATAHITSFX, 0, 0 },
	{"NPCDATASTILE", NPCDATASTILE, 0, 0 },
	{"NPCDATASWIDTH", NPCDATASWIDTH, 0, 0 },
	{"NPCDATASHEIGHT", NPCDATASHEIGHT, 0, 0 },
	{"NPCDATAETILE", NPCDATAETILE, 0, 0 },
	{"NPCDATAEWIDTH", NPCDATAEWIDTH, 0, 0 },
	{"NPCDATAEHEIGHT", NPCDATAEHEIGHT, 0, 0 },
	{"NPCDATAHP", NPCDATAHP, 0, 0 },
	{"NPCDATAFAMILY", NPCDATAFAMILY, 0, 0 },
	{"NPCDATACSET", NPCDATACSET, 0, 0 },
	{"NPCDATAANIM", NPCDATAANIM, 0, 0 },
	{"NPCDATAEANIM", NPCDATAEANIM, 0, 0 },
	{"NPCDATAFRAMERATE", NPCDATAFRAMERATE, 0, 0 },
	{"NPCDATAEFRAMERATE", NPCDATAEFRAMERATE, 0, 0 },
	{"NPCDATATOUCHDAMAGE", NPCDATATOUCHDAMAGE, 0, 0 },
	{"NPCDATAWEAPONDAMAGE", NPCDATAWEAPONDAMAGE, 0, 0 },
	{"NPCDATAWEAPON", NPCDATAWEAPON, 0, 0 },
	{"NPCDATARANDOM", NPCDATARANDOM, 0, 0 },
	{"NPCDATAHALT", NPCDATAHALT, 0, 0 },
	{"NPCDATASTEP", NPCDATASTEP, 0, 0 },
	{"NPCDATAHOMING", NPCDATAHOMING, 0, 0 },
	{"NPCDATAHUNGER", NPCDATAHUNGER, 0, 0 },
	{"NPCDATADROPSET", NPCDATADROPSET, 0, 0 },
	{"NPCDATABGSFX", NPCDATABGSFX, 0, 0 },
	{"NPCDATADEATHSFX", NPCDATADEATHSFX, 0, 0 },
	{"NPCDATAXOFS", NPCDATAXOFS, 0, 0 },
	{"NPCDATAYOFS", NPCDATAYOFS, 0, 0 },
	{"NPCDATAZOFS", NPCDATAZOFS, 0, 0 },
	{"NPCDATAHXOFS", NPCDATAHXOFS, 0, 0 },
	{"NPCDATAHYOFS", NPCDATAHYOFS, 0, 0 },
	{"NPCDATAHITWIDTH", NPCDATAHITWIDTH, 0, 0 },
	{"NPCDATAHITHEIGHT", NPCDATAHITHEIGHT, 0, 0 },
	{"NPCDATAHITZ", NPCDATAHITZ, 0, 0 },
	{"NPCDATATILEWIDTH", NPCDATATILEWIDTH, 0, 0 },
	{"NPCDATATILEHEIGHT", NPCDATATILEHEIGHT, 0, 0 },
	{"NPCDATAWPNSPRITE", NPCDATAWPNSPRITE, 0, 0 },
	{"NPCDATADEFENSE", NPCDATADEFENSE, 0, 0 },
	{"NPCDATASIZEFLAG", NPCDATASIZEFLAG, 0, 0 },
	{"NPCDATAATTRIBUTE", NPCDATAATTRIBUTE, 0, 0 },
	
	{"NPCDATAFROZENTILE", NPCDATAFROZENTILE, 0, 0 },
	{"NPCDATAFROZENCSET", NPCDATAFROZENCSET, 0, 0 },
	
	//mapdata md->
	{"MAPDATAVALID", MAPDATAVALID, 0, 0 },
	{"MAPDATAGUY", MAPDATAGUY, 0, 0 },
	{"MAPDATASTRING", MAPDATASTRING, 0, 0 },
	{"MAPDATAROOM", MAPDATAROOM, 0, 0 },
	{"MAPDATAITEM", MAPDATAITEM, 0, 0 },
	{"MAPDATAHASITEM", MAPDATAHASITEM, 0, 0 },
	{"MAPDATATILEWARPTYPE", MAPDATATILEWARPTYPE, 0, 0 },
	{"MAPDATATILEWARPOVFLAGS", MAPDATATILEWARPOVFLAGS, 0, 0 },
	{"MAPDATADOORCOMBOSET", MAPDATADOORCOMBOSET, 0, 0 },
	{"MAPDATAWARPRETX", MAPDATAWARPRETX, 0, 0 },
	{"MAPDATAWARPRETY", MAPDATAWARPRETY, 0, 0 },
	{"MAPDATAWARPRETURNC", MAPDATAWARPRETURNC, 0, 0 },
	{"MAPDATASTAIRX", MAPDATASTAIRX, 0, 0 },
	{"MAPDATASTAIRY", MAPDATASTAIRY, 0, 0 },
	{"MAPDATACOLOUR", MAPDATACOLOUR, 0, 0 },
	{"MAPDATAENEMYFLAGS", MAPDATAENEMYFLAGS, 0, 0 },
	{"MAPDATADOOR", MAPDATADOOR, 0, 0 },
	{"MAPDATATILEWARPDMAP", MAPDATATILEWARPDMAP, 0, 0 },
	{"MAPDATATILEWARPSCREEN", MAPDATATILEWARPSCREEN, 0, 0 },
	{"MAPDATAEXITDIR", MAPDATAEXITDIR, 0, 0 },
	{"MAPDATAENEMY", MAPDATAENEMY, 0, 0 },
	{"MAPDATAPATTERN", MAPDATAPATTERN, 0, 0 },
	{"MAPDATASIDEWARPTYPE", MAPDATASIDEWARPTYPE, 0, 0 },
	{"MAPDATASIDEWARPOVFLAGS", MAPDATASIDEWARPOVFLAGS, 0, 0 },
	{"MAPDATAWARPARRIVALX", MAPDATAWARPARRIVALX, 0, 0 },
	{"MAPDATAWARPARRIVALY", MAPDATAWARPARRIVALY, 0, 0 },
	{"MAPDATAPATH", MAPDATAPATH, 0, 0 },
	{"MAPDATASIDEWARPSC", MAPDATASIDEWARPSC, 0, 0 },
	{"MAPDATASIDEWARPDMAP", MAPDATASIDEWARPDMAP, 0, 0 },
	{"MAPDATASIDEWARPINDEX", MAPDATASIDEWARPINDEX, 0, 0 },
	{"MAPDATAUNDERCOMBO", MAPDATAUNDERCOMBO, 0, 0 },
	{"MAPDATAUNDERCSET", MAPDATAUNDERCSET, 0, 0 },
	{"MAPDATACATCHALL", MAPDATACATCHALL, 0, 0 },
	{"MAPDATAFLAGS", MAPDATAFLAGS, 0, 0 },
	{"MAPDATACSENSITIVE", MAPDATACSENSITIVE, 0, 0 },
	{"MAPDATANORESET", MAPDATANORESET, 0, 0 },
	{"MAPDATANOCARRY", MAPDATANOCARRY, 0, 0 },
	{"MAPDATALAYERMAP", MAPDATALAYERMAP, 0, 0 },
	{"MAPDATALAYERSCREEN", MAPDATALAYERSCREEN, 0, 0 },
	{"MAPDATALAYEROPACITY", MAPDATALAYEROPACITY, 0, 0 },
	{"MAPDATATIMEDWARPTICS", MAPDATATIMEDWARPTICS, 0, 0 },
	{"MAPDATANEXTMAP", MAPDATANEXTMAP, 0, 0 },
	{"MAPDATANEXTSCREEN", MAPDATANEXTSCREEN, 0, 0 },
	{"MAPDATASECRETCOMBO", MAPDATASECRETCOMBO, 0, 0 },
	{"MAPDATASECRETCSET", MAPDATASECRETCSET, 0, 0 },
	{"MAPDATASECRETFLAG", MAPDATASECRETFLAG, 0, 0 },
	{"MAPDATAVIEWX", MAPDATAVIEWX, 0, 0 },
	{"MAPDATAVIEWY", MAPDATAVIEWY, 0, 0 },
	{"MAPDATASCREENWIDTH", MAPDATASCREENWIDTH, 0, 0 },
	{"MAPDATASCREENHEIGHT", MAPDATASCREENHEIGHT, 0, 0 },
	{"MAPDATAENTRYX", MAPDATAENTRYX, 0, 0 },
	{"MAPDATAENTRYY", MAPDATAENTRYY, 0, 0 },
	{"MAPDATANUMFF", MAPDATANUMFF, 0, 0 },
	{"MAPDATAFFDATA", MAPDATAFFDATA, 0, 0 },
	{"MAPDATAFFCSET", MAPDATAFFCSET, 0, 0 },
	{"MAPDATAFFDELAY", MAPDATAFFDELAY, 0, 0 },
	{"MAPDATAFFX", MAPDATAFFX, 0, 0 },
	{"MAPDATAFFY", MAPDATAFFY, 0, 0 },
	{"MAPDATAFFXDELTA", MAPDATAFFXDELTA, 0, 0 },
	{"MAPDATAFFYDELTA", MAPDATAFFYDELTA, 0, 0 },
	{"MAPDATAFFXDELTA2", MAPDATAFFXDELTA2, 0, 0 },
	{"MAPDATAFFYDELTA2", MAPDATAFFYDELTA2, 0, 0 },
	{"MAPDATAFFFLAGS", MAPDATAFFFLAGS, 0, 0 },
	{"MAPDATAFFWIDTH", MAPDATAFFWIDTH, 0, 0 },
	{"MAPDATAFFHEIGHT", MAPDATAFFHEIGHT, 0, 0 },
	{"MAPDATAFFLINK", MAPDATAFFLINK, 0, 0 },
	{"MAPDATAFFSCRIPT", MAPDATAFFSCRIPT, 0, 0 },
	{"MAPDATAINTID", MAPDATAINTID, 0, 0 }, //Needs to be a function [32][10]
	{"MAPDATAINITA", MAPDATAINITA, 0, 0 }, //needs to be a function, [32][2]
	{"MAPDATAFFINITIALISED", MAPDATAFFINITIALISED, 0, 0 },
	{"MAPDATASCRIPTENTRY", MAPDATASCRIPTENTRY, 0, 0 },
	{"MAPDATASCRIPTOCCUPANCY", MAPDATASCRIPTOCCUPANCY, 0, 0 },
	{"MAPDATASCRIPTEXIT", MAPDATASCRIPTEXIT, 0, 0 },
	{"MAPDATAOCEANSFX", MAPDATAOCEANSFX, 0, 0 },
	{"MAPDATABOSSSFX", MAPDATABOSSSFX, 0, 0 },
	{"MAPDATASECRETSFX", MAPDATASECRETSFX, 0, 0 },
	{"MAPDATAHOLDUPSFX", MAPDATAHOLDUPSFX, 0, 0 },
	{"MAPDATASCREENMIDI", MAPDATASCREENMIDI, 0, 0 },
	{"MAPDATALENSLAYER", MAPDATALENSLAYER, 0, 0 },
	{"MAPDATAMISCD", MAPDATAMISCD, 0, 0},
	
	{"MAPDATASCREENSTATED", MAPDATASCREENSTATED, 0, 0},
	{"MAPDATASCREENFLAGSD", MAPDATASCREENFLAGSD, 0, 0},
	{"MAPDATASCREENEFLAGSD", MAPDATASCREENEFLAGSD, 0, 0},
	
	
	
	{"MAPDATACOMBODD", MAPDATACOMBODD, 0, 0},
	{"MAPDATACOMBOCD", MAPDATACOMBOCD, 0, 0},
	{"MAPDATACOMBOFD", MAPDATACOMBOFD, 0, 0},
	{"MAPDATACOMBOTD", MAPDATACOMBOTD, 0, 0},
	{"MAPDATACOMBOID", MAPDATACOMBOID, 0, 0},
	{"MAPDATACOMBOSD", MAPDATACOMBOSD, 0, 0},
	
		{"SCREENDATAVALID", SCREENDATAVALID, 0, 0 },
	{"SCREENDATAGUY", SCREENDATAGUY, 0, 0 },
	{"SCREENDATASTRING", SCREENDATASTRING, 0, 0 },
	{"SCREENDATAROOM", SCREENDATAROOM, 0, 0 },
	{"SCREENDATAITEM", SCREENDATAITEM, 0, 0 },
	{"SCREENDATAHASITEM", SCREENDATAHASITEM, 0, 0 },
	{"SCREENDATATILEWARPTYPE", SCREENDATATILEWARPTYPE, 0, 0 },
	{"SCREENDATATILEWARPOVFLAGS", SCREENDATATILEWARPOVFLAGS, 0, 0 },
	{"SCREENDATADOORCOMBOSET", SCREENDATADOORCOMBOSET, 0, 0 },
	{"SCREENDATAWARPRETX", SCREENDATAWARPRETX, 0, 0 },
	{"SCREENDATAWARPRETY", SCREENDATAWARPRETY, 0, 0 },
	{"SCREENDATAWARPRETURNC", SCREENDATAWARPRETURNC, 0, 0 },
	{"SCREENDATASTAIRX", SCREENDATASTAIRX, 0, 0 },
	{"SCREENDATASTAIRY", SCREENDATASTAIRY, 0, 0 },
	{"SCREENDATACOLOUR", SCREENDATACOLOUR, 0, 0 },
	{"SCREENDATAENEMYFLAGS", SCREENDATAENEMYFLAGS, 0, 0 },
	{"SCREENDATADOOR", SCREENDATADOOR, 0, 0 },
	{"SCREENDATATILEWARPDMAP", SCREENDATATILEWARPDMAP, 0, 0 },
	{"SCREENDATATILEWARPSCREEN", SCREENDATATILEWARPSCREEN, 0, 0 },
	{"SCREENDATAEXITDIR", SCREENDATAEXITDIR, 0, 0 },
	{"SCREENDATAENEMY", SCREENDATAENEMY, 0, 0 },
	{"SCREENDATAPATTERN", SCREENDATAPATTERN, 0, 0 },
	{"SCREENDATASIDEWARPTYPE", SCREENDATASIDEWARPTYPE, 0, 0 },
	{"SCREENDATASIDEWARPOVFLAGS", SCREENDATASIDEWARPOVFLAGS, 0, 0 },
	{"SCREENDATAWARPARRIVALX", SCREENDATAWARPARRIVALX, 0, 0 },
	{"SCREENDATAWARPARRIVALY", SCREENDATAWARPARRIVALY, 0, 0 },
	{"SCREENDATAPATH", SCREENDATAPATH, 0, 0 },
	{"SCREENDATASIDEWARPSC", SCREENDATASIDEWARPSC, 0, 0 },
	{"SCREENDATASIDEWARPDMAP", SCREENDATASIDEWARPDMAP, 0, 0 },
	{"SCREENDATASIDEWARPINDEX", SCREENDATASIDEWARPINDEX, 0, 0 },
	{"SCREENDATAUNDERCOMBO", SCREENDATAUNDERCOMBO, 0, 0 },
	{"SCREENDATAUNDERCSET", SCREENDATAUNDERCSET, 0, 0 },
	{"SCREENDATACATCHALL", SCREENDATACATCHALL, 0, 0 },
	{"SCREENDATAFLAGS", SCREENDATAFLAGS, 0, 0 },
	{"SCREENDATACSENSITIVE", SCREENDATACSENSITIVE, 0, 0 },
	{"SCREENDATANORESET", SCREENDATANORESET, 0, 0 },
	{"SCREENDATANOCARRY", SCREENDATANOCARRY, 0, 0 },
	{"SCREENDATALAYERMAP", SCREENDATALAYERMAP, 0, 0 },
	{"SCREENDATALAYERSCREEN", SCREENDATALAYERSCREEN, 0, 0 },
	{"SCREENDATALAYEROPACITY", SCREENDATALAYEROPACITY, 0, 0 },
	{"SCREENDATATIMEDWARPTICS", SCREENDATATIMEDWARPTICS, 0, 0 },
	{"SCREENDATANEXTMAP", SCREENDATANEXTMAP, 0, 0 },
	{"SCREENDATANEXTSCREEN", SCREENDATANEXTSCREEN, 0, 0 },
	{"SCREENDATASECRETCOMBO", SCREENDATASECRETCOMBO, 0, 0 },
	{"SCREENDATASECRETCSET", SCREENDATASECRETCSET, 0, 0 },
	{"SCREENDATASECRETFLAG", SCREENDATASECRETFLAG, 0, 0 },
	{"SCREENDATAVIEWX", SCREENDATAVIEWX, 0, 0 },
	{"SCREENDATAVIEWY", SCREENDATAVIEWY, 0, 0 },
	{"SCREENDATASCREENWIDTH", SCREENDATASCREENWIDTH, 0, 0 },
	{"SCREENDATASCREENHEIGHT", SCREENDATASCREENHEIGHT, 0, 0 },
	{"SCREENDATAENTRYX", SCREENDATAENTRYX, 0, 0 },
	{"SCREENDATAENTRYY", SCREENDATAENTRYY, 0, 0 },
	{"SCREENDATANUMFF", SCREENDATANUMFF, 0, 0 },
	{"SCREENDATAFFDATA", SCREENDATAFFDATA, 0, 0 },
	{"SCREENDATAFFCSET", SCREENDATAFFCSET, 0, 0 },
	{"SCREENDATAFFDELAY", SCREENDATAFFDELAY, 0, 0 },
	{"SCREENDATAFFX", SCREENDATAFFX, 0, 0 },
	{"SCREENDATAFFY", SCREENDATAFFY, 0, 0 },
	{"SCREENDATAFFXDELTA", SCREENDATAFFXDELTA, 0, 0 },
	{"SCREENDATAFFYDELTA", SCREENDATAFFYDELTA, 0, 0 },
	{"SCREENDATAFFXDELTA2", SCREENDATAFFXDELTA2, 0, 0 },
	{"SCREENDATAFFYDELTA2", SCREENDATAFFYDELTA2, 0, 0 },
	{"SCREENDATAFFFLAGS", SCREENDATAFFFLAGS, 0, 0 },
	{"SCREENDATAFFWIDTH", SCREENDATAFFWIDTH, 0, 0 },
	{"SCREENDATAFFHEIGHT", SCREENDATAFFHEIGHT, 0, 0 },
	{"SCREENDATAFFLINK", SCREENDATAFFLINK, 0, 0 },
	{"SCREENDATAFFSCRIPT", SCREENDATAFFSCRIPT, 0, 0 },
	{"SCREENDATAINTID", SCREENDATAINTID, 0, 0 }, //Needs to be a function [32][10]
	{"SCREENDATAINITA", SCREENDATAINITA, 0, 0 }, //needs to be a function, [32][2]
	{"SCREENDATAFFINITIALISED", SCREENDATAFFINITIALISED, 0, 0 },
	{"SCREENDATASCRIPTENTRY", SCREENDATASCRIPTENTRY, 0, 0 },
	{"SCREENDATASCRIPTOCCUPANCY", SCREENDATASCRIPTOCCUPANCY, 0, 0 },
	{"SCREENDATASCRIPTEXIT", SCREENDATASCRIPTEXIT, 0, 0 },
	{"SCREENDATAOCEANSFX", SCREENDATAOCEANSFX, 0, 0 },
	{"SCREENDATABOSSSFX", SCREENDATABOSSSFX, 0, 0 },
	{"SCREENDATASECRETSFX", SCREENDATASECRETSFX, 0, 0 },
	{"SCREENDATAHOLDUPSFX", SCREENDATAHOLDUPSFX, 0, 0 },
	{"SCREENDATASCREENMIDI", SCREENDATASCREENMIDI, 0, 0 },
	{"SCREENDATALENSLAYER", SCREENDATALENSLAYER, 0, 0 },
	
	{"LINKSCRIPTTILE", LINKSCRIPTTILE, 0, 0 },
	{"LINKSCRIPFLIP", LINKSCRIPFLIP, 0, 0 },
	{"MAPDATAITEMX", MAPDATAITEMX, 0, 0 },
	{"MAPDATAITEMY", MAPDATAITEMY, 0, 0 },
	{"SCREENDATAITEMX", SCREENDATAITEMX, 0, 0 },
	{"SCREENDATAITEMY", SCREENDATAITEMY, 0, 0 },
	
	{"MAPDATAFFEFFECTWIDTH", MAPDATAFFEFFECTWIDTH, 0, 0 },
	{"MAPDATAFFEFFECTHEIGHT", MAPDATAFFEFFECTHEIGHT, 0, 0 },
	{"SCREENDATAFFEFFECTWIDTH", SCREENDATAFFEFFECTWIDTH, 0, 0 },
	{"SCREENDATAFFEFFECTHEIGHT", SCREENDATAFFEFFECTHEIGHT, 0, 0 },
	
	{"LOADMAPDATA", LOADMAPDATA, 0, 0 },
	{"LWPNPARENT", LWPNPARENT, 0, 0 },
	{"LWPNLEVEL", LWPNLEVEL, 0, 0 },
	{"EWPNPARENT", EWPNPARENT, 0, 0 },
	
	
	{"SHOPDATANAME", SHOPDATANAME, 0, 0 },
	{"SHOPDATAITEM", SHOPDATAITEM, 0, 0 },
	{"SHOPDATAHASITEM", SHOPDATAHASITEM, 0, 0 },
	{"SHOPDATAPRICE", SHOPDATAPRICE, 0, 0 },
	{"SHOPDATASTRING", SHOPDATASTRING, 0, 0 },
	
	{"NPCDATASHIELD", NPCDATASHIELD, 0, 0 },
	{"NPCSHIELD", NPCSHIELD, 0, 0 },
	{"AUDIOVOLUME", AUDIOVOLUME, 0, 0 },
	{"AUDIOPAN", AUDIOPAN, 0, 0 },
	
	{"MESSAGEDATANEXT", MESSAGEDATANEXT, 0, 0 },
	{"MESSAGEDATATILE", MESSAGEDATATILE, 0, 0 },
	{"MESSAGEDATACSET", MESSAGEDATACSET, 0, 0 },
	{"MESSAGEDATATRANS", MESSAGEDATATRANS, 0, 0 },
	{"MESSAGEDATAFONT", MESSAGEDATAFONT, 0, 0 },
	{"MESSAGEDATAX", MESSAGEDATAX, 0, 0 },
	{"MESSAGEDATAY", MESSAGEDATAY, 0, 0 },
	{"MESSAGEDATAW", MESSAGEDATAW, 0, 0 },
	{"MESSAGEDATAH", MESSAGEDATAH, 0, 0 },
	{"MESSAGEDATASFX", MESSAGEDATASFX, 0, 0 },
	{"MESSAGEDATALISTPOS", MESSAGEDATALISTPOS, 0, 0 },
	{"MESSAGEDATAVSPACE", MESSAGEDATAVSPACE, 0, 0 },
	{"MESSAGEDATAHSPACE", MESSAGEDATAHSPACE, 0, 0 },
	{"MESSAGEDATAFLAGS", MESSAGEDATAFLAGS, 0, 0 },
	
	{"DMAPDATAMAP", DMAPDATAMAP, 0, 0 },
	{"DMAPDATALEVEL", DMAPDATALEVEL, 0, 0 },
	{"DMAPDATAOFFSET", DMAPDATAOFFSET, 0, 0 },
	{"DMAPDATACOMPASS", DMAPDATACOMPASS, 0, 0 },
	{"DMAPDATAPALETTE", DMAPDATAPALETTE, 0, 0 },
	{"DMAPDATAMIDI", DMAPDATAMIDI, 0, 0 },
	{"DMAPDATACONTINUE", DMAPDATACONTINUE, 0, 0 },
	{"DMAPDATATYPE", DMAPDATATYPE, 0, 0 },
	{"DMAPDATAGRID", DMAPDATAGRID, 0, 0 },
	{"DMAPDATAMINIMAPTILE", DMAPDATAMINIMAPTILE, 0, 0 },
	{"DMAPDATAMINIMAPCSET", DMAPDATAMINIMAPCSET, 0, 0 },
	{"DMAPDATALARGEMAPTILE", DMAPDATALARGEMAPTILE, 0, 0 },
	{"DMAPDATALARGEMAPCSET", DMAPDATALARGEMAPCSET, 0, 0 },
	{"DMAPDATAMUISCTRACK", DMAPDATAMUISCTRACK, 0, 0 },
	{"DMAPDATASUBSCRA", DMAPDATASUBSCRA, 0, 0 },
	{"DMAPDATASUBSCRP", DMAPDATASUBSCRP, 0, 0 },
	{"DMAPDATADISABLEDITEMS", DMAPDATADISABLEDITEMS, 0, 0 },
	{"DMAPDATAFLAGS", DMAPDATAFLAGS, 0, 0 },
	
	{"NPCFROZEN", NPCFROZEN, 0, 0 },
	{"NPCFROZENTILE", NPCFROZENTILE, 0, 0 },
	{"NPCFROZENCSET", NPCFROZENCSET, 0, 0 },
	
	{"ITEMPSTRING", ITEMPSTRING, 0, 0 },
	{"ITEMPSTRINGFLAGS", ITEMPSTRINGFLAGS, 0, 0 },
	{"ITEMOVERRIDEFLAGS", ITEMOVERRIDEFLAGS, 0, 0 },
	{"LINKPUSH", LINKPUSH, 0, 0 },
	{"LINKSTUN", LINKSTUN, 0, 0 },
	{"IDATACOSTCOUNTER", IDATACOSTCOUNTER, 0, 0 },
	{"TYPINGMODE", TYPINGMODE, 0, 0 },
	//{"DMAPDATAGRAVITY", DMAPDATAGRAVITY, 0, 0 },
	//{"DMAPDATAJUMPLAYER", DMAPDATAJUMPLAYER, 0, 0 },
	//end ffscript vars
	//END VARS END OF BYTECODE
	
	//newcombo
	{"COMBODTILE", COMBODTILE, 0, 0 },
	{"COMBODFLIP", COMBODFLIP, 0, 0 },
	{"COMBODWALK", COMBODWALK, 0, 0 },
	{"COMBODTYPE", COMBODTYPE, 0, 0 },
	{"COMBODCSET", COMBODCSET, 0, 0 },
	{"COMBODFOO", COMBODFOO, 0, 0 },
	{"COMBODFRAMES", COMBODFRAMES, 0, 0 },
	{"COMBODNEXTD", COMBODNEXTD, 0, 0 },
	{"COMBODNEXTC", COMBODNEXTC, 0, 0 },
	{"COMBODFLAG", COMBODFLAG, 0, 0 },
	{"COMBODSKIPANIM", COMBODSKIPANIM, 0, 0 },
	{"COMBODNEXTTIMER", COMBODNEXTTIMER, 0, 0 },
	{"COMBODAKIMANIMY", COMBODAKIMANIMY, 0, 0 },
	{"COMBODANIMFLAGS", COMBODANIMFLAGS, 0, 0 },
	{"COMBODEXPANSION", COMBODEXPANSION, 0, 0 },
	{"COMBODATTRIBUTES", COMBODATTRIBUTES, 0, 0 },
	{"COMBODUSRFLAGS", COMBODUSRFLAGS, 0, 0 },
	{"COMBODTRIGGERFLAGS", COMBODTRIGGERFLAGS, 0, 0 },
	{"COMBODTRIGGERLEVEL", COMBODTRIGGERLEVEL, 0, 0 },

	//comboclass
	{"COMBODNAME", COMBODNAME, 0, 0 },
	{"COMBODBLOCKNPC", COMBODBLOCKNPC, 0, 0 },
	{"COMBODBLOCKHOLE", COMBODBLOCKHOLE, 0, 0 },
	{"COMBODBLOCKTRIG", COMBODBLOCKTRIG, 0, 0 },
	{"COMBODBLOCKWEAPON", COMBODBLOCKWEAPON, 0, 0 },
	{"COMBODCONVXSPEED", COMBODCONVXSPEED, 0, 0 },
	{"COMBODCONVYSPEED", COMBODCONVYSPEED, 0, 0 },
	{"COMBODSPAWNNPC", COMBODSPAWNNPC, 0, 0 },
	{"COMBODSPAWNNPCWHEN", COMBODSPAWNNPCWHEN, 0, 0 },
	{"COMBODSPAWNNPCCHANGE", COMBODSPAWNNPCCHANGE, 0, 0 },
	{"COMBODDIRCHANGETYPE", COMBODDIRCHANGETYPE, 0, 0 },
	{"COMBODDISTANCECHANGETILES", COMBODDISTANCECHANGETILES, 0, 0 },
	{"COMBODDIVEITEM", COMBODDIVEITEM, 0, 0 },
	{"COMBODDOCK", COMBODDOCK, 0, 0 },
	{"COMBODFAIRY", COMBODFAIRY, 0, 0 },
	{"COMBODFFATTRCHANGE", COMBODFFATTRCHANGE, 0, 0 },
	{"COMBODFOORDECOTILE", COMBODFOORDECOTILE, 0, 0 },
	{"COMBODFOORDECOTYPE", COMBODFOORDECOTYPE, 0, 0 },
	{"COMBODHOOKSHOTPOINT", COMBODHOOKSHOTPOINT, 0, 0 },
	{"COMBODLADDERPASS", COMBODLADDERPASS, 0, 0 },
	{"COMBODLOCKBLOCK", COMBODLOCKBLOCK, 0, 0 },
	{"COMBODLOCKBLOCKCHANGE", COMBODLOCKBLOCKCHANGE, 0, 0 },
	{"COMBODMAGICMIRROR", COMBODMAGICMIRROR, 0, 0 },
	{"COMBODMODHPAMOUNT", COMBODMODHPAMOUNT, 0, 0 },
	{"COMBODMODHPDELAY", COMBODMODHPDELAY, 0, 0 },
	{"COMBODMODHPTYPE", COMBODMODHPTYPE, 0, 0 },
	{"COMBODNMODMPAMOUNT", COMBODNMODMPAMOUNT, 0, 0 },
	{"COMBODMODMPDELAY", COMBODMODMPDELAY, 0, 0 },
	{"COMBODMODMPTYPE", COMBODMODMPTYPE, 0, 0 },
	{"COMBODNOPUSHBLOCK", COMBODNOPUSHBLOCK, 0, 0 },
	{"COMBODOVERHEAD", COMBODOVERHEAD, 0, 0 },
	{"COMBODPLACENPC", COMBODPLACENPC, 0, 0 },
	{"COMBODPUSHDIR", COMBODPUSHDIR, 0, 0 },
	{"COMBODPUSHWAIT", COMBODPUSHWAIT, 0, 0 },
	{"COMBODPUSHHEAVY", COMBODPUSHHEAVY, 0, 0 },
	{"COMBODPUSHED", COMBODPUSHED, 0, 0 },
	{"COMBODRAFT", COMBODRAFT, 0, 0 },
	{"COMBODRESETROOM", COMBODRESETROOM, 0, 0 },
	{"COMBODSAVEPOINTTYPE", COMBODSAVEPOINTTYPE, 0, 0 },
	{"COMBODSCREENFREEZETYPE", COMBODSCREENFREEZETYPE, 0, 0 },
	{"COMBODSECRETCOMBO", COMBODSECRETCOMBO, 0, 0 },
	{"COMBODSINGULAR", COMBODSINGULAR, 0, 0 },
	{"COMBODSLOWWALK", COMBODSLOWWALK, 0, 0 },
	{"COMBODSTATUETYPEs", COMBODSTATUETYPE, 0, 0 },
	{"COMBODSTEPTYPE", COMBODSTEPTYPE, 0, 0 },
	{"COMBODSTEPCHANGEINTO", COMBODSTEPCHANGEINTO, 0, 0 },
	{"COMBODSTRIKEWEAPONS", COMBODSTRIKEWEAPONS, 0, 0 },
	{"COMBODSTRIKEREMNANTS", COMBODSTRIKEREMNANTS, 0, 0 },
	{"COMBODSTRIKEREMNANTSTYPE", COMBODSTRIKEREMNANTSTYPE, 0, 0 },
	{"COMBODSTRIKECHANGE", COMBODSTRIKECHANGE, 0, 0 },
	{"COMBODSTRIKEITEM", COMBODSTRIKEITEM, 0, 0 },
	{"COMBODTOUCHITEM", COMBODTOUCHITEM, 0, 0 },
	{"COMBODTOUCHSTAIRS", COMBODTOUCHSTAIRS, 0, 0 },
	{"COMBODTRIGGERTYPE", COMBODTRIGGERTYPE, 0, 0 },
	{"COMBODTRIGGERSENS", COMBODTRIGGERSENS, 0, 0 },
	{"COMBODWARPTYPE", COMBODWARPTYPE, 0, 0 },
	{"COMBODWARPSENS", COMBODWARPSENS, 0, 0 },
	{"COMBODWARPDIRECT", COMBODWARPDIRECT, 0, 0 },
	{"COMBODWARPLOCATION", COMBODWARPLOCATION, 0, 0 },
	{"COMBODWATER", COMBODWATER, 0, 0 },
	{"COMBODWHISTLE", COMBODWHISTLE, 0, 0 },
	{"COMBODWINGAME", COMBODWINGAME, 0, 0 },
	{"COMBODBLOCKWPNLEVEL", COMBODBLOCKWPNLEVEL, 0, 0 },

	{"TYPINGMODE", TYPINGMODE, 0, 0 },
	{"TYPINGMODE", TYPINGMODE, 0, 0 },
	{"LINKHITBY", LINKHITBY, 0, 0 },
	{"LINKDEFENCE", LINKDEFENCE, 0, 0 },
	{"NPCHITBY", NPCHITBY, 0, 0 },
	{"NPCISCORE", NPCISCORE, 0, 0 },
	{"NPCSCRIPTUID", NPCSCRIPTUID, 0, 0 },
	{"LWEAPONSCRIPTUID", LWEAPONSCRIPTUID, 0, 0 },
	{"EWEAPONSCRIPTUID", EWEAPONSCRIPTUID, 0, 0 },
	{"ITEMSCRIPTUID", ITEMSCRIPTUID, 0, 0 },
	
	{"DMAPDATASIDEVIEW", DMAPDATASIDEVIEW, 0, 0 },
	
	{"DONULL", DONULL, 0, 0 },
	{"DEBUGD", DEBUGD, 0, 0 },
	{"GETPIXEL", GETPIXEL, 0, 0 },
	{"DOUNTYPE", DOUNTYPE, 0, 0 },
	
	{ " ",                       -1,             0,             0 }
};

long ffparse(char *string)
{
    //return int(atof(string)*10000);
    
    //this function below isn't working too well yet
    //clean_numeric_string(string);
    double negcheck = atof(string);
    
    //if no decimal point, ascii to int conversion
    char *ptr=strchr(string, '.');
    
    if(!ptr)
    {
        return atoi(string)*10000;
    }
    
    long ret=0;
    char *tempstring1;
    tempstring1=(char *)zc_malloc(strlen(string)+5);
    sprintf(tempstring1, string);
    
    for(int i=0; i<4; ++i)
    {
        tempstring1[strlen(string)+i]='0';
    }
    
    ptr=strchr(tempstring1, '.');
    *ptr=0;
    ret=atoi(tempstring1)*10000;
    
    ++ptr;
    char *ptr2=ptr;
    ptr2+=4;
    *ptr2=0;
    
    if(negcheck<0)
        ret-=atoi(ptr);
    else ret+=atoi(ptr);
    
    zc_free(tempstring1);
    return ret;
}

bool ffcheck(char *arg)
{

    for(int i=0; i<0x100; i++)
    {
        if(arg[i]!='\0')
        {
            if(i==0)
            {
                if(arg[i]!='-' && arg[i]!='.' && !(arg[i] >= '0' && arg[i] <='9'))
                    return false;
            }
            else
            {
                if(arg[i]!='.' && !(arg[i] >= '0' && arg[i] <='9'))
                    return false;
            }
        }
        else
        {
            i=0x100;
        }
    }
    
    return true;
}

char labels[65536][80];
int lines[65536];
int numlines;

int parse_script(ffscript **script)
{
    if(!getname("Import Script (.txt)","txt",NULL,datapath,false))
        return D_CLOSE;
        
    return parse_script_file(script,temppath, true);
}

int parse_script_file(ffscript **script, const char *path, bool report_success)
{
    saved=false;
    FILE *fscript = fopen(path,"rb");
    char *buffer = new char[0x400];
    char *combuf = new char[0x100];
    char *arg1buf = new char[0x100];
    char *arg2buf = new char[0x100];
    bool stop=false;
    bool success=true;
    numlines = 0;
    int num_commands;
    
    for(int i=0;; i++)
    {
        buffer[0]=0;
        
        if(stop)
        {
            num_commands=i+1;
            break;
        }
        
        for(int j=0; j<0x400; j++)
        {
            char temp;
            temp = getc(fscript);
            
            if(feof(fscript))
            {
                stop=true;
                buffer[j]='\0';
                j=0x400;
                ungetc(temp,fscript);
            }
            else
            {
                ungetc(temp,fscript);
                buffer[j] = getc(fscript);
                
                if(buffer[j] == ';' || buffer[j] == '\n' || buffer[j] == 13)
                {
                    if(buffer[j] == '\n')
                    {
                        buffer[j] = '\0';
                        j=0x400;
                    }
                    else
                    {
                        while(getc(fscript)!='\n')
                        {
                            if(feof(fscript))
                            {
                                stop=true;
                                break;
                            }
                        }
                        
                        buffer[j] = '\0';
                        j=0x400;
                    }
                }
            }
        }
        
        int k=0;
        
        while(buffer[k] == ' ' || buffer[k] == '\t') k++;
        
        if(buffer[k] == '\0')
        {
            i--;
            continue;
        }
        
        k=0;
        
        if(buffer[k] != ' ' && buffer[k] !='\t' && buffer[k] != '\0')
        {
            while(buffer[k] != ' ' && buffer[k] !='\t' && buffer[k] != '\0')
            {
                labels[numlines][k] = buffer[k];
                k++;
            }
            
            labels[numlines][k] = '\0';
            lines[numlines] = i;
            numlines++;
        }
    }
    
    fseek(fscript, 0, SEEK_SET);
    stop = false;
    
    if((*script)!=NULL) delete [](*script);
    
    (*script) = new ffscript[num_commands];
    
    for(int i=0; i<num_commands; i++)
    {
        if(stop)
        {
            (*script)[i].command = zasm::cmd_terminator;
            break;
        }
        else
        {
            /*
                  sprintf(buffer, "");
                  sprintf(combuf, "");
                  sprintf(arg1buf, "");
                  sprintf(arg2buf, "");
            */
            buffer[0]=0;
            combuf[0]=0;
            arg1buf[0]=0;
            arg2buf[0]=0;
            
            for(int j=0; j<0x400; j++)
            {
                char temp;
                temp = getc(fscript);
                
                if(feof(fscript))
                {
                    stop=true;
                    buffer[j]='\0';
                    j=0x400;
                    ungetc(temp,fscript);
                }
                else
                {
                    ungetc(temp,fscript);
                    buffer[j] = getc(fscript);
                    
                    if(buffer[j] == ';' || buffer[j] == '\n' || buffer[j] == 13)
                    {
                        if(buffer[j] == '\n')
                        {
                            buffer[j] = '\0';
                            j=0x400;
                        }
                        else
                        {
                            while(getc(fscript)!='\n')
                            {
                                if(feof(fscript))
                                {
                                    stop=true;
                                    break;
                                }
                            }
                            
                            buffer[j] = '\0';
                            j=0x400;
                        }
                    }
                }
            }
            
            int k=0, l=0;
            
            while(buffer[k] == ' ' || buffer[k] == '\t') k++;
            
            if(buffer[k] == '\0')
            {
                i--;
                continue;
            }
            
            k=0;
            
            while(buffer[k] != ' ' && buffer[k] != '\t' && buffer[k] != '\0') k++;
            
            while((buffer[k] == ' ' || buffer[k] == '\t') && buffer[k] != '\0')  k++;
            
            while(buffer[k] != ' ' && buffer[k] != '\t' && buffer[k] != '\0')
            {
                combuf[l] = buffer[k];
                k++;
                l++;
            }
            
            combuf[l] = '\0';
            l=0;
            
            while((buffer[k] == ' ' || buffer[k] == '\t') && buffer[k] != '\0') k++;
            
            while(buffer[k] != ',' && buffer[k] != ' ' && buffer[k] != '\t' && buffer[k] != '\0')
            {
                arg1buf[l] = buffer[k];
                k++;
                l++;
            }
            
            arg1buf[l] = '\0';
            l=0;
            
            while((buffer[k] == ' ' || buffer[k] == '\t' || buffer[k] == ',') && buffer[k] != '\0') k++;
            
            while(buffer[k] != ' ' && buffer[k] != '\t' && buffer[k] != '\0')
            {
                arg2buf[l] = buffer[k];
                k++;
                l++;
            }
            
            arg2buf[l] = '\0';
            
            if (string const* error = parse_script_section(
	                combuf, arg1buf, arg2buf, (*script)[i]))
            {
                char buf[80],buf2[80],buf3[80],name[13];
                extract_name(temppath,name,FILENAME8_3);
                sprintf(buf,"Unable to parse instruction %d from script %s",i+1,name);
                sprintf(buf2,"The error was: %s", error->c_str());
                sprintf(buf3,"The command was (%s) (%s,%s)",combuf,arg1buf,arg2buf);
                jwin_alert("Error",buf,buf2,buf3,"O&K",NULL,'k',0,lfont);
                stop=true;
                success=false;
                (*script)[0].command = zasm::cmd_terminator;
            }
        }
    }
    
    if(report_success && success) //(!stop) // stop is never true here
    {
        char buf[80],name[13];
        extract_name(temppath,name,FILENAME8_3);
        sprintf(buf,"Script %s has been parsed",name);
        jwin_alert("Success",buf,NULL,NULL,"O&K",NULL,'k',0,lfont);
    }
    
    delete [] buffer;
    delete [] combuf;
    delete [] arg1buf;
    delete [] arg2buf;
    fclose(fscript);
    return success?D_O_K:D_CLOSE;
}

int set_argument(char *argbuf, long& target)
{
    int i=0;
    char tempvar[20];
    
    while(variable_list[i].id>-1)
    {
        if(variable_list[i].maxcount>1)
        {
            for(int j=0; j<variable_list[i].maxcount; ++j)
            {
                if(strcmp(variable_list[i].name,"A")==0)
                    sprintf(tempvar, "%s%d", variable_list[i].name, j+1);
                else sprintf(tempvar, "%s%d", variable_list[i].name, j);
                
                if(stricmp(argbuf,tempvar)==0)
                {
                    long temp = variable_list[i].id+(j*zc_max(1,variable_list[i].multiple));
                    target = temp;
                    return 1;
                }
            }
        }
        else
        {
            if(stricmp(argbuf,variable_list[i].name)==0)
            {
                target = variable_list[i].id;
                return 1;
            }
        }
        
        ++i;
    }
    
    return 0;
}

bool parse_command_argument(
	zasm::arg_type type,
	char* arg_buf,
	long& target)
{
	switch (type)
	{
	case zasm::arg_none:
		return true;
	case zasm::arg_register:
		return set_argument(arg_buf, target);
	case zasm::arg_immediate:
		if (!ffcheck(arg_buf)) return false;
		target = ffparse(arg_buf);
		return true;
		
	case zasm::arg_label:
		for (int line = 0; line < numlines; ++line)
		{
			if (stricmp(arg_buf, labels[line]) == 0)
			{
				target = lines[line];
				return true;
			}
		}
		target = atoi(arg_buf) - 1;
		return true;

	default:
		return false;
	}
}

string const err_instruction("invalid instruction!");
string const err_param1("parameter 1 invalid!");
string const err_param2("parameter 2 invalid!");

// Return error message.
string const* parse_script_section(
	char* command_buf,
	char* arg1_buf,
	char* arg2_buf,
	ffscript& instruction)
{
    instruction.arg1 = 0;
    instruction.arg2 = 0;

    instruction.command = zasm::get_command(command_buf);
    if (!is_valid(instruction.command))
	    return &err_instruction;

    int arg_count = get_arg_count(instruction.command);

    if (arg_count >= 1)
	    if (!parse_command_argument(
		        get_first_argtype(instruction.command),
		        arg1_buf, instruction.arg1))
		    return &err_param1;

    if (arg_count >= 2)
	    if (!parse_command_argument(
		        get_second_argtype(instruction.command),
		        arg2_buf, instruction.arg2))
		    return &err_param2;

    return NULL;
}

