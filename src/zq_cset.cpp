//--------------------------------------------------------
//  Zelda Classic
//  by Jeremy Craner, 1999-2000
//
//  zquest.cc
//
//  Main code for the quest editor.
//
//--------------------------------------------------------

#ifndef __GTHREAD_HIDE_WIN32API
#define __GTHREAD_HIDE_WIN32API 1
#endif                            //prevent indirectly including windows.h

#include "precompiled.h" //always first

//#include <mem.h>
#include <string.h>
#include <stdio.h>

#include "gui.h"
#include "zc_alleg.h"
#include "zdefs.h"
#include "colors.h"
#include "pal.h"
#include "zquest.h"
#include "jwin.h"
#include "zsys.h"
#include "zq_tiles.h"
#include "zq_misc.h"
#include "zq_cset.h"
#include "zq_class.h"

extern int d_dummy_proc(int msg,DIALOG *d,int c);
extern int d_dropdmaplist_proc(int msg,DIALOG *d,int c);
extern int onHelp();
extern FONT *lfont;
extern int jwin_pal[jcMAX];
extern const char *dmaplist(int index, int *list_size);
extern bool saved;
extern int dmap_list_size;
extern bool dmap_list_zero;
extern zinitdata zinit;
extern int startdmapxy[6];
extern PALETTE RAMpal;


#include "gfxpal.h"

int color_index=0, color_copy=-1;

void get_cset(int dataset,int row,RGB *pal)
{
    saved=false;
    
    for(int i=0; i<16; i++)
    {
        colordata[(CSET(dataset)+i)*3]   = pal[(row<<4)+i].r;
        colordata[(CSET(dataset)+i)*3+1] = pal[(row<<4)+i].g;
        colordata[(CSET(dataset)+i)*3+2] = pal[(row<<4)+i].b;
    }
}

int color = 0;
int gray  = 0;
int ratio = 32;

int jwin_hsl_proc(int msg, DIALOG *d, int c);
int jwin_cset_proc(int msg, DIALOG* d, int c);
int edit_cset_kb_handler(int msg, DIALOG* d, int c);
void onInsertColor();
void onInsertColor_Text();
void jumpText(int r, int g, int b);
void onJumpText();
void onJumpHSL();

static DIALOG edit_cset_dlg[] =
{
	/* (dialog proc)       (x)    (y)   (w)   (h)     (fg)      (bg)     (key)      (flags)     (d1)           (d2)     (dp) */
	{ jwin_win_proc,         0,   0,    320,  240,    vc(14),   vc(1),      0,      D_EXIT,     0,             0, (void *) "Edit CSet", NULL, NULL },
	{ d_timer_proc,          0,    0,     0,    0,    0,        0,          0,      0,          0,             0,       NULL, NULL, NULL },
	{ d_dummy_proc,          0,    0,     0,    0,    0,        0,          0,      0,          0,             0, NULL, NULL, NULL },
	{ jwin_button_proc,    240,  184,    61,   21,    vc(14),   vc(1),     27,      D_EXIT,     0,             0, (void *) "Cancel", NULL, NULL },
	{ jwin_button_proc,    240,  152,    61,   21,    vc(14),   vc(1),     13,      D_EXIT,     0,             0, (void *) "OK", NULL, NULL },
	{ d_keyboard_proc,       0,    0,     0,    0,         0,       0,      0,      0,          KEY_F1,        0, (void *) onHelp, NULL, NULL },
	{ edit_cset_kb_handler,  0,    0,     0,    0,         0,       0,      0,      0,          0,             0, NULL, NULL, NULL },
	// 7
	{ d_dummy_proc,         160, 140,    61,   21,    vc(14),   vc(1),      0,      0,          0,             0, (void *) "Jump", NULL, (void*) onJumpHSL },
	{ jwin_hsl_proc,        72,   28,   174,   88,    vc(14),   vc(1),      0,      0,          0,             0, NULL, NULL, NULL },
	{ jwin_cset_proc,       96,  170,   128,   24,    vc(14),   vc(1),      0,      0,          0,             0, NULL, NULL, NULL },
    // 10
	{ jwin_edit_proc,       24,  120,   24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          3,             0,       NULL, NULL, NULL },
    { jwin_edit_proc,       24,  140,   24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          3,             0,       NULL, NULL, NULL },
    { jwin_edit_proc,       24,  160,   24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          3,             0,       NULL, NULL, NULL },
	{ jwin_func_button_proc,16,  180,   40,   21,    vc(14),   vc(1),      0,      0,          0,             0, (void *) "Insert", NULL, (void*) onInsertColor_Text },
	{ jwin_func_button_proc,16,  205,   40,   21,    vc(14),   vc(1),      0,      0,          0,             0, (void *) "Jump", NULL, (void*) onJumpText },
	// 15
	{ jwin_func_button_proc,130, 140,    61,   21,    vc(14),   vc(1),      0,      0,          0,             0, (void *) "&Insert", NULL, (void*) onInsertColor },
    { jwin_text_proc,        16,  124,   8,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "R", NULL, NULL },
    { jwin_text_proc,        16,  144,   8,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "G", NULL, NULL },
    { jwin_text_proc,        16,  164,   8,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "B", NULL, NULL },
    { jwin_check_proc,      230,   30,   8,   9,    vc(0),              vc(11),           0,       0,          0,             0, (void *) "View 8b values", NULL, NULL },
	// 20
	
	{ NULL,                 0,    0,    0,    0,   0,       0,       0,       0,          0,             0,       NULL,                           NULL,  NULL }
};

int jwin_hsl_proc(int msg, DIALOG *d, int c)
{
#define HSL_FLAG_HASMOUSE	1
#define HSL_FLAG_COLOR		2
#define HSL_FLAG_SATURATION	4
#define HSL_FLAGS_CS		(HSL_FLAG_COLOR|HSL_FLAG_SATURATION)
	ASSERT(d);
	//d->w and d->h are ignored
	d->w = (is_large?1.5:1)*174;
	d->h = (is_large?1.5:1)*88;
	int ret = D_O_K;
	static int hue_x_offs = 24, hue_y_offs = 2, light_x_offs = 2, light_y_offs = 24,
		sat_x_offs = 158, sat_y_offs = 24, c_x_offs = 24, c_y_offs = 24, c_wid = 128, c_hei = 64, misc_wh = 16;
	if(d->flags & D_DIRTY)
	{
		ret |= D_REDRAWME;
		d->flags &= ~D_DIRTY;
	}
	if(is_large && c_hei == 64)
	{
		hue_x_offs *= 1.5;
		hue_y_offs *= 1.5;
		light_x_offs *= 1.5;
		light_y_offs *= 1.5;
		sat_x_offs *= 1.5;
		sat_y_offs *= 1.5;
		c_x_offs *= 1.5;
		c_y_offs *= 1.5;
		c_wid *= 1.5;
		c_hei *= 1.5;
		misc_wh *= 1.5;
	}
	int clr = color, gr = gray, rat = ratio;
	int x=gui_mouse_x();
	int y=gui_mouse_y();
	switch(msg)
	{
		case MSG_START:
			d->d1 = 0;
			break;
		case MSG_GOTMOUSE:
			d->d1 |= HSL_FLAG_HASMOUSE;
			break;
		case MSG_LOSTMOUSE:
			d->d1 &= ~HSL_FLAG_HASMOUSE;
			break;
		case MSG_CLICK:
		case MSG_DCLICK:
		case MSG_LPRESS:
			if((d->d1 & HSL_FLAG_HASMOUSE)&&!(d->d1 & HSL_FLAGS_CS))
			{
				if(isinRect(x,y,(d->x+c_x_offs),(d->y+c_y_offs),d->x+c_x_offs+c_wid-1,(d->y+c_y_offs)+c_hei-1))
				{
					d->d1 |= HSL_FLAG_COLOR;
				}
				else if(isinRect(x,y,(d->x+sat_x_offs),(d->y+sat_y_offs),(d->x+sat_x_offs)+misc_wh-1,(d->y+sat_y_offs)+c_hei-1))
				{
					d->d1 |= HSL_FLAG_SATURATION;
				}
			}
			break;
		case MSG_LRELEASE:
			d->d1 &= ~HSL_FLAGS_CS;
			break;
		case MSG_IDLE:
			if(gui_mouse_b()==1)
			{
				if(d->d1 & HSL_FLAG_COLOR)
				{
					color = vbound(x-(d->x+c_x_offs),0,c_wid-1);
					gray = vbound(y-(d->y+c_y_offs),0,c_hei-1);
				}
				if(d->d1 & HSL_FLAG_SATURATION)
				{
					ratio = vbound(y-(d->y+sat_y_offs),0,c_hei-1);
				}
			}
			else
			{
				d->d1 &= ~(HSL_FLAG_COLOR|HSL_FLAG_SATURATION);
			}
			//Fallthrough
		case MSG_DRAW:
			if(is_large)
			{
				clr /= 1.5;
				gr /= 1.5;
				rat /= 1.5;
			}
			custom_vsync();
			scare_mouse();
			//Hue
			jwin_draw_frame(screen, d->x+hue_x_offs-2, d->y+hue_y_offs-2, int(128*(is_large?1.5:1)+4), misc_wh+4, FR_DEEP);
			for(int i=0; i<128; i++)
			{
				rectfill(screen,int(floor(i*(is_large?1.5:1))+d->x+hue_x_offs),d->y+hue_y_offs,int(ceil(i*(is_large?1.5:1))+d->x+hue_x_offs),d->y+hue_y_offs+misc_wh-1,i);
			}
			//Light
			jwin_draw_frame(screen, d->x+light_x_offs-2, d->y+light_y_offs-2, misc_wh+4, int(64*(is_large?1.5:1)+4), FR_DEEP);
			for(int i=0; i<32; i++)
			{
				rectfill(screen,d->x+light_x_offs,((int)floor(i*(is_large?1.5:1))<<1)+d->y+light_y_offs,
						 d->x+light_x_offs+misc_wh-1,((int)ceil(i*(is_large?1.5:1))<<1)+d->y+light_y_offs+1,i+128);
			}
			//Saturation
			jwin_draw_frame(screen, d->x + (sat_x_offs-2), d->y + (sat_y_offs-2), misc_wh+4, int(64*(is_large?1.5:1)+4), FR_DEEP);
			for(int i=0; i<32; i++)
			{
				RAMpal[i+160] = mixRGB(gfx_pal[clr*3],gfx_pal[clr*3+1],
									   gfx_pal[clr*3+2],gr,gr,gr,i<<1); //saturatn
				rectfill(screen,d->x + sat_x_offs,((int)floor(i*(is_large?1.5:1))<<1)+d->y+sat_y_offs,
						 d->x+sat_x_offs+misc_wh-1,((int)ceil(i*(is_large?1.5:1))<<1)+d->y+sat_y_offs+1,i+160);
			}
			RAMpal[edc] = mixRGB(gfx_pal[clr*3],gfx_pal[clr*3+1],gfx_pal[clr*3+2],gr,gr,gr,rat);
			RAMpal[edi] = invRGB(RAMpal[edc]);
			set_palette_range(RAMpal,0,255,false);
			//Color
			jwin_draw_frame(screen, d->x+c_x_offs-2, d->y+c_y_offs-2, c_wid+4, c_hei+4, FR_DEEP);
			rectfill(screen,d->x+c_x_offs,d->y+c_y_offs,d->x+c_x_offs+c_wid-1,d->y+c_y_offs+c_hei-1,edc);
			//
			_allegro_hline(screen,d->x+c_x_offs,gray+d->y+c_y_offs,d->x+c_x_offs+c_wid-1,edi);
			_allegro_vline(screen,color+d->x+c_x_offs,d->y+c_y_offs,d->y+c_y_offs+c_hei-1,edi);
			_allegro_hline(screen,d->x+sat_x_offs,ratio+d->y+sat_y_offs,d->x+sat_x_offs+misc_wh-1,edi);
			if((edit_cset_dlg[19].flags & D_SELECTED))
				textprintf_centre_ex(screen,font,d->x+(d->w/2),int(d->y+c_y_offs+c_hei+10*(is_large?1.5:1)),jwin_pal[jcBOXFG],jwin_pal[jcBOX],"  RGB - %3d %3d %3d  ",RAMpal[edc].r*4,RAMpal[edc].g*4,RAMpal[edc].b*4);
			else textprintf_centre_ex(screen,font,d->x+(d->w/2),int(d->y+c_y_offs+c_hei+10*(is_large?1.5:1)),jwin_pal[jcBOXFG],jwin_pal[jcBOX],"  RGB - %2d %2d %2d  ",RAMpal[edc].r,RAMpal[edc].g,RAMpal[edc].b);
			unscare_mouse();
			SCRFIX();
			break;
	}
	return ret;
}

int jwin_cset_proc(int msg, DIALOG* d, int c)
{
	static int cs_hei = 8;
	static int last_index = 0, last_copy = 0;
	static int lastshow16 = 0;
	ASSERT(d);
	int ret = D_O_K;
	if(d->flags & D_DIRTY)
	{
		ret |= D_REDRAWME;
		d->flags &= ~D_DIRTY;
	}
	if(lastshow16 != (edit_cset_dlg[19].flags & D_SELECTED))
	{
		lastshow16 = (edit_cset_dlg[19].flags & D_SELECTED);
		char* c1 = (char*)edit_cset_dlg[10].dp;
		char* c2 = (char*)edit_cset_dlg[11].dp;
		char* c3 = (char*)edit_cset_dlg[12].dp;
		int r = atoi(c1), g = atoi(c2), b = atoi(c3);
		
		if(!lastshow16) //Just turned off
		{
			r /= 4;
			g /= 4;
			b /= 4;
		}
		jumpText(r,g,b);
		ret |= D_REDRAW | D_REDRAWME;
	}
	if(is_large && cs_hei == 8)
	{
		cs_hei *= 1.5;
		d->h = cs_hei * 3;
	}
	int x = gui_mouse_x();
	int y = gui_mouse_y();
	if(gui_mouse_b()!=1) d->d1 = -1;
	switch(msg)
	{
		case MSG_START:
			color_index = 0;
			color_copy = -1;
			d->d1 = -1;
			break;
		case MSG_LPRESS:
		{
			int new_index=vbound((int)((x-d->x)/(is_large?1.5:1))>>3,0,15);
			if(isinRect(x,y,d->x,d->y,d->x+d->w-1,d->y + d->h - 1))
				d->d1 = new_index;
			break;
		}
		case MSG_LRELEASE:
			d->d1 = -1;
			break;
		case MSG_WANTMOUSE:
			if(d->d1 > -1)
				ret |= D_WANTFOCUS;
			break;
		case MSG_IDLE:
			if(gui_mouse_b()==1)
			{
				int new_index=vbound((int)((x-d->x)/(is_large?1.5:1))>>3,0,15);
                
                if(color_index!=new_index && (d->d1 > -1 || isinRect(x,y,d->x,d->y,d->x+d->w-1,d->y + d->h - 1)))
                {
					if(d->d1 > -1)
					{
						if(color_index<new_index)
						{
							for(int i=color_index; i<new_index; ++i)
								zc_swap(RAMpal[14*16+i], RAMpal[14*16+i+1]);
						}
						else
						{
							for(int i=color_index; i>new_index; --i)
								zc_swap(RAMpal[14*16+i], RAMpal[14*16+i-1]);
						}
					}
					color_index = new_index;
                }
			}
			else d->d1 = -1;
			if(color_index != last_index || color_copy != last_copy)
			{
				last_index = color_index;
				last_copy = color_copy;
				ret |= D_REDRAWME;
			}
			break;
		case MSG_DRAW:
			rectfill(screen,d->x-2,d->y-2,d->x+d->w+4,d->y+d->h+20,jwin_pal[jcBOX]);
			// Old colors
			jwin_draw_frame(screen, d->x-2, d->y-2, d->w+4, cs_hei+4, FR_DEEP);
			for(int i=0; i<16; ++i)
			{
				rectfill(screen,int((i<<3)*(is_large?1.5:1)+d->x),d->y,int((i<<3)*(is_large?1.5:1)+d->x+cs_hei-1),d->y+cs_hei-1,12*16+i);
			}
			// New colors
			jwin_draw_frame(screen, d->x-2, d->y-2+(cs_hei*2), d->w+4, cs_hei+4, FR_DEEP);
			for(int i=0; i<16; ++i)
			{
				rectfill(screen,int((i<<3)*(is_large?1.5:1)+d->x),d->y+(cs_hei*2),int((i<<3)*(is_large?1.5:1)+d->x+cs_hei-1),d->y+cs_hei-1+(cs_hei*2),14*16+i);
			}
			//Text
			rectfill(screen,d->x,d->y + d->h + 3,d->x + d->w - 1,int(d->y + d->h + (32*(is_large?1.5:1))),jwin_pal[jcBOX]);
			
			if(color_copy>=0)
			{
				textout_ex(screen,(is_large?lfont_l:font),"\x81",int((color_copy<<3)*(is_large?1.5:1)+d->x),d->y + d->h + 3,jwin_pal[jcBOXFG],jwin_pal[jcBOX]);
			}
			
			textout_ex(screen,(is_large?lfont_l:font),"\x88",int((color_index<<3)*(is_large?1.5:1)+d->x),d->y + d->h + 3,jwin_pal[jcBOXFG],jwin_pal[jcBOX]);
			
			if((edit_cset_dlg[19].flags & D_SELECTED))
			{
				textprintf_centre_ex(screen,(is_large?lfont_l:font),d->x + d->w/2,d->y + d->h + int(12*(is_large?1.5:1)),jwin_pal[jcBOXFG],jwin_pal[jcBOX],"Old: %2d - %3d %3d %3d",color_index, RAMpal[12*16+color_index].r*4,RAMpal[12*16+color_index].g*4,RAMpal[12*16+color_index].b*4);
				textprintf_centre_ex(screen,(is_large?lfont_l:font),d->x + d->w/2,d->y + d->h + int(22*(is_large?1.5:1)),jwin_pal[jcBOXFG],jwin_pal[jcBOX],"New: %2d - %3d %3d %3d",color_index, RAMpal[14*16+color_index].r*4,RAMpal[14*16+color_index].g*4,RAMpal[14*16+color_index].b*4);
			}
			else
			{
				textprintf_centre_ex(screen,(is_large?lfont_l:font),d->x + d->w/2,d->y + d->h + int(12*(is_large?1.5:1)),jwin_pal[jcBOXFG],jwin_pal[jcBOX],"Old: %2d - %2d %2d %2d",color_index, RAMpal[12*16+color_index].r,RAMpal[12*16+color_index].g,RAMpal[12*16+color_index].b);
				textprintf_centre_ex(screen,(is_large?lfont_l:font),d->x + d->w/2,d->y + d->h + int(22*(is_large?1.5:1)),jwin_pal[jcBOXFG],jwin_pal[jcBOX],"New: %2d - %2d %2d %2d",color_index, RAMpal[14*16+color_index].r,RAMpal[14*16+color_index].g,RAMpal[14*16+color_index].b);
			}
			break;
	}
	return ret;
}

int edit_cset_kb_handler(int msg, DIALOG* d, int c)
{
	ASSERT(d);
	int ret = D_O_K;
	switch(msg)
	{
		case MSG_START:
			d->h = d->w = 0;
			break;
		case MSG_XCHAR:
		case MSG_CHAR:
			switch(c>>8)
			{
				case KEY_LEFT:
				case KEY_4:
				case KEY_4_PAD:
					if(key[KEY_ZC_LCONTROL] || key[KEY_ZC_RCONTROL])
					{
						zc_swap(RAMpal[14*16+color_index], RAMpal[14*16+((color_index-1)&15)]);
					}
					if(color_index==0)
						color_index = 15;
					else --color_index;
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
				case KEY_RIGHT:
				case KEY_6:
				case KEY_6_PAD:
					if(key[KEY_ZC_LCONTROL] || key[KEY_ZC_RCONTROL])
					{
						zc_swap(RAMpal[14*16+color_index], RAMpal[14*16+((color_index+1)&15)]);
					}
					if(color_index==15)
						color_index = 0;
					else ++color_index;
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
				case KEY_HOME:
				case KEY_7:
				case KEY_7_PAD:
					++RAMpal[14*16+color_index].r;
					RAMpal[14*16+color_index].r&=0x3F;
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
					
				case KEY_END:
				case KEY_1:
				case KEY_1_PAD:
					--RAMpal[14*16+color_index].r;
					RAMpal[14*16+color_index].r&=0x3F;
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
					
				case KEY_UP:
				case KEY_8:
				case KEY_8_PAD:
					++RAMpal[14*16+color_index].g;
					RAMpal[14*16+color_index].g&=0x3F;
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
					
				case KEY_DOWN:
				case KEY_2:
				case KEY_2_PAD:
					--RAMpal[14*16+color_index].g;
					RAMpal[14*16+color_index].g&=0x3F;
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
					
				case KEY_PGUP:
				case KEY_9:
				case KEY_9_PAD:
					++RAMpal[14*16+color_index].b;
					RAMpal[14*16+color_index].b&=0x3F;
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
					
				case KEY_PGDN:
				case KEY_3:
				case KEY_3_PAD:
					--RAMpal[14*16+color_index].b;
					RAMpal[14*16+color_index].b&=0x3F;
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
					
				case KEY_C:
					color_copy=color_index;
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
					
				case KEY_V:
					if(color_copy>=0)
					{
						RAMpal[14*16+color_index]=RAMpal[14*16+color_copy];
						color_copy=-1;
						edit_cset_dlg[9].flags |= D_DIRTY;
					}
					break;
					
				case KEY_M:
					if(color_copy>=0)
					{
						RGB temp=RAMpal[14*16+color_index];
						RAMpal[14*16+color_index]=RAMpal[14*16+color_copy];
						RAMpal[14*16+color_copy]=temp;
						color_copy=-1;
						edit_cset_dlg[9].flags |= D_DIRTY;
					}
					break;
					
				case KEY_I:
				case KEY_INSERT:
					RAMpal[14*16+color_index]=RAMpal[edc];
					edit_cset_dlg[9].flags |= D_DIRTY;
					break;
					
				case KEY_S:
					onSnapshot();
					break;
				default:
				{
					switch(c&255)
					{
						case '4':
						{
							if(key[KEY_ZC_LCONTROL] || key[KEY_ZC_RCONTROL])
							{
								zc_swap(RAMpal[14*16+color_index], RAMpal[14*16+((color_index-1)&15)]);
							}
							
							color_index=(color_index-1)&15;
							break;
						}
						
						case '6':
						{
							if(key[KEY_ZC_LCONTROL] || key[KEY_ZC_RCONTROL])
							{
								zc_swap(RAMpal[14*16+color_index], RAMpal[14*16+((color_index+1)&15)]);
							}
							
							color_index=(color_index+1)&15;
							break;
						}
						
						case '7':
							++RAMpal[14*16+color_index].r;
							RAMpal[14*16+color_index].r&=0x3F;
							break;
							
						case '1':
							--RAMpal[14*16+color_index].r;
							RAMpal[14*16+color_index].r&=0x3F;
							break;
							
						case '8':
							++RAMpal[14*16+color_index].g;
							RAMpal[14*16+color_index].g&=0x3F;
							break;
							
						case '2':
							--RAMpal[14*16+color_index].g;
							RAMpal[14*16+color_index].g&=0x3F;
							break;
							
						case '9':
							++RAMpal[14*16+color_index].b;
							RAMpal[14*16+color_index].b&=0x3F;
							break;
							
						case '3':
							--RAMpal[14*16+color_index].b;
							RAMpal[14*16+color_index].b&=0x3F;
							break;
						
						default:
							return ret;
					}
					edit_cset_dlg[9].flags |= D_DIRTY;
				}
			}
			ret |= D_USED_CHAR;
			broadcast_dialog_message(MSG_IDLE,0);
			
	}
	return ret;
}

void onInsertColor()
{
	RAMpal[14*16+color_index] = RAMpal[edc];
	set_palette_range(RAMpal,0,255,false);
	edit_cset_dlg[9].flags |= D_DIRTY;
}

void onInsertColor_Text()
{
	int r = atoi((char*)edit_cset_dlg[10].dp),
		g = atoi((char*)edit_cset_dlg[11].dp),
		b = atoi((char*)edit_cset_dlg[12].dp);
	if(edit_cset_dlg[19].flags & D_SELECTED)
	{
		r = vbound(r, 0, 252)/4;
		g = vbound(g, 0, 252)/4;
		b = vbound(b, 0, 252)/4;
	}
	else
	{
		r = vbound(r, 0, 63);
		g = vbound(g, 0, 63);
		b = vbound(b, 0, 63);
	}
	RAMpal[14*16+color_index].r = r;
	RAMpal[14*16+color_index].g = g;
	RAMpal[14*16+color_index].b = b;
	set_palette_range(RAMpal,0,255,false);
	edit_cset_dlg[9].flags |= D_DIRTY;
}

void jumpText(int r, int g, int b)
{
	if(edit_cset_dlg[19].flags & D_SELECTED)
	{
		r*=4;
		g*=4;
		b*=4;
	}
	char* c1 = (char*)edit_cset_dlg[10].dp;
	char* c2 = (char*)edit_cset_dlg[11].dp;
	char* c3 = (char*)edit_cset_dlg[12].dp;
	for(int q = 0; q < 2; ++q)
	{
		c1[q] = 0;
		c2[q] = 0;
		c3[q] = 0;
	}
	sprintf(c1, "%d", r);
	sprintf(c2, "%d", g);
	sprintf(c3, "%d", b);
	object_message(&edit_cset_dlg[10], MSG_DRAW, 0);
	object_message(&edit_cset_dlg[11], MSG_DRAW, 0);
	object_message(&edit_cset_dlg[12], MSG_DRAW, 0);
}

void onJumpText()
{
	jumpText(RAMpal[14*16+color_index].r, RAMpal[14*16+color_index].g, RAMpal[14*16+color_index].b);
}

void onJumpHSL()
{
	
}

void init_gfxpal()
{
	for(int i=0; i<128; i++)
	{
		RAMpal[i] = _RGB(gfx_pal+i*3); //hue
	}
	for(int i=0; i<32; i++)
	{
		RAMpal[i+128] = _RGB(i<<1,i<<1,i<<1); //lightness
	}
}

void edit_dataset(int dataset)
{
	PALETTE holdpal;
	memcpy(holdpal,RAMpal,sizeof(RAMpal));
	
	if(is_large)
	{
		rectfill(screen, 0, 0, screen->w, screen->h, 128);
		large_dialog(edit_cset_dlg);
	}
	
	load_cset(RAMpal,12,dataset);
	load_cset(RAMpal,14,dataset);
	set_palette_range(RAMpal,0,255,false);
	FONT* old = font;
	font = is_large?lfont_l:nfont;
	
	init_gfxpal();
	char bufr[4] = "0", bufg[4] = "0", bufb[4] = "0";
	edit_cset_dlg[10].dp = (void*)bufr;
	edit_cset_dlg[11].dp = (void*)bufg;
	edit_cset_dlg[12].dp = (void*)bufb;
	
	while(gui_mouse_b()) {} //Do nothing
	edit_cset_dlg[0].dp2 = lfont;
	int ret = zc_popup_dialog(edit_cset_dlg,3);
	al_trace("DLG RETURN VAL -------------------------- %d", ret);
	switch(ret)
	{
		case 3: //Cancel
			break;
		case 4: //OK
			get_cset(dataset,14,RAMpal);
			saved=false;
			break;
	}
	
	memcpy(RAMpal, holdpal, sizeof(holdpal));
	
	set_palette(RAMpal);
	rectfill(screen, 0, 0, screen->w, screen->h, BLACK);
	while(gui_mouse_b()) {} //Do nothing
	font = old;
}

int pal_index(RGB *pal,RGB c)
{
    for(int i=0; i<256; i++)
        if(pal[i].r==c.r && pal[i].g==c.g && pal[i].b==c.b)
            return i;
            
    return -1;
}

void grab_dataset(int dataset)
{
    int row=0;
    static int palx=176;
    static int paly=64;
    int imagex=0;
    int imagey=0;
    int buttonx=184;
    int buttony=195;
    int filenamex=0;
    int filenamey=232;
    
    if(is_large)
    {
        palx=665;
        paly=354;
        imagex=4;
        imagey=4;
        buttonx=570;
        buttony=521;
        filenamex=4;
        filenamey=583;
    }
    
    PALETTE tmp;
    
    if(!pic && load_the_pic(&pic,picpal))
        return;
        
    get_palette(imagepal);
    
    create_rgb_table(&rgb_table, imagepal, NULL);
    rgb_map = &rgb_table;
    create_color_table(&imagepal_table, RAMpal, return_RAMpal_color, NULL);
    
    int jwin_pal2[jcMAX];
    memcpy(jwin_pal2, jwin_pal, sizeof(int)*jcMAX);
    
    jwin_pal[jcBOX]    =imagepal_table.data[0][jwin_pal[jcBOX]];
    jwin_pal[jcLIGHT]  =imagepal_table.data[0][jwin_pal[jcLIGHT]];
    jwin_pal[jcMEDLT]  =imagepal_table.data[0][jwin_pal[jcMEDLT]];
    jwin_pal[jcMEDDARK]=imagepal_table.data[0][jwin_pal[jcMEDDARK]];
    jwin_pal[jcDARK]   =imagepal_table.data[0][jwin_pal[jcDARK]];
    jwin_pal[jcBOXFG]  =imagepal_table.data[0][jwin_pal[jcBOXFG]];
    jwin_pal[jcTITLEL] =imagepal_table.data[0][jwin_pal[jcTITLEL]];
    jwin_pal[jcTITLER] =imagepal_table.data[0][jwin_pal[jcTITLER]];
    jwin_pal[jcTITLEFG]=imagepal_table.data[0][jwin_pal[jcTITLEFG]];
    jwin_pal[jcTEXTBG] =imagepal_table.data[0][jwin_pal[jcTEXTBG]];
    jwin_pal[jcTEXTFG] =imagepal_table.data[0][jwin_pal[jcTEXTFG]];
    jwin_pal[jcSELBG]  =imagepal_table.data[0][jwin_pal[jcSELBG]];
    jwin_pal[jcSELFG]  =imagepal_table.data[0][jwin_pal[jcSELFG]];
    gui_bg_color=jwin_pal[jcBOX];
    gui_fg_color=jwin_pal[jcBOXFG];
    gui_mg_color=jwin_pal[jcMEDDARK];
    jwin_set_colors(jwin_pal);
    
    get_bw(picpal,pblack,pwhite);
    int bg = gui_bg_color;
    int fg = gui_fg_color;
    gui_bg_color = pblack;
    gui_fg_color = pwhite;
    
    char fname[13];
    extract_name(imagepath,fname,FILENAME8_3);
    
    draw_bw_mouse(pwhite, MOUSE_BMP_NORMAL, MOUSE_BMP_BLANK);
    scare_mouse();
    clear_bitmap(screen2);
    set_mouse_sprite(mouse_bmp[MOUSE_BMP_BLANK][0]);
    unscare_mouse();
    set_palette(picpal);
    
    bool redraw=true;
    bool reload=false;
    int done=0;
    int f=0;
    FONT *fnt = font;
    
    if(is_large)
        font = lfont_l;
        
    do
    {
        rest(1);
        int x=gui_mouse_x();
        int y=gui_mouse_y();
        
        custom_vsync();
        
        if(reload)
        {
            reload=false;
            
            if(load_the_pic(&pic,picpal)==2)
                done=1;
            else
            {
                clear_bitmap(screen2);
                set_palette(picpal);
                redraw=true;
            }
        }
        
        if(redraw)
        {
            redraw=false;
            scare_mouse();
            clear_to_color(screen2,is_large?jwin_pal[jcBOX]:pblack);
            
            if(is_large)
            {
                jwin_draw_frame(screen2,imagex-2,imagey-2,658,551,FR_DEEP);
                rectfill(screen2, imagex, imagey, imagex+654-1, imagey+547-1, jwin_pal[jcBOXFG]);
                jwin_draw_frame(screen2,palx-3,paly-3,134,134,FR_DEEP);
            }
            
            blit(pic,screen2,0,0,imagex,imagey,pic->w,pic->h);
            textout_ex(screen2,is_large?lfont_l:font,fname,filenamex,filenamey,is_large?jwin_pal[jcBOXFG]:pwhite,is_large?jwin_pal[jcBOX]:pblack);
            draw_text_button(screen2,buttonx,buttony+(is_large?36:24),(is_large?90:60),(is_large?31:21),"File",pblack,pwhite,0,true);
            draw_text_button(screen2,buttonx+(is_large?114:76),buttony,
                             (is_large?90:60),(is_large?31:21),"OK",pblack,pwhite,0,true);
            draw_text_button(screen2,buttonx+(is_large?114:76),buttony+(is_large?36:24),
                             (is_large?90:60),(is_large?31:21),"Cancel",pblack,pwhite,0,true);
            unscare_mouse();
        }
        
        if((gui_mouse_b()&1) && isinRect(x,y,palx,paly,palx+127,paly+127))
        {
            row=((y-paly)>>3);
        }
        
        if((gui_mouse_b()&1) && isinRect(x,y,buttonx,buttony+(is_large?36:24),buttonx+(is_large?90:60),buttony+(is_large?36+31:24+21)))
        {
            if(do_text_button(buttonx,buttony+(is_large?36:24),(is_large?90:60),(is_large?31:21),"File",pblack,pwhite,true))
            {
                reload=true;
            }
        }
        
        if((gui_mouse_b()&1) && isinRect(x,y,buttonx+(is_large?114:76),buttony,buttonx+(is_large?114+90:76+60),buttony+(is_large?31:21)))
        {
            if(do_text_button(buttonx+(is_large?114:76),buttony,(is_large?90:60),(is_large?31:21),"OK",pblack,pwhite,true))
            {
                done=2;
            }
        }
        
        if((gui_mouse_b()&1) && isinRect(x,y,buttonx+(is_large?114:76),buttony+(is_large?36:24),buttonx+(is_large?114+90:76+60),buttony+(is_large?36+31:24+21)))
        {
            if(do_text_button(buttonx+(is_large?114:76),buttony+(is_large?36:24),(is_large?90:60),(is_large?31:21),"Cancel",pblack,pwhite,true))
            {
                done=1;
            }
        }
        
        if(keypressed())
        {
            switch(readkey()>>8)
            {
            case KEY_ESC:
                done=1;
                break;
                
            case KEY_ENTER:
                done=2;
                break;
                
            case KEY_SPACE:
                reload=true;
                break;
                
            case KEY_UP:
                row=(row-1)&15;
                break;
                
            case KEY_DOWN:
                row=(row+1)&15;
                break;
                
            case KEY_LEFT:
            case KEY_RIGHT:
                if(!is_large)
                {
                    palx = 192-palx;
                    redraw=true;
                }
                
                break;
                
            case KEY_TAB:
                //        tmp = picpal;
                memcpy(tmp,picpal,sizeof(picpal));
                
                for(int i=0; i<16; i++)
                    tmp[(row<<4)+i] = invRGB(tmp[(row<<4)+i]);
                    
                for(int i=0; i<12; i++)
                {
                    custom_vsync();
                    
                    if(i&2)
                        set_palette(picpal);
                    else
                        set_palette(tmp);
                        
                    //if(zqwin_scale > 1)
                    {
                        //stretch_blit(screen, hw_screen, 0, 0, screen->w, screen->h, 0, 0, hw_screen->w, hw_screen->h);
                    }
                    //else
                    {
                        //blit(screen, hw_screen, 0, 0, 0, 0, screen->w, screen->h);
                    }
                }
                
                break;
            }
        }
        
        scare_mouse();
        
        for(int i=0; i<256; i++)
        {
            int x2=((i&15)<<3)+palx;
            int y2=((i>>4)<<3)+paly;
            rectfill(screen2,x2,y2,x2+7,y2+7,i);
        }
        
        ++f;
        rect(screen2,palx-1,paly-1,palx+128,paly+128,is_large?pblack:pwhite);
        rect(screen2,palx-1,(row<<3)+paly-1,palx+128,(row<<3)+paly+8,(f&2)?pwhite:pblack);
        blit(screen2, screen, 0, 0, 0, 0, screen->w, screen->h);
        unscare_mouse();
        SCRFIX();
        
        //if(zqwin_scale > 1)
        {
            //stretch_blit(screen, hw_screen, 0, 0, screen->w, screen->h, 0, 0, hw_screen->w, hw_screen->h);
        }
        //else
        {
            //blit(screen, hw_screen, 0, 0, 0, 0, screen->w, screen->h);
        }
    }
    while(!done);
    
    if(done==2)
        get_cset(dataset,row,picpal);
        
    font = fnt;
    
    gui_bg_color = bg;
    gui_fg_color = fg;
    
    while(gui_mouse_b())
    {
        /* do nothing */
    }
    
    scare_mouse();
    
    set_mouse_sprite(mouse_bmp[MOUSE_BMP_NORMAL][0]);
    clear_to_color(mouse_bmp[MOUSE_BMP_BLANK][0],0);
    memcpy(jwin_pal, jwin_pal2, sizeof(int)*jcMAX);
    
    gui_bg_color=jwin_pal[jcBOX];
    gui_fg_color=jwin_pal[jcBOXFG];
    gui_mg_color=jwin_pal[jcMEDDARK];
    jwin_set_colors(jwin_pal);
    
    rgb_map = &zq_rgb_table;
    unscare_mouse();
}

byte cset_hold[15][16*3];
byte cset_hold_cnt;
bool cset_ready = false;
int cset_count,cset_first;
PALETTE pal,undopal;

//#define FLASH 243
//byte rc[16] = {253,248,0,0,0,0,0,246,247,249,250,251,252,240,255,254};

void undo_pal()
{
    for(int i=0; i<cset_count; i++)
        get_cset(cset_first+i,i,undopal);
        
    //  undopal=pal;
    memcpy(undopal,pal,sizeof(pal));
}

void calc_dark(int first)
{
    //  undopal=pal;
    memcpy(undopal,pal,sizeof(pal));
    
    PALETTE tmp;
    //  tmp = black_palette;
    memcpy(tmp,black_palette,sizeof(black_palette));
    
    fade_interpolate(pal,black_palette,tmp,16,0,47);
    
    for(int i=0; i<48; i++)
        tmp[i+64] = tmp[i];
        
    fade_interpolate(pal,black_palette,tmp,32,0,47);
    
    for(int i=0; i<48; i++)
        tmp[i+112] = tmp[i];
        
    tmp[160+3] = tmp[3];
    
    for(int i=4; i<11; i++)
        get_cset(first+i,i,tmp);
}

static DIALOG cycle_dlg[] =
{
    // (dialog proc)     (x)   (y)   (w)   (h)   (fg)     (bg)    (key)    (flags)     (d1)           (d2)     (dp)
    { jwin_win_proc, 64,   48,   192+1,  152+1,  0,  0,  0,       D_EXIT,          0,             0, (void *) "Palette Cycles", NULL, NULL },
    { d_timer_proc,         0,    0,     0,    0,    0,       0,       0,       0,          0,          0,         NULL, NULL, NULL },
    { jwin_button_proc,     90,   176,  61,   21,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  13,      D_EXIT,     0,             0, (void *) "OK", NULL, NULL },
    { jwin_button_proc,     170,  176,  61,   21,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  27,      D_EXIT,     0,             0, (void *) "Cancel", NULL, NULL },
    { jwin_text_proc,       152,  72,   96,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "1   2   3", NULL, NULL },
    { jwin_text_proc,       152,  72,   96,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "1   2   3", NULL, NULL },
    { jwin_text_proc,       88,   88,   56,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "CSet:", NULL, NULL },
    { jwin_text_proc,       88,   104,  56,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "First:", NULL, NULL },
    { jwin_text_proc,       88,   120,  56,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "Last:", NULL, NULL },
    { jwin_text_proc,       88,   136,  56,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "Count:", NULL, NULL },
    { jwin_text_proc,       88,   152,  56,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "Speed:", NULL, NULL },
    // 10
    { jwin_edit_proc,       152,  88,   24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          1,             0,       NULL, NULL, NULL },
    { d_hexedit_proc,    152,  104,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          1,             0,       NULL, NULL, NULL },
    { d_hexedit_proc,    152,  120,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          1,             0,       NULL, NULL, NULL },
    { jwin_edit_proc,       152,  136,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          3,             0,       NULL, NULL, NULL },
    { jwin_edit_proc,       152,  152,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          3,             0,       NULL, NULL, NULL },
    
    { jwin_edit_proc,       184,  88,   24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          1,             0,       NULL, NULL, NULL },
    { d_hexedit_proc,    184,  104,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          1,             0,       NULL, NULL, NULL },
    { d_hexedit_proc,    184,  120,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          1,             0,       NULL, NULL, NULL },
    { jwin_edit_proc,       184,  136,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          3,             0,       NULL, NULL, NULL },
    { jwin_edit_proc,       184,  152,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          3,             0,       NULL, NULL, NULL },
    
    { jwin_edit_proc,       216,  88,   24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          1,             0,       NULL, NULL, NULL },
    { d_hexedit_proc,    216,  104,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          1,             0,       NULL, NULL, NULL },
    { d_hexedit_proc,    216,  120,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          1,             0,       NULL, NULL, NULL },
    { jwin_edit_proc,       216,  136,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          3,             0,       NULL, NULL, NULL },
    { jwin_edit_proc,       216,  152,  24,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          3,             0,       NULL, NULL, NULL },
    
    { NULL,                 0,    0,    0,    0,   0,       0,       0,       0,          0,             0,       NULL,                           NULL,  NULL }
};

void edit_cycles(int level)
{
    char buf[15][8];
    palcycle cycle_none[1][3];  //create a null palette cycle here. -Z
	memset(cycle_none, 0, sizeof(cycle_none)); 
    for(int i=0; i<3; i++)
    {
        palcycle c = ( level < 256 ) ? misc.cycles[level][i] : cycle_none[0][i]; //Only level palettes 0 through 255 have valid data in 2.50.x. -Z
        sprintf(buf[i*5  ],"%d",c.first>>4);
        sprintf(buf[i*5+1],"%X",c.first&15);
        sprintf(buf[i*5+2],"%X",c.count&15);
        sprintf(buf[i*5+3],"%d",c.count>>4);
        sprintf(buf[i*5+4],"%d",c.speed);
    }
    
    for(int i=0; i<15; i++)
        cycle_dlg[i+10].dp = buf[i];
        
    if(is_large)
        large_dialog(cycle_dlg);
        
    if(zc_popup_dialog(cycle_dlg,3)==2)
    {
        saved=false;
        reset_pal_cycling();
        
        for(int i=0; i<3; i++)
        {
            palcycle c;
            c.first = (atoi(buf[i*5])&7)<<4;
            c.first += xtoi(buf[i*5+1])&15;
            c.count =  xtoi(buf[i*5+2])&15;
            c.count += (atoi(buf[i*5+3])&15)<<4;
            c.speed =  atoi(buf[i*5+4]);
            misc.cycles[level][i] = c;
        }
    }
}

void draw_cset_proc(DIALOG *d)
{
    int d1 = zc_min(d->d1, d->d2);
    int d2 = zc_max(d->d1, d->d2);
    
    
    d_bitmap_proc(MSG_DRAW,d,0);
    //  text_mode(d->bg);
    
    //  rect(screen,d->x,(d1<<3)+d->y,d->x+d->w-1,(d2<<3)+d->y+7,FLASH);
    rect(screen,d->x,int((d1<<3)*(is_large?1.5:1)+d->y),d->x+d->w-1,int((d2<<3)*(is_large?1.5:1)+d->y+(is_large?11:7)),dvc(0)); //highlights currently selected cset
    int drc;
    
    if((d->flags & D_GOTFOCUS))
        drc = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
    else
        drc = d->bg;
        
    dotted_rect(d->x-1, d->y-1, d->x+d->w, d->y+d->h, drc, d->bg);
}

int d_cset_proc(int msg,DIALOG *d,int c)
{
    switch(msg)
    {
    case MSG_START:
        d->d2 = d->d1;
        break;
        
    case MSG_WANTFOCUS:
        return D_WANTFOCUS;
        
    case MSG_DRAW:
        draw_cset_proc(d);
        break;
        
    case MSG_CLICK:
    {
        bool dragging=false;
        int src=0;
        int x;
        int y;
        
        // Start dragging?
        if(key[KEY_ZC_LCONTROL] || key[KEY_ZC_RCONTROL])
        {
            x=gui_mouse_x();
            y=gui_mouse_y();
            
            if(isinRect(x,y,d->x,d->y,d->x+d->w-1,d->y+d->h-1))
            {
                dragging=true;
                src=vbound((int)((y-d->y) / (is_large?1.5:1))>>3,0,15) * 16 +
                    vbound((int)((x-d->x) / (is_large?1.5:1))>>3,0,15);
            }
        }
        
        do
        {
            x=gui_mouse_x();
            y=gui_mouse_y();
            
            if(!dragging && isinRect(x,y,d->x,d->y,d->x+d->w-1,d->y+d->h-1))
            {
                d->d2 = vbound((int)((y-d->y)/(is_large?1.5:1))>>3,0,15);
                
                if(!(key_shifts&KB_SHIFT_FLAG))
                    d->d1 = d->d2;
            }
            
            custom_vsync();
            scare_mouse();
            draw_cset_proc(d);
            unscare_mouse();
            //sniggles
            //        ((RGB*)d->dp3)[243]=((RGB*)d->dp3)[rc[(fc++)&15]];
            //        set_palette_range(((RGB*)d->dp3),FLASH,FLASH,false);
            ((RGB*)d->dp3)[dvc(0)]=((RGB*)d->dp3)[rand()%14+dvc(1)];
            set_palette_range(((RGB*)d->dp3),dvc(0),dvc(0),false);
            //if(zqwin_scale > 1)
            {
                //stretch_blit(screen, hw_screen, 0, 0, screen->w, screen->h, 0, 0, hw_screen->w, hw_screen->h);
            }
            //else
            {
                //blit(screen, hw_screen, 0, 0, 0, 0, screen->w, screen->h);
            }
        }
        while(gui_mouse_b());
        
        if(dragging && isinRect(x,y,d->x,d->y,d->x+d->w-1,d->y+d->h-1))
        {
            int dest=vbound((int)((y-d->y) / (is_large?1.5:1))>>3,0,15) * 16 +
                     vbound((int)((x-d->x) / (is_large?1.5:1))>>3,0,15);
                     
            if(src!=dest)
            {
                memcpy(undopal,pal,sizeof(pal));
                
                if(key[KEY_LSHIFT] || key[KEY_RSHIFT])
                {
                    for(int i=0; i<3; i++)
                        zc_swap(*(((byte*)d->dp2)+dest*3+i), *(((byte*)d->dp2)+src*3+i));
                }
                else
                {
                    for(int i=0; i<3; i++)
                        *(((byte*)d->dp2)+dest*3+i) = *(((byte*)d->dp2)+src*3+i);
                }
                
                for(int i=0; i<cset_count; i++)
                    load_cset(pal,i,cset_first+i);
                    
                set_palette(pal);
                saved=false;
            }
        }
    }
    break;
    
    case MSG_CHAR:
    {
        int shift = (key_shifts&KB_SHIFT_FLAG);
        int k=c>>8;
        
        switch(k)
        {
        case KEY_UP:
            if(d->d2>0) --d->d2;
            
            if(!shift) d->d1 = d->d2;
            
            break;
            
        case KEY_DOWN:
            if(d->d2<((int)ceil((d->h)/(is_large?1.5:1))>>3)-1)
                ++d->d2;
                
            if(!shift) d->d1 = d->d2;
            
            break;
            
        case KEY_PGUP:
            d->d2=0;
            
            if(!shift) d->d1 = d->d2;
            
            break;
            
        case KEY_PGDN:
            d->d2=((int)ceil((d->h)/(is_large?1.5:1))>>3)-1;
            
            if(!shift) d->d1 = d->d2;
            
            break;
            
        case KEY_C:
            cset_hold_cnt=0;
            
            for(int row=0; row <= abs(d->d1 - d->d2); row++)
            {
                int d1 = zc_min(d->d1,d->d2);
                ++cset_hold_cnt;
                
                for(int i=0; i<16*3; i++)
                    cset_hold[row][i] = *(((byte*)d->dp2)+CSET(d1+row)*3+i);
            }
            
            cset_ready=true;
            break;
            
            
        case KEY_V:
            if(cset_ready)
            {
                //         undopal=pal;
                memcpy(undopal,pal,sizeof(pal));
                int d1 = zc_min(d->d1,d->d2);
                
                for(int row=0; row<cset_hold_cnt && d1+row<cset_count; row++)
                {
                    for(int i=0; i<16*3; i++)
                        *(((byte*)d->dp2)+CSET(d1+row)*3+i) = cset_hold[row][i];
                }
                
                for(int i=0; i<cset_count; i++)
                    load_cset(pal,i,cset_first+i);
                    
                set_palette(pal);
                saved=false;
            }
            
            break;
            
        case KEY_U:
            undo_pal();
            
            for(int i=0; i<cset_count; i++)
                load_cset(pal,i,cset_first+i);
                
            set_palette(pal);
            break;
            
        default:
            return D_O_K;
        }
        
        custom_vsync();
        scare_mouse();
        draw_cset_proc(d);
        unscare_mouse();
        //if(zqwin_scale > 1)
        {
            //stretch_blit(screen, hw_screen, 0, 0, screen->w, screen->h, 0, 0, hw_screen->w, hw_screen->h);
        }
        //else
        {
            //blit(screen, hw_screen, 0, 0, 0, 0, screen->w, screen->h);
        }
        return D_USED_CHAR;
    }
    }
    
    return D_O_K;
}

byte mainpal_csets[30]    = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14, 11,11,12,12,12,11, 10,10,10,12,10,10,10,10,9 };
byte levelpal_csets[26]   = { 2,3,4,9,2,3,4,2,3,4, 2, 3, 4,       15,15,15,15, 7,7,7, 8,8,8, 0,0,0 };
byte levelpal2_csets[26]  = { 2,3,4,9,2,0,1,2,3,4, 5, 6, 7,       15,15,15,15, 8,  9,9,9,9,9,9,9,9 };
byte spritepal_csets[30]  = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14, 14,14,14,14,14,14, 14,14,14,14,14,14,14,14,14 };
byte spritepal2_csets[30] = { 15,16,17,18,19,20,21,22,23,24,25,26,27,28,29, 14,14,14,14,14,14, 14,14,14,14,14,14,14,14,14 };

static DIALOG colors_dlg[] =
{
    // (dialog proc)     (x)   (y)   (w)   (h)   (fg)     (bg)    (key)    (flags)     (d1)           (d2)     (dp)
    { jwin_win_proc,     40,   16,   240+1,  216+1,  jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       D_EXIT,          0,             0,       NULL, NULL, NULL },
    
    { jwin_frame_proc,   103,  48,   96,  256,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          FR_DEEP,             0,       NULL, NULL, NULL },
    { d_cset_proc,       106,  51,   256,  96,   jwin_pal[jcBOXFG],  jwin_pal[jcLIGHT],  0,       0,          0,             0,       NULL, NULL, NULL },
    
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    { jwin_rtext_proc,      102,   0,    16,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0,       NULL, NULL, NULL },
    
    { d_dummy_proc,       104,  39,   128,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "0123456789ABCDEF", NULL, NULL },
    // 19
    
    { jwin_button_proc,     60,   178,  61,   21,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  'e',     D_EXIT,     0,             0, (void *) "&Edit", NULL, NULL },
    { jwin_button_proc,     130,  178,  61,   21,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  'g',     D_EXIT,     0,             0, (void *) "&Grab", NULL, NULL },
    { jwin_button_proc,     200,  178,  61,   21,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       D_EXIT,     0,             0, (void *) "Cycle", NULL, NULL },
    { jwin_button_proc,     244,  152,  25,   21,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  'u',     D_EXIT,     0,             0, (void *) "&U", NULL, NULL },
    // 23
    
    { jwin_button_proc,     60,   204,  131,  21,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  'l',     D_EXIT,     0,             0, (void *) "&Load to CS 9", NULL, NULL },
    
    { jwin_button_proc,     200,  204,  61,   21,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  27,      D_EXIT,     0,             0, (void *) "Done", NULL, NULL },
    { jwin_rtext_proc,      60,   164,  48,   8,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "Name: ", NULL, NULL },
    { jwin_edit_proc,       88,  160, 136,   16,    jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  0,       0,          16,            0,       NULL, NULL, NULL },
    { jwin_ctext_proc,       110,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "0", NULL, NULL },
    { jwin_ctext_proc,       118,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "1", NULL, NULL },
    { jwin_ctext_proc,       126,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "2", NULL, NULL },
    { jwin_ctext_proc,       134,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "3", NULL, NULL },
    { jwin_ctext_proc,       142,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "4", NULL, NULL },
    { jwin_ctext_proc,       150,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "5", NULL, NULL },
    { jwin_ctext_proc,       158,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "6", NULL, NULL },
    { jwin_ctext_proc,       166,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "7", NULL, NULL },
    { jwin_ctext_proc,       174,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "8", NULL, NULL },
    { jwin_ctext_proc,       182,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "9", NULL, NULL },
    { jwin_ctext_proc,       190,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "A", NULL, NULL },
    { jwin_ctext_proc,       198,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "B", NULL, NULL },
    { jwin_ctext_proc,       206,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "C", NULL, NULL },
    { jwin_ctext_proc,       214,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "D", NULL, NULL },
    { jwin_ctext_proc,       222,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "E", NULL, NULL },
    { jwin_ctext_proc,       230,  39,   0,  8,    jwin_pal[jcBOXFG],   jwin_pal[jcBOX],  0,       0,          0,             0, (void *) "F", NULL, NULL },
    //43
    { jwin_button_proc,     244,  125,  25,   21,   jwin_pal[jcBOXFG],  jwin_pal[jcBOX],  's',     D_EXIT,     0,             0, (void *) "Save", NULL, NULL },
    
    { d_timer_proc,         0,    0,     0,    0,    0,       0,       0,       0,          0,          0,         NULL, NULL, NULL },
    { NULL,                 0,    0,    0,    0,   0,       0,       0,       0,          0,             0,       NULL,                           NULL,  NULL }
};

int EditColors(const char *caption,int first,int count,byte *label)
{

    char tempstuff[17];
    cset_first=first;
    cset_count=count;
    
    for(int i=240; i<256; i++)
    {
        pal[i] = RAMpal[i];
    }
    
    go();
    
    int bw = 128;
    int bh = count*8;
    
    if(colors_dlg[0].d1)
    {
        bw = (is_large?192:128);
        bh = count*(is_large?12:8);
    }
    
    BITMAP *bmp = create_bitmap_ex(8,(is_large?192:128),count*(is_large?12:8));
    
    if(!bmp)
        return 0;
        
    for(int i=0; i<16*count; i++)
    {
        int x=int(((i&15)<<3)*(is_large?1.5:1));
        int y=int(((i>>4)<<3)*(is_large?1.5:1));
        rectfill(bmp,x,y,x+(is_large ? 15:7),y+(is_large ? 15:7),i);
    }
    
    colors_dlg[2].dp = bmp;
    colors_dlg[2].w  = bw;
    colors_dlg[2].h  = bh;
    colors_dlg[2].d1 = colors_dlg[2].d1 = 0;
    colors_dlg[2].dp2 = colordata+CSET(first)*3;
    colors_dlg[2].dp3 = pal;
    colors_dlg[1].w  = bw + 6;
    colors_dlg[1].h  = bh + 6;
    colors_dlg[21].proc = (count==pdLEVEL) ? jwin_button_proc : d_dummy_proc;
    //if the palette is > 255, disable button [21]
    colors_dlg[21].dp   = get_bit(quest_rules,qr_FADE) ? (void *) "Cycle" : (void *) "Dark";
    colors_dlg[26].dp   =  tempstuff;
    colors_dlg[25].x    =(count==pdLEVEL)?colors_dlg[0].x+60:colors_dlg[0].x+12;
    colors_dlg[25].w    =(count==pdLEVEL)?48:0;
    colors_dlg[25].dp   =(count==pdLEVEL)?(void *) "Name: ":(void *) " ";
    colors_dlg[26].proc =(count==pdLEVEL) ? jwin_edit_proc : d_dummy_proc;
    colors_dlg[26].x    =(count==pdLEVEL)?colors_dlg[0].x+63:colors_dlg[0].x+12;
    colors_dlg[26].w    =(count==pdLEVEL)?134:0;
    colors_dlg[26].d1   =(count==pdLEVEL)?16:0;
    colors_dlg[26].dp   =(count==pdLEVEL)?palnames[(first-poLEVEL)/pdLEVEL]:NULL;
    
    //char (*buf)[4]= new char[count][4];
    char buf[50][4];
    
    for(int i=0; i<15; i++)
    {
        if(i<count)
        {
            sprintf(buf[i],"%2d",label[i]);
        }
        else
        {
            buf[i][0]=0;
        }
        
        colors_dlg[3+i].dp=buf[i];
        colors_dlg[3+i].y=colors_dlg[0].y+int(((i<<3)+36-(is_large?3:0))*((is_large && colors_dlg[0].d1)?1.5:1));
        //sniggles
        //    colors_dlg[3+i].fg=rc[label[i+count]];
    }
    
    colors_dlg[0].dp  = (void *)caption;
    colors_dlg[0].dp2 = lfont;
    
    for(int i=0; i<count; i++)
    {
        load_cset(pal,i,i+first);
    }
    
    memcpy(undopal,pal,sizeof(pal));
    int ret=0;
    
    do
    {
        rest(4);
        
        for(int i=0; i<count; i++)
        {
            load_cset(pal,i,i+first);
        }
        
        scare_mouse();
        clear_to_color(screen,0);
        set_palette(pal);
        unscare_mouse();
        colors_dlg[19].flags =
            colors_dlg[20].flags =
                colors_dlg[23].flags = D_EXIT;
                
        if(is_large)
        {
            if(!colors_dlg[0].d1)
            {
                colors_dlg[2].x  += 0;
                colors_dlg[2].y  -= 1;
            }
            
            large_dialog(colors_dlg);
        }
        
        DIALOG_PLAYER *p = init_dialog(colors_dlg,2);
        bool enable = true;
        
        while(update_dialog(p))
        {
            custom_vsync();
            //sniggles
            //      pal[FLASH]=pal[rc[(fc++)&15]];
            pal[dvc(0)]=pal[rand()%14+dvc(1)];
            set_palette_range(pal,dvc(0),dvc(0),false);
            
            bool en = (colors_dlg[2].d1 == colors_dlg[2].d2);
            
            if(en!=enable)
            {
                colors_dlg[19].flags =
                    colors_dlg[20].flags =
                        colors_dlg[23].flags = en  ? D_EXIT : D_DISABLED;
                broadcast_dialog_message(MSG_DRAW,0);
                enable = en;
            }
            
            //if(zqwin_scale > 1)
            {
                //stretch_blit(screen, hw_screen, 0, 0, screen->w, screen->h, 0, 0, hw_screen->w, hw_screen->h);
            }
            //else
            {
                //blit(screen, hw_screen, 0, 0, 0, 0, screen->w, screen->h);
            }
        }
        
        ret = shutdown_dialog(p);
        
        if(ret==19)
        {
            memcpy(undopal,pal,sizeof(pal));
            edit_dataset(first+colors_dlg[2].d2);
        }
        
        if(ret==20)
        {
            memcpy(undopal,pal,sizeof(pal));
            grab_dataset(first+colors_dlg[2].d2);
        }
        
        if(ret==21)
        {
		int curpal = (first/pdLEVEL+poLEVEL)-10; 
		
		
		    if(!get_bit(quest_rules,qr_FADE))
		    {
			calc_dark(first);
		    }
		    else
		    {
			if ( curpal < 256 ) //don't display cycle data for palettes 256 through 511. They don't have valid cycle data. 
				edit_cycles((first-poLEVEL)/pdLEVEL);
			else jwin_alert("Notice","Palettes above 0xFF do not have Palette Cycles",NULL,NULL,"O&K",NULL,'k',0,lfont);
		    }
		
        }
        
        if(ret==22)
        {
            undo_pal();
        }
	if ( ret == 43 )
	{
		if(getname("Save Palette (.png)","png",NULL,datapath,false))
		{
			//bool cancel;
			//char buf[80],buf2[80],
			char name[13];
			extract_name(temppath,name,FILENAME8_3);
			save_bitmap(temppath, bmp, pal);
			//save_bitmap("current_lvl_pal.png", bmp, pal);
		}
	}
    }
    while(ret<23&&ret!=0);
    
    while(gui_mouse_b())
    {
        /* do nothing */
    }
    
    clear_to_color(screen,vc(0));
    set_palette(RAMpal);
    
    loadlvlpal(Color);
    
    if(ret==23)
    {
        load_cset(RAMpal,9,first+colors_dlg[2].d2);
        set_pal();
    }
    
    saved=false; //It's just easier this way :)
    //  gui_fg_color = vc(14);
    //  gui_bg_color = vc(1);
    //  gui_mg_color = vc(9);
    
    comeback();
    destroy_bitmap(bmp);
    //delete[] buf;
    return int(ret==23);
}

int onColors_Main()
{
    int l9 = EditColors("Main Palette",0,pdFULL,mainpal_csets);
    
    // copy main to level 0
    int di = CSET(poLEVEL)*3;
    int si = CSET(2)*3;
    
    for(int i=0; i<CSET(3)*3; i++)
        colordata[di++] = colordata[si++];
        
    si = CSET(9)*3;
    
    for(int i=0; i<16*3; i++)
        colordata[di++] = colordata[si++];
        
    loadlvlpal(Color);
    
    if(l9)
    {
        load_cset(RAMpal,9,colors_dlg[2].d2);
        set_pal();
    }
    
    return D_O_K;
}

void copyPal(int src, int dest)
{
    int srcStart=CSET(src*pdLEVEL+poLEVEL)*3;
    int destStart=CSET(dest*pdLEVEL+poLEVEL)*3;
    
    for(int i=0; i<pdLEVEL*16*3; i++)
        colordata[destStart+i]=colordata[srcStart+i];
}

int onColors_Levels()
{
    int cycle = get_bit(quest_rules,qr_FADE);
    int index=Map.getcolor();
    
    while((index=select_data("Select Level",index,levelnumlist,"Edit","Done",lfont, copyPal))!=-1)
    {
        char buf[40];
        sprintf(buf,"Level %X Palettes",index);
        int l9 = EditColors(buf,index*pdLEVEL+poLEVEL,pdLEVEL,cycle?levelpal2_csets:levelpal_csets);
        setup_lcolors();
        
        if(index==0)
        {
            // copy level 0 to main
            int si = CSET(poLEVEL)*3;
            int di = CSET(2)*3;
            
            for(int i=0; i<CSET(3)*3; i++)
                colordata[di++] = colordata[si++];
                
            di = CSET(9)*3;
            
            for(int i=0; i<16*3; i++)
                colordata[di++] = colordata[si++];
        }
        
        loadlvlpal(Color);
        
        if(l9)
        {
            load_cset(RAMpal,9,index*pdLEVEL+poLEVEL+colors_dlg[2].d2);
            set_pal();
        }
    }
    
    return D_O_K;
}

int onColors_Sprites()
{
    int index;
    
    do
    {
        index = jwin_alert3("Edit Palette", "Select an extra sprite","palette set to edit",NULL,"&1","&2","&Done",'1','2','d', lfont);
        
        if(index==1)
            EditColors("Extra Sprite Palettes 1",newerpoSPRITE,15,spritepal_csets);
            
        if(index==2)
            EditColors("Extra Sprite Palettes 2",newerpoSPRITE+15,15,spritepal2_csets);
    }
    while(index==1 || index==2);
    
    return D_O_K;
}


void center_zq_cset_dialogs()
{
    jwin_center_dialog(cycle_dlg);
    jwin_center_dialog(colors_dlg);
	jwin_center_dialog(edit_cset_dlg);
}

