/*
 *	Copyright (C) 2004-2005 Michael Liebscher
 *	Copyright (C) 1997-2001 Id Software, Inc.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/*
 *	wolf_menu.c:   Menu management.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */


#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "wolf_menu.h"

#include "../entities/wolf_player.h"
#include "../wolf_local.h"
#include "../video/wolf_renderer.h"


#include "../../../font/font_manager.h"
#include "../../../client/client.h"
#include "../../../client/console.h"
#include "../../../client/menu_conf.h"
#include "../../../client/keys.h"
#include "../../../string/com_string.h"
#include "../../../device/timer/timer.h"
#include "../../../device/filesys/filesystem.h"
#include "../../../memory/memory.h"
#include "../../../video/video.h"
#include "../../../video/renderer.h"
#include "../../../sound/sound.h"


colour3_t bgcolour		= {	136, 0, 0 };
colour3_t bord2colour	= {	212, 0, 0 };
colour3_t deactive		= {	112, 0, 0 };
colour3_t bkgdcolour	= {	88, 0, 0 };

colour3_t readcolour	= {	180, 172, 0 };
colour3_t readhcolour	= {	252, 244, 0 };
colour3_t textcolour	= {	140, 140, 140 };
colour3_t highlight		= {	192, 192, 192 };


colour3_t bannerline	= {	96, 0, 0 };

colour3_t interbkgnd	= {	0, 64, 64 };


/* SOD colours */

colour3_t sodbgcolour	= {	0, 0, 152 };
colour3_t sodbord2colour= {	0, 0, 212 };
colour3_t soddeactive	= {	0, 0, 112 };
colour3_t sodbkgdcolour	= {	0, 0, 88 };


const char *menu_in_sound	= "lsfx/032.wav";
const char *menu_move_sound	= "lsfx/005.wav";
const char *menu_out_sound	= "lsfx/039.wav";


PRIVATE int	m_main_cursor;


void M_Menu_Main_f(void);
extern void M_Menu_Game_f(void);
extern void M_Menu_Skill_f(void);
extern void M_Menu_Options_f(void);
void M_Menu_LoadGame_f(void);
void M_Menu_SaveGame_f(void);
extern void M_Menu_Credits_f(void);
extern void M_Menu_Sound_f(void);
void M_Menu_Video_f(void);


extern void M_Menu_Keys_f(void);
extern void M_Menu_Quit_f(void);

extern void M_Menu_Credits(void);


extern void M_Intro_f(void);
extern void M_Intermission_f(void);

_boolean	m_entersound; /* play after drawing a frame, so that caching
						   * will NOT disrupt the sound */

void	(*m_drawfunc) (void);
const char *(*m_keyfunc) (int key);

/*============================================================================*/



/********************************************************************
 *
 *	Support Routines
 *
 *******************************************************************/


#define	MAX_MENU_DEPTH	8


typedef struct
{
	void	(*draw) (void);
	const char *(*key) (int k);

} menulayer_t;

menulayer_t	m_layers[MAX_MENU_DEPTH];
int		m_menudepth;


/*
 -----------------------------------------------------------------------------
 Function: M_Banner

 Parameters:
		name -[in]: string for the filename of the image/texture to draw.
		nYOffset -[in]: y coordinate (had been "nyOffest", but that looked like
									  it had been a typo, with the "e" and the
									  "s" switched, so I fixed it)

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
PUBLIC void M_Banner(const char *name, W16 nYOffset)
{
	SW32 w, h;

	if (g_version->value == SPEAROFDESTINY) {
		R_Draw_Fill(0, 20, (int)viddef.width, 44, colourBlack);
		R_Draw_Fill(0, 66, (int)viddef.width, 2, colourBlack);
	} else {
		R_Draw_Fill(0, 20, (int)viddef.width, 48, colourBlack);
		R_Draw_Fill(0, 64, (int)viddef.width, 2, bannerline);
	}

	TM_GetTextureSize(&w, &h, name);
#ifdef __i386__
	/* needs an extra cast: */
	R_Draw_Pic((int)((viddef.width - (unsigned long)(w)) >> 1), nYOffset, name);
#else
	R_Draw_Pic((int)((viddef.width - w) >> 1), nYOffset, name);
#endif /* __i386__ */
}


/*
 -----------------------------------------------------------------------------
 Function: M_BannerString

 Parameters:
		string -[in]:
		nYOffset -[in]:

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
PUBLIC void M_BannerString(const char *string, W16 nYOffset)
{
	int w, h;

	Font_GetMsgDimensions(1, string, &w, &h);
	Font_SetColour(1, readhcolour);
	Font_put_line(1, (int)((viddef.width - (unsigned int)(w)) >> 1),
				  (int)nYOffset, string);
}

/*
 -----------------------------------------------------------------------------
 Function: M_PushMenu

 Parameters: Yikes...

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
PUBLIC void M_PushMenu(void (*draw) (void), const char *(*key) (int k))
{
	int		i;

#if 0
	if (Cvar_VariableValue ("maxclients") == 1
		&& Com_ServerState ()) {
		Cvar_Set ("paused", "1");
	}
#endif /* 0 */

	/* if this menu is already present, drop back to that level
	 * to avoid stacking menus by hotkeys */
	for( i = 0; i < m_menudepth; ++i ) {
		if( m_layers[ i ].draw == draw &&
			m_layers[ i ].key == key ) {
			m_menudepth = i;
		}
	}

	if( i == m_menudepth ) {
		if( m_menudepth >= MAX_MENU_DEPTH ) {
			Com_Error( ERR_FATAL, "M_PushMenu: MAX_MENU_DEPTH" );
		}
		m_layers[ m_menudepth ].draw = m_drawfunc;
		m_layers[ m_menudepth ].key = m_keyfunc;
		m_menudepth++;
	}

	m_drawfunc = draw;
	m_keyfunc = key;

	m_entersound = true;

	ClientStatic.key_dest = key_menu;

}

/*
 -----------------------------------------------------------------------------
 Function: M_ForceMenuOff

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
PUBLIC void M_ForceMenuOff(void)
{
	m_drawfunc = 0;
	m_keyfunc = 0;

	if( r_world == NULL ) {
		ClientStatic.key_dest = key_console;
	} else {
		ClientStatic.key_dest = key_game;
	}

	m_menudepth = 0;
	Key_ClearStates();
	Cvar_Set( "paused", "0" );
}

/*
 -----------------------------------------------------------------------------
 Function: M_PopMenu

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_PopMenu(void)
{
#if 0
	Sound_StartLocalSound( menu_out_sound );
#endif /* 0 */
	if( m_menudepth < 1 ) {
		Com_Error( ERR_FATAL, "M_PopMenu: depth < 1" );
	}

	m_menudepth--;

	m_drawfunc = m_layers[ m_menudepth ].draw;
	m_keyfunc = m_layers[ m_menudepth ].key;

	if( ! m_menudepth ) {
		M_ForceMenuOff();
	}
}

/*
 -----------------------------------------------------------------------------
 Function: Default_MenuKey

 Parameters: key -[in] Current key that has been pressed.

 Returns:

 Notes:
 -----------------------------------------------------------------------------
 */
const char *Default_MenuKey(menuframework_s *m, int key)
{
	const char *sound = NULL;
	menucommon_s *item;

	if( m ) {
		if( ( item = Menu_ItemAtCursor( m ) ) != 0 ) {
			if ( item->type == MTYPE_FIELD ) {
				if ( Field_Key( ( menufield_s * ) item, key ) ) {
					return NULL;
				}
			}
		}
	}

	switch( key ) {
		case K_ESCAPE:
			M_PopMenu();
			return menu_out_sound;

		case K_KP_UPARROW:
		case K_UPARROW:
			if ( m ) {
				m->cursor--;
				Menu_AdjustCursor( m, -1 );
				sound = menu_move_sound;
			}
			break;
		case K_TAB:
			if ( m ) {
				m->cursor++;
				Menu_AdjustCursor( m, 1 );
				sound = menu_move_sound;
			}
			break;
		case K_KP_DOWNARROW:
		case K_DOWNARROW:
			if ( m ) {
				m->cursor++;
				Menu_AdjustCursor( m, 1 );
				sound = menu_move_sound;
			}
			break;
		case K_KP_LEFTARROW:
		case K_LEFTARROW:
			if ( m ) {
				Menu_SlideItem( m, -1 );
				sound = menu_move_sound;
			}
			break;
		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			if ( m ) {
				Menu_SlideItem( m, 1 );
				sound = menu_move_sound;
			}
			break;

		case K_MOUSE1:
		case K_MOUSE2:
		case K_MOUSE3:
		case K_JOY1:
		case K_JOY2:
		case K_JOY3:
		case K_JOY4:
		case K_AUX1:
		case K_AUX2:
		case K_AUX3:
		case K_AUX4:
		case K_AUX5:
		case K_AUX6:
		case K_AUX7:
		case K_AUX8:
		case K_AUX9:
		case K_AUX10:
		case K_AUX11:
		case K_AUX12:
		case K_AUX13:
		case K_AUX14:
		case K_AUX15:
		case K_AUX16:
		case K_AUX17:
		case K_AUX18:
		case K_AUX19:
		case K_AUX20:
		case K_AUX21:
		case K_AUX22:
		case K_AUX23:
		case K_AUX24:
		case K_AUX25:
		case K_AUX26:
		case K_AUX27:
		case K_AUX28:
		case K_AUX29:
		case K_AUX30:
		case K_AUX31:
		case K_AUX32:

		case K_KP_ENTER:
		case K_ENTER:
			if( m ) {
				Menu_SelectItem( m );
			}
			sound = menu_move_sound;
			break;
	}

	return sound;
}

/*============================================================================*/

/*
================================================================
M_DrawCharacter

(will eventually) draw one solid graphics character
cx and cy are in 320*240 coordinates, and should be centered on
higher res screens. Returns nothing.
================================================================
*/
void M_DrawCharacter(int cx, int cy, int num)
{
	/* both of these are dummy conditions just to use the parameters: */
	if ((cx < 320) && (cy < 240)) {
		;
	}
	if (num == 0) {
		;
	}
/* TODO: actually put something here for real? */
}

/*
 -----------------------------------------------------------------------------
 Function: M_Print

 Parameters:
		cx -[in]:
		cy -[in]:
		str -[in]:

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_Print(int cx, int cy, const char *str)
{
	while (*str) {
		M_DrawCharacter(cx, cy, ((*str) + 128));
		str++;
		cx += 8;
	}
}

/*
 -----------------------------------------------------------------------------
 Function: M_PrintWhite

 Parameters:
		cx -[in]:
		cy -[in]:
		str -[in]:

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_PrintWhite(int cx, int cy, const char *str)
{
	while (*str) {
		M_DrawCharacter(cx, cy, *str);
		str++;
		cx += 8;
	}
}

/*
 -----------------------------------------------------------------------------
 Function: M_DrawPic

 Parameters:
		x -[in]:
		y -[in]:
		pic -[in]:

 Returns: Nothing.

 Notes: Simple wrapper around R_DrawPic(), adjusted for the menu.
 -----------------------------------------------------------------------------
 */
void M_DrawPic(int x, int y, const char *pic)
{
	R_Draw_Pic((int)((unsigned int)(x) + ((viddef.width - 320) >> 1)),
			   (int)((unsigned int)(y) + ((viddef.height - 240) >> 1)), pic);
}


/*
=============================================
M_DrawCursor

Draws an animating cursor with the point at
x,y.  The pic will extend to the left of x,
and both above and below y. Returns nothing.
=============================================
*/
void M_DrawCursor(int x, int y, int f)
{
	char	cursorname[80];
	static _boolean cached;

	if (! cached) {
		int i;

		for ((i = 0); (i < NUM_CURSOR_FRAMES); ++i) {
			if( g_version->value == SPEAROFDESTINY ) {
				my_snprintf(cursorname, sizeof(cursorname),
							"pics/SC_CURSOR%dPIC.tga", i);
			} else {
				my_snprintf(cursorname, sizeof(cursorname),
							"pics/C_CURSOR%dPIC.tga", i);
			}

			TM_FindTexture(cursorname, TT_Pic);
		}
		cached = true;
	}


	if (g_version->value == SPEAROFDESTINY) {
		my_snprintf(cursorname, sizeof(cursorname),
					"pics/SC_CURSOR%dPIC.tga", f);
	} else {
		my_snprintf(cursorname, sizeof(cursorname),
					"pics/C_CURSOR%dPIC.tga", f);
	}

	R_Draw_Pic(x, y, cursorname);

}

/*
 -----------------------------------------------------------------------------
 Function: M_DrawTextBox

 Parameters:

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_DrawTextBox (int x, int y, int width, int lines)
{
	int		cx, cy;
	int		n;

	/* draw left side */
	cx = x;
	cy = y;
	M_DrawCharacter (cx, cy, 1);
	for ((n = 0); (n < lines); n++) {
		cy += 8;
		M_DrawCharacter(cx, cy, 4);
	}
	M_DrawCharacter(cx, (cy + 8), 7);

	/* draw middle */
	cx += 8;
	while (width > 0) {
		cy = y;
		M_DrawCharacter(cx, cy, 2);
		for ((n = 0); (n < lines); n++) {
			cy += 8;
			M_DrawCharacter(cx, cy, 5);
		}
		M_DrawCharacter(cx, (cy + 8), 8);
		width -= 1;
		cx += 8;
	}

	/* draw right side */
	cy = y;
	M_DrawCharacter(cx, cy, 3);
	for ((n = 0); (n < lines); ++n) {
		cy += 8;
		M_DrawCharacter(cx, cy, 6);
	}
	M_DrawCharacter(cx, (cy + 8), 9);
}

/*
 -----------------------------------------------------------------------------
 Function: M_DrawWindow

 Parameters:

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
PUBLIC void M_DrawWindow(int x, int y, int w, int h,
						 colour3_t bg, colour3_t act, colour3_t deact)
{
	R_Draw_Fill(x, y, w, h, bg);


	R_Draw_Line((x),		(y),	 (x + w), (y),			1, deact); /* Top */
	R_Draw_Line((x),		(y),	 (x),	  (y + h),		1, deact);/* Left */
	R_Draw_Line((x + w),	(y),	 (x + w), (y + h + 1),  1, act); /* Right */
	R_Draw_Line((x),		(y + h), (x + w), (y + h),		1, act);/* Bottom */
}

/*
 -----------------------------------------------------------------------------
 Function: M_DrawInfoBar

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_DrawInfoBar(void)
{
	SW32 w, h;

	if (g_version->value == SPEAROFDESTINY) {
		TM_GetTextureSize(&w, &h, "pics/SC_MOUSELBACKPIC.tga");
#ifdef __i386__
		/* needs some extra casts: */
		R_Draw_Pic((int)((viddef.width - (unsigned long)(w)) >> 1),
				   (int)(viddef.height - (unsigned long)(h)),
				   "pics/SC_MOUSELBACKPIC.tga");
#else
		R_Draw_Pic((int)((viddef.width - w) >> 1), (int)(viddef.height - h),
				   "pics/SC_MOUSELBACKPIC.tga");
#endif /* __i386__ */
	} else {
		TM_GetTextureSize(&w, &h, "pics/C_MOUSELBACKPIC.tga");
#ifdef __i386__
		/* needs some extra casts: */
		R_Draw_Pic((int)((viddef.width - (unsigned long)(w)) >> 1),
				   (int)(viddef.height - (unsigned long)(h)),
				   "pics/C_MOUSELBACKPIC.tga");
#else
		R_Draw_Pic((int)((viddef.width - w) >> 1), (int)(viddef.height - h),
				   "pics/C_MOUSELBACKPIC.tga");
#endif /* __i386__ */
	}
}


/* ===================================================================== */


/*
=======================================================================

	MAIN MENU
	---------

=======================================================================
*/

#define	MAIN_ITEMS	10	/* Number of menu items on the main menu. */
						/* Used to keep track of cursor position. */

PRIVATE const char *menunames[] =
{
	"New Game",
	"Sound",
	"Control",
	"Video",
	"Load Game",
	"Save Game",
	"View Scores",
	"End Game",
	"Credits",
	"Quit",
	0
};


/*
-----------------------------------------------------------------------------
 Function: M_Main_Draw	-Renders the main menu.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
void M_Main_Draw(void)
{
	int i;
	SW32 w, h;
	int cx, cy;


	R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, bgcolour);

	M_Banner("pics/C_OPTIONSPIC.tga", (W16)0);


	TM_GetTextureSize(&w, &h, "pics/C_MOUSELBACKPIC.tga");
#ifdef __i386__
	/* needs some extra casts: */
	R_Draw_Pic((int)((viddef.width - (unsigned long)(w)) >> 1),
			   (int)(viddef.height - (unsigned long)(h)),
			   "pics/C_MOUSELBACKPIC.tga");
#else
	R_Draw_Pic((int)((viddef.width - w) >> 1), (int)(viddef.height - h),
			   "pics/C_MOUSELBACKPIC.tga");
#endif /* __i386__ */

	cx = ((viddef.width - 356) >> 1);
	cy = ((viddef.height - 272) >> 1);
	M_DrawWindow(cx, cy, 356, 272, bkgdcolour, bord2colour, deactive);

	for ((i = 0); (menunames[i] != 0); ++i) {
		if (m_main_cursor == i) {
			Menu_DrawString(FONT1, (cx + 60), (cy + 5 + (i * 26)), menunames[i],
							highlight);
		} else {
			Menu_DrawString(FONT1, (cx + 60), (cy + 5 + (i * 26)), menunames[i],
							textcolour);
		}
	}

	M_DrawCursor((cx + 3), (cy + (m_main_cursor * 26) + 2),
				 ((int)(ClientStatic.realtime / 1000) % NUM_CURSOR_FRAMES));

}

/*
-----------------------------------------------------------------------------
 Function: M_Main_Key	-Handles key input for the main menu.

 Parameters: key -[in] Current key that has been pressed.

 Returns: Pointer to a string that contains the filename for the sound effect to
		  be played, NULL otherwise.

 Notes:
-----------------------------------------------------------------------------
*/
const char *M_Main_Key(int key)
{
	const char *sound = menu_move_sound;

	switch (key) {
		case K_ESCAPE:
			M_PopMenu();
			break;

		case K_KP_DOWNARROW:
		case K_DOWNARROW:
			if (++m_main_cursor >= MAIN_ITEMS) {
				m_main_cursor = 0;
			}
			return sound;

		case K_KP_UPARROW:
		case K_UPARROW:
			if (--m_main_cursor < 0) {
				m_main_cursor = (MAIN_ITEMS - 1);
			}
			return sound;

		case K_KP_ENTER:
		case K_ENTER:
			m_entersound = true;

			switch (m_main_cursor) {
				case 0:
					if (g_version->value == SPEAROFDESTINY) {
						M_Menu_Skill_f();
					} else {
						M_Menu_Game_f();
					}
					break;

				case 1:
					M_Menu_Sound_f();
					break;

				case 2:
					M_Menu_Options_f();
					break;

				case 3:
					M_Menu_Video_f();
					break;

				case 4:
					M_Menu_LoadGame_f();
					break;

				case 5:
					M_Menu_SaveGame_f();
					break;

				case 6:

					break;

				case 8:
					M_Menu_Credits_f();
					break;

				case 9:
					M_Menu_Quit_f();
					break;
			}
	}

	return NULL;
}


/*
 -----------------------------------------------------------------------------
 Function: M_SMain_Draw

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
/* SOD main menu */
void M_SMain_Draw(void)
{
	int i;
	SW32 w, h;
	int cx, cy;


	R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height,
				"pics/C_BACKDROPPIC.tga");

	M_Banner("pics/SC_OPTIONSPIC.tga", (W16)0);


	TM_GetTextureSize(&w, &h, "pics/SC_MOUSELBACKPIC.tga");
#ifdef __i386__
	/* needs some extra casts: */
	R_Draw_Pic((int)((viddef.width - (unsigned long)(w)) >> 1),
			   (int)(viddef.height - (unsigned long)(h)),
			   "pics/SC_MOUSELBACKPIC.tga");
#else
	R_Draw_Pic((int)((viddef.width - w) >> 1), (int)(viddef.height - h),
			   "pics/SC_MOUSELBACKPIC.tga");
#endif /* __i386__ */

	cx = ((viddef.width - 356) >> 1);
	cy = ((viddef.height - 272) >> 1);
	M_DrawWindow(cx, cy, 356, 272, sodbkgdcolour, sodbord2colour, soddeactive);

	for ((i = 0); (menunames[i] != 0); ++i) {
		if (m_main_cursor == i) {
			Menu_DrawString(FONT1, (cx + 60), (cy + 5 + (i * 26)), menunames[i],
							highlight);
		} else {
			Menu_DrawString(FONT1, (cx + 60), (cy + 5 + (i * 26)), menunames[i],
							textcolour);
		}
	}

	M_DrawCursor((cx + 3), (cy + (m_main_cursor * 26) + 2),
				 ((int)(ClientStatic.realtime / 1000) % NUM_CURSOR_FRAMES));
}


/*
 -----------------------------------------------------------------------------
 Function: M_Menu_Main_f

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_Menu_Main_f(void)
{
	Sound_StopBGTrack();
	Sound_StartBGTrack( "music/WONDERIN.ogg", "music/WONDERIN.ogg" );

	if (g_version->value == SPEAROFDESTINY) {
		M_PushMenu(M_SMain_Draw, M_Main_Key);

		return;
	}

	M_PushMenu(M_Main_Draw, M_Main_Key);

}


/*
=======================================================================

	VIDEO MENU

=======================================================================
*/

extern void Video_MenuDraw(void);
extern const char *Video_MenuKey(int key);

/*
 -----------------------------------------------------------------------------
 Function: M_Menu_Video_f

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_Menu_Video_f(void)
{
	Video_MenuInit();
	M_PushMenu(Video_MenuDraw, Video_MenuKey);
}


/*
=============================================================================

	LOADGAME MENU
	-------------

=============================================================================
*/

#define	MAX_SAVEGAMES	10

static menuframework_s	s_savegame_menu;

static menuframework_s	s_loadgame_menu;
static menuaction_s		s_loadgame_actions[MAX_SAVEGAMES];

char		m_savestrings[MAX_SAVEGAMES][32];
_boolean	m_savevalid[MAX_SAVEGAMES];

/*
 -----------------------------------------------------------------------------
 Function: Create_Savestrings

 Parameters: Nothing

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void Create_Savestrings(void)
{
	;
/* TODO: actually put something here? */
}

/*
 -----------------------------------------------------------------------------
 Function: LoadGameCallback

 Parameters: Nothing (?)

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void LoadGameCallback(void *self)
{
	menuaction_s *a = (menuaction_s *)self;

	if (m_savevalid[a->generic.localdata[0]]) {
		Cbuf_AddText(va("load save%i\n",  a->generic.localdata[0]));
	}
	M_ForceMenuOff();
}

/*
 -----------------------------------------------------------------------------
 Function: LoadGame_MenuInit

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void LoadGame_MenuInit(void)
{
	int i;

	s_loadgame_menu.x = (int)((viddef.width / 2) - 120);
	s_loadgame_menu.y = (int)((viddef.height / 2) - 58);
	s_loadgame_menu.nitems = 0;

	Create_Savestrings();

	for ((i = 0); (i < MAX_SAVEGAMES); ++i) {
		s_loadgame_actions[i].generic.name			= m_savestrings[i];
		s_loadgame_actions[i].generic.flags			= MF_LEFT_JUSTIFY;
		s_loadgame_actions[i].generic.localdata[0]	= i;
		s_loadgame_actions[i].generic.callback		= LoadGameCallback;

		s_loadgame_actions[i].generic.x = 0;
		s_loadgame_actions[i].generic.y = ( i ) * 20;
		if (i > 0) { /* separate from autosave */
			s_loadgame_actions[i].generic.y += 20;
		}

		s_loadgame_actions[i].generic.type = MTYPE_ACTION;

		Menu_AddItem(&s_loadgame_menu, &s_loadgame_actions[i]);
	}
}

/*
 -----------------------------------------------------------------------------
 Function: LoadGame_MenuDraw

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void LoadGame_MenuDraw(void)
{
	if (g_version->value == SPEAROFDESTINY) {
		R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height,
					"pics/C_BACKDROPPIC.tga");

		M_Banner("pics/SC_LOADGAMEPIC.tga", (W16)0);
	} else {
		R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, bgcolour);

		M_Banner("pics/C_LOADGAMEPIC.tga", (W16)0);
	}
#if 0 || __clang_analyzer__
	Menu_AdjustCursor(&s_loadgame_menu, 1);
	Menu_Draw(&s_loadgame_menu);
#endif /* 0 || __clang_analyzer__ */
}

/*
 -----------------------------------------------------------------------------
 Function: LoadGame_MenuKey

 Parameters: key -[in] Current key that has been pressed.

 Returns:

 Notes:
 -----------------------------------------------------------------------------
 */
const char *LoadGame_MenuKey(int key)
{
	if ((key == K_ESCAPE) || (key == K_ENTER)) {
		s_savegame_menu.cursor = (s_loadgame_menu.cursor - 1);
		if (s_savegame_menu.cursor < 0) {
			s_savegame_menu.cursor = 0;
		}
	}
	return Default_MenuKey(&s_loadgame_menu, key);
}

/*
 -----------------------------------------------------------------------------
 Function: M_Menu_LoadGame_f

 Parameters:

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_Menu_LoadGame_f(void)
{
	LoadGame_MenuInit();
	M_PushMenu(LoadGame_MenuDraw, LoadGame_MenuKey);
}


/*
=============================================================================

	SAVEGAME MENU
	-------------

=============================================================================
*/
static menuframework_s	s_savegame_menu;
static menuaction_s		s_savegame_actions[MAX_SAVEGAMES];

/*
 -----------------------------------------------------------------------------
 Function: SaveGameCallback

 Parameters: Nothing (?)

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void SaveGameCallback(void *self)
{
	menuaction_s *a = (menuaction_s *)self;

	Cbuf_AddText(va("save save%i\n", a->generic.localdata[0]));
	M_ForceMenuOff();
}

/*
 -----------------------------------------------------------------------------
 Function: SaveGame_MenuDraw

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void SaveGame_MenuDraw(void)
{
	if (g_version->value == SPEAROFDESTINY) {
		R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height,
					"pics/C_BACKDROPPIC.tga");

		M_Banner("pics/SC_SAVEGAMEPIC.tga", (W16)0);
	} else {
		R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, bgcolour);

		M_Banner("pics/C_SAVEGAMEPIC.tga", (W16)0);
	}
#if 0 || __clang_analyzer__
	Menu_AdjustCursor(&s_savegame_menu, 1);
	Menu_Draw(&s_savegame_menu);
#endif /* 0 || __clang_analyzer__ */
}

/*
 -----------------------------------------------------------------------------
 Function: SaveGame_Menu_init

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void SaveGame_MenuInit(void)
{
	int i;

	s_savegame_menu.x = (int)((viddef.width / 2) - 120);
	s_savegame_menu.y = (int)((viddef.height / 2) - 58);
	s_savegame_menu.nitems = 0;

	Create_Savestrings();

	/* do NOT include the autosave slot */
	for ((i = 0); (i < (MAX_SAVEGAMES - 1)); ++i ) {
		s_savegame_actions[i].generic.name = m_savestrings[(i + 1)];
		s_savegame_actions[i].generic.localdata[0] = (i + 1);
		s_savegame_actions[i].generic.flags = MF_LEFT_JUSTIFY;
		s_savegame_actions[i].generic.callback = SaveGameCallback;

		s_savegame_actions[i].generic.x = 0;
		s_savegame_actions[i].generic.y = ((i) * 20);

		s_savegame_actions[i].generic.type = MTYPE_ACTION;

		Menu_AddItem(&s_savegame_menu, &s_savegame_actions[i]);
	}
}

/*
 -----------------------------------------------------------------------------
 Function: SaveGame_MenuKey

 Parameters: key -[in] Current key that has been pressed.

 Returns: Looks like a string?

 Notes: A little more than your average simple wrapper around Default_MenuKey()
		this time, actually.
 -----------------------------------------------------------------------------
 */
const char *SaveGame_MenuKey(int key)
{
	if ((key == K_ENTER) || (key == K_ESCAPE)) {
		s_loadgame_menu.cursor = (s_savegame_menu.cursor - 1);
		if (s_loadgame_menu.cursor < 0) {
			s_loadgame_menu.cursor = 0;
		}
	}
	return Default_MenuKey(&s_savegame_menu, key);
}

/*
 -----------------------------------------------------------------------------
 Function: M_Menu_SaveGame_f

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_Menu_SaveGame_f(void)
{
#if 0 || __clang_analyzer__
	if (! Com_ServerState()) {
		return;	/* not playing a game */
	}
#endif /* 0 || __clang_analyzer__ */

	SaveGame_MenuInit();
	M_PushMenu(SaveGame_MenuDraw, SaveGame_MenuKey);
	Create_Savestrings();
}



/*============================================================================*/
/* Menu Subsystem */

/*
 -----------------------------------------------------------------------------
 Function: Menu_Init

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Just adds a bunch of commands, looks like...
 -----------------------------------------------------------------------------
 */
void Menu_Init(void)
{
	Cmd_AddCommand("intro", M_Intro_f);
	Cmd_AddCommand("intermission", M_Intermission_f);

	Cmd_AddCommand("menu_main", M_Menu_Main_f);
	Cmd_AddCommand("menu_game", M_Menu_Game_f);
	Cmd_AddCommand("menu_loadgame", M_Menu_LoadGame_f);
	Cmd_AddCommand("menu_savegame", M_Menu_SaveGame_f);
	Cmd_AddCommand("menu_credits", M_Menu_Credits_f );
	Cmd_AddCommand("menu_video", M_Menu_Video_f);
	Cmd_AddCommand("menu_options", M_Menu_Options_f);
	Cmd_AddCommand("menu_keys", M_Menu_Keys_f);
	Cmd_AddCommand("menu_quit", M_Menu_Quit_f);
}


/*
 -----------------------------------------------------------------------------
 Function: M_Draw

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_Draw(void)
{
	if (ClientStatic.key_dest != key_menu) {
		return;
	}

#if 0 /* try other conditions */
	/* repaint everything next frame */
	SCR_DirtyScreen();

	/* dim everything behind it down */
	if (cl.cinematictime > 0) { /* 'cl' is undeclared */
		R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, bgcolour);
	} else {
		Draw_FadeScreen(); /* unimplemented */
	}
#endif /* 0 */

	m_drawfunc(); /* function pointer from up top */

	/* delay playing the enter sound until after the
	 * menu has been drawn, to avoid delay while
	 * caching images */
	if (m_entersound) {
		Sound_StartLocalSound(menu_in_sound);
		m_entersound = false;
	}
}

/*
 -----------------------------------------------------------------------------
 Function: M_Keydown

 Parameters: key -[in] Current key that has been pressed.

 Returns: Nothing.

 Notes:
 -----------------------------------------------------------------------------
 */
void M_Keydown(int key)
{
	const char *s;

	if (m_keyfunc) {
		if ((s = m_keyfunc(key)) != 0) {
			Sound_StartLocalSound((char *) s);
		}
	}
}

/* EOF */
