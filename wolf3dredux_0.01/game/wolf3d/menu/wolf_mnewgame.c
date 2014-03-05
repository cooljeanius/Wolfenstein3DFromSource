/*
 *	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 */

/*
 *	wolf_mnewgame.c:   New game menu.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <string.h>

#include "wolf_menu.h"

#include "../wolf_local.h"
#include "../entities/wolf_player.h"

#include "../../../client/client.h"
#include "../../../string/com_string.h"
#include "../../../video/renderer.h"
#include "../../../sound/sound.h"

#include "../script/episode.h"


extern void M_Menu_Skill_f( void );

/********************************************************************
 *
 *	NEW GAME MENU
 *
 *******************************************************************/

#define MAX_EPISODES	6

PRIVATE int		m_game_cursor;

PRIVATE menuframework_s	s_game_menu;
PRIVATE menuaction_s	s_episode_actions[ MAX_EPISODES ];


PRIVATE void SetEpisodeGameFunc( void *data )
{
	menuaction_s *a = (menuaction_s *)data;
	char num[ 16 ];

	my_snprintf( num, sizeof( num ), "%d", a->generic.parent->cursor );

	Cvar_ForceSet( "episode", num );

	M_Menu_Skill_f();
}


PRIVATE void MenuCursorDrawFunc( menuframework_s *menu )
{
	M_DrawCursor( ((viddef.width - 616) >> 1) + 5, 70 + menu->cursor * 60, (int)(ClientStatic.realtime / 1000) % NUM_CURSOR_FRAMES );
}

PRIVATE void MenuDrawNewGameImages( void )
{
	char buffer[ 24 ];
	int i;

	for( i = 0; i < 6; ++i ) {
		my_snprintf( buffer, sizeof( buffer ),  "pics/C_EPISODE%dPIC.tga",  i+1 );
		R_Draw_Pic( ((viddef.width - 616) >> 1) + 69, 70 + i * 60, buffer );
	}
}

PRIVATE void Game_MenuInit( void )
{
	static char *episode_names[] =
	{
		"Episode 1\nEscape from Wolfenstein",
		"Episode 2\nOperation: Eisenfaust",
		"Episode 3\nDie, Fuhrer, Die!",
		"Episode 4\nA Dark Secret",
		"Episode 5\nTrail of the Madman",
		"Episode 6\nConfrontation",
		0
	};

	int i;

	s_game_menu.x = (viddef.width - 616) >> 1;
	s_game_menu.nitems = 0;
	s_game_menu.cursordraw = MenuCursorDrawFunc;


	for( i = 0; episode_names[ i ] != 0; ++i ) {
		s_episode_actions[ i ].generic.type	= MTYPE_ACTION;
		s_episode_actions[ i ].generic.flags  = MF_LEFT_JUSTIFY;
		s_episode_actions[ i ].generic.x		=  175;
		s_episode_actions[ i ].generic.y		=  70 + i * 60;
		s_episode_actions[ i ].generic.fs		= FONT1;
		s_episode_actions[ i ].generic.fontBaseColour = &textcolour;
		s_episode_actions[ i ].generic.fontHighColour = &highlight;
		s_episode_actions[ i ].generic.name	= episode_names[ i ];
		s_episode_actions[ i ].generic.callback = SetEpisodeGameFunc;

		Menu_AddItem( &s_game_menu, &s_episode_actions[ i ] );
	}

#if 0
	Menu_Center( &s_game_menu );
#endif /* 0 */
}

PRIVATE void Game_MenuDraw( void )
{
	R_Draw_Fill( 0, 0, viddef.width, viddef.height, bgcolour );

	M_BannerString( "Which episode to play?", 15 );

	M_DrawWindow(((viddef.width - 616)>>1), 60, 616, 370,
				 bkgdcolour, bord2colour, deactive );
	M_DrawInfoBar();
	MenuDrawNewGameImages();
	Menu_AdjustCursor( &s_game_menu, 1 );
	Menu_Draw( &s_game_menu );
}

PRIVATE const char *Game_MenuKey( int key )
{
	return Default_MenuKey( &s_game_menu, key );
}

PUBLIC void M_Menu_Game_f (void)
{
	Game_MenuInit();
	M_PushMenu( Game_MenuDraw, Game_MenuKey );
	m_game_cursor = 1;
}

/********************************************************************
 *
 *	SKILL MENU
 *
 *******************************************************************/

#define	MAX_SKILLS	6

PRIVATE menuframework_s	s_skill_menu;
PRIVATE menuaction_s	s_skill_actions[ MAX_SKILLS ];


PRIVATE void StartGame( void )
{
	char szTextMsg[ 128 ];
	char *mapfilename;

	memset( &LevelRatios, 0, sizeof( LevelRatios ) );

	PL_NewGame( &Player );

	Sound_StopBGTrack();

#if 0
	/* disable updates and start the cinematic going */
	cl.servercount = -1;
#endif /* 0 */
	M_ForceMenuOff();
#if 0
	Cvar_SetValue( "deathmatch", 0 );
	Cvar_SetValue( "coop", 0 );

	Cvar_SetValue( "gamerules", 0 ); /* PGM */
#endif /* 0 */

	if( g_version->value == SPEAROFDESTINY ) {
		my_snprintf( szTextMsg, sizeof( szTextMsg ),
			"loading ; map s%.2d.map\n", 0 );
	} else {
		switch( (int)episode->value ) {
			case 0:
				episode_setEpisode( "escapeFromWolfenstein" );
				break;

			case 1:
				episode_setEpisode( "OperationEisenfaust" );
				break;

			case 2:
				episode_setEpisode( "DieFuhrerDie" );
				break;

			case 3:
				episode_setEpisode( "ADarkSecret" );
				break;

			case 4:
				episode_setEpisode( "TrailoftheMadman" );
				break;

			case 5:
				episode_setEpisode( "Confrontation" );
				break;
		}

		mapfilename = episode_getCurrentMapFileName( &floornumber );

		if( ! mapfilename ) {
			Com_Printf( "Unable to get current map file name\n" );

			ClientStatic.key_dest = key_console;

			return;
		}

		my_snprintf( szTextMsg, sizeof( szTextMsg ),
			"loading ; map %s\n", mapfilename );
	}

	Cbuf_AddText( szTextMsg );

}

PRIVATE void ToughPic( int i )
{
	char string[ 32 ];

	if( g_version->value == SPEAROFDESTINY ) {
		my_snprintf( string, sizeof( string ), "pics/SC_SKILL%dPIC.tga", i+1 );
	} else {
		my_snprintf( string, sizeof( string ), "pics/C_SKILL%dPIC.tga", i+1 );
	}


	R_Draw_Pic( ((viddef.width - 450) >> 1) + 375, 214, string );
}


PRIVATE void SkillMenuCursorDrawFunc( void *data )
{
	menuframework_s *menu = (menuframework_s *)data;
	M_DrawCursor( ((viddef.width - 450) >> 1) + 5, 187 + menu->cursor * 30, (int)(ClientStatic.realtime / 1000) % NUM_CURSOR_FRAMES );
	ToughPic( menu->cursor );
}


PRIVATE void SkillGameFunc( void *data )
{
	menuaction_s *a = (menuaction_s *)data;
	char num[ 16 ];

	my_snprintf( num, sizeof( num ), "%d", a->generic.parent->cursor );

	Cvar_ForceSet( "skill", num );
	StartGame();
}


PRIVATE void Skill_MenuInit( void )
{
	static const char *skill_names[] =
	{
		"Can I play, Daddy?",
		"Don't hurt me.",
		"Bring 'em on!",
		"I am Death incarnate!",
		0
	};

	int i;


	s_skill_menu.x = ((viddef.width - 450) >> 1);
	s_skill_menu.y = 0;
	s_skill_menu.nitems = 0;
	s_skill_menu.cursordraw = SkillMenuCursorDrawFunc;


	for( i = 0; skill_names[ i ] != 0; ++i ) {
		s_skill_actions[ i ].generic.type	= MTYPE_ACTION;
		s_skill_actions[ i ].generic.flags  = MF_LEFT_JUSTIFY;
		s_skill_actions[ i ].generic.x		= 65;
		s_skill_actions[ i ].generic.y		= 190 + i * 30;
		s_skill_actions[ i ].generic.fs		= FONT1;
		s_skill_actions[ i ].generic.fontBaseColour = &textcolour;
		s_skill_actions[ i ].generic.fontHighColour = &highlight;
		s_skill_actions[ i ].generic.name	= skill_names[ i ];
		s_skill_actions[ i ].generic.callback = SkillGameFunc;

		Menu_AddItem( &s_skill_menu, &s_skill_actions[ i ] );
	}
}

PRIVATE void Skill_MenuDraw( void )
{
	if( g_version->value == SPEAROFDESTINY ) {
		SW32 w, h;

		R_Draw_Tile( 0, 0, viddef.width, viddef.height, "pics/C_BACKDROPPIC.tga" );

		TM_GetTextureSize( &w, &h, "pics/C_HOWTOUGHPIC.tga" );
		R_Draw_Pic( (viddef.width - w) >> 1, 136, "pics/C_HOWTOUGHPIC.tga" );

		M_DrawWindow(  ((viddef.width - 450) >> 1), 180, 450, 134,
			sodbkgdcolour, sodbord2colour, soddeactive );
	} else {
		R_Draw_Fill( 0, 0, viddef.width, viddef.height, bgcolour );

		M_BannerString( "How tough are you?", 136 );

		M_DrawWindow(  ((viddef.width - 450) >> 1), 180, 450, 134,
			bkgdcolour, bord2colour, deactive );
	}

	M_DrawInfoBar();

	Menu_AdjustCursor( &s_skill_menu, 1 );
	Menu_Draw( &s_skill_menu );
}


PRIVATE const char *Skill_MenuKey( int key )
{
	return Default_MenuKey( &s_skill_menu, key );
}

PUBLIC void M_Menu_Skill_f( void )
{
	Skill_MenuInit();
	M_PushMenu( Skill_MenuDraw, Skill_MenuKey );
}

/* EOF */
