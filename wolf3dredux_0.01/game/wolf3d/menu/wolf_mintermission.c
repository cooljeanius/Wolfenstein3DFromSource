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
 *	wolf_mintermission.c:   Intermission menu.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include "wolf_menu.h"


#include "../wolf_local.h"
#include "../entities/wolf_player.h"
#include "../script/episode.h"

#include "../../../client/client.h"
#include "../../../string/com_string.h"
#include "../../../video/renderer.h"
#include "../../../sound/sound.h"

#ifndef strstr
# include <string.h>
#endif /* !strstr */

/********************************************************************
 *
 *	Intermission
 *
 ********************************************************************/


PRIVATE char *nextlevelname; /* unused (?) */

/* */
PRIVATE void M_DrawInterBJ(int x, int y, int f)
{
	char	guypic[32];
	static _boolean cached;

	if (! cached) {
		W8 i;

		for ((i = 0); (i < 2); ++i) {
			my_snprintf(guypic, sizeof(guypic), "pics/L_GUY%dPIC.tga", i);

			(void)TM_FindTexture(guypic, TT_Pic);
		}

		cached = true;
	}

	my_snprintf(guypic, sizeof(guypic), "pics/L_GUY%dPIC.tga", f);

	R_Draw_Pic(x, y, guypic);

}

#define PAR_AMOUNT	500

extern void R_DrawHUD(void);
extern W32 gameTimeCount;
PRIVATE W32 leveltime;
PRIVATE _boolean bgive_bonus = false;

/* */
PRIVATE void M_Secret_Draw(void)
{
	char string[32];

	R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, interbkgnd);

	M_DrawInterBJ(0, 38, ((int)(ClientStatic.realtime / 500) % 2));

	if (g_version->value == SPEAROFDESTINY) {
		switch (levelstate.floornum) {
			case 4:
				my_snprintf(string, sizeof(string), "TRANS\nGROSSE\nDEFEATED!");
				break;

			case 9:
				my_snprintf(string, sizeof(string),
							"BARNACLE\nWILHELM\nDEFEATED!");
				break;

			case 15:
				my_snprintf(string, sizeof(string), "UBERMUTANT\nDEFEATED!");
				break;

			case 17:
				my_snprintf(string, sizeof(string), "DEATH\nKNIGHT\nDEFEATED!");
				break;

			case 18:
				my_snprintf(string, sizeof(string),
							"SECRET TUNNEL\nAREA\nCOMPLETED!");
				break;

			case 19:
				my_snprintf(string, sizeof(string),
							"SECRET CASTLE\nAREA\nCOMPLETED!");
				break;
		}

		R_put_line(240, 56, string);
	} else {
		my_snprintf(string, sizeof(string), "SECRET FLOOR\n COMPLETED!");
		R_put_line(240, 56, string);
	}

	my_snprintf(string, sizeof(string), "15000 BONUS!");
	R_put_line(128, 256, string);

	R_DrawHUD();
}

PRIVATE W16 ElevatorBackTo[] = { 1, 1, 7, 3, 5, 3 }; /* unused? */

/* This is for Spear of Destiny */
#define FROMSECRET1		3
#define FROMSECRET2		11

/* */
PRIVATE char *M_Secret_Key(int key)
{
	char szTextMsg[128];
	char *mapfilename;

	PL_NextLevel(&Player);


	M_ForceMenuOff();

	/* dummy condition to use parameter 'key': */
	if (key == 0) {
		;
	}

	if (g_version->value == SPEAROFDESTINY) {
		int mapon = 0;

		switch (levelstate.floornum) {
			case 18:
				mapon = (FROMSECRET1 + 1);
				break;

			case 19:
				mapon = (FROMSECRET2 + 1);
				break;

			default:
				mapon = (int)(levelstate.floornum + 1);
		}
		my_snprintf(szTextMsg, sizeof(szTextMsg),
					"loading ; map s%.2d.map\n", mapon);
	} else {
		mapfilename = episode_getNextMapName((W32 *)&floornumber);

		if (! mapfilename) {
			Com_Printf("Unable to get next map file name\n");

			ClientStatic.key_dest = key_console;

			return NULL;
		}

		my_snprintf(szTextMsg, sizeof(szTextMsg), "loading ; map %s\n",
					mapfilename);
	}

	Player.playstate = ex_playing;

	Cbuf_AddText(szTextMsg);

	return NULL;
}

/* */
PRIVATE void M_Intermission_Draw(void)
{
	char string[32];
	W32 ratio;
	static W32 bonus = 0;
	W32 timeleft = 0;
	W32 min, sec;


	R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, interbkgnd);

	M_DrawInterBJ(0, 38, ((int)(ClientStatic.realtime / 500) % 2));


	sec = (leveltime / 70);

	if (sec > (99 * 60)) { /* 99 minutes max */
	   sec = (99 * 60);
	}

	min = (sec / 60);
	sec %= 60;

	if (! bgive_bonus) {
		/* TODO: Fix bonus points being given when this screen is redrawn. */
		bgive_bonus = true;

		if (leveltime < (levelstate.fpartime * 4200)) {
			timeleft = (W32)(((levelstate.fpartime * 4200) / 70) - sec);
		}

		bonus = (timeleft * PAR_AMOUNT);

		PL_GivePoints(&Player, bonus);
	}


	my_snprintf(string, sizeof(string), "FLOOR %d\nCOMPLETED",
				(levelstate.floornum + 1));
	R_put_line(240, 38, string);

	my_snprintf( string, sizeof( string ), "BONUS %d", bonus );
	R_put_line( 240, 120, string );

	my_snprintf( string, sizeof( string ), "TIME %.2d:%.2d", min, sec );
	R_put_line( 240, 180, string );

	my_snprintf( string, sizeof( string ), "PAR  %s", levelstate.spartime );
	R_put_line( 240, 216, string );

	/*
	 * Kill Ratio
	 */

	if( levelstate.total_monsters > 0 ) {
		ratio = ((levelstate.killed_monsters * 100) / levelstate.total_monsters);
	} else {
		ratio = 0;
	}

	my_snprintf( string, sizeof( string ), "    KILL RATIO %%%d", ratio );
	R_put_line( 20, 268, string );

	/*
	 * Secret Ratio
	 */

	if( levelstate.total_secrets > 0 ) {
		ratio = (levelstate.found_secrets * 100) / levelstate.total_secrets;
	} else {
		ratio = 0;
	}

	my_snprintf( string, sizeof( string ), "  SECRET RATIO %%%d", ratio );
	R_put_line( 20, 308, string );

	/*
	 * Treasure Ratio
	 */

	if( levelstate.total_treasure > 0 ) {
		ratio = (levelstate.found_treasure * 100) / levelstate.total_treasure;
	} else {
		ratio = 0;
	}

	my_snprintf( string, sizeof( string ), "TREASURE RATIO %%%d", ratio );
	R_put_line( 20, 348, string );


	R_DrawHUD();
}

/* */
PRIVATE char *M_Intermission_Key(int key)
{
	char szTextMsg[128];

	PL_NextLevel(&Player);

	/* dummy condition to use parameter 'key': */
	if (key == 0) {
		;
	}

	M_ForceMenuOff();

	if (g_version->value == SPEAROFDESTINY) {
		if (Player.playstate == ex_secretlevel) {
			int mapon = 0;

			switch (levelstate.floornum) {
				case FROMSECRET1:
					mapon = 18;
					break;

				case FROMSECRET2:
					mapon = 19;
					break;
			}

			my_snprintf(szTextMsg, sizeof(szTextMsg),
						"loading ; map s%.2d.map\n", mapon);
		} else {
			my_snprintf(szTextMsg, sizeof(szTextMsg),
						"loading ; map s%.2d.map\n", (levelstate.floornum + 1));
		}
	} else {
		char *mapfilename;

		if (Player.playstate == ex_secretlevel) {
			mapfilename = episode_getNextSecretMapName((W32 *)&floornumber);

			if (! mapfilename) {
				Com_Printf("Unable to get next secret map file name\n");

				ClientStatic.key_dest = key_console;

				return NULL;
			}

			my_snprintf(szTextMsg, sizeof(szTextMsg),
						"loading ; map %s\n", mapfilename);
		} else {
			mapfilename = episode_getNextMapName((W32 *)&floornumber);

			if (! mapfilename) {
				Com_Printf("Unable to get next map file name\n");

				ClientStatic.key_dest = key_console;

				return NULL;
			}

			my_snprintf(szTextMsg, sizeof(szTextMsg),
						"loading ; map %s\n", mapfilename);
		}
	}

	Player.playstate = ex_playing;

	Cbuf_AddText(szTextMsg);

	return NULL;
}


/*
 *	Victory
 */

PRIVATE W8 nvictory = 1;

/* */
PRIVATE void M_Victory_Draw( void )
{
	char string[ 32 ];
	W32 ratio;
	W32 min, sec;

	R_Draw_Fill( 0, 0, (int)viddef.width, (int)viddef.height, interbkgnd );

	R_Draw_Pic( 32, 12, "pics/L_BJWINSPIC.tga" );


	my_snprintf( string, sizeof( string ), "YOU WIN!" );
	R_put_line( 240, 38, string );

	sec = LevelRatios.time / 70;

	/* 99 minutes max: */
	if (sec > (99 * 60)) {
	   sec = (99 * 60);
	}

	min = (sec / 60);
	sec %= 60;

	my_snprintf( string, sizeof( string ), "TOTAL TIME %.2d:%.2d", min, sec );
	R_put_line( 64, 194, string );

	my_snprintf( string, sizeof( string ), "AVERAGES", levelstate.spartime );
	R_put_line( 192, 230, string );

	/*
	 * Kill Ratio
	 */

	if( LevelRatios.total_monsters != 0 ) {
		ratio = (LevelRatios.killed_monsters * 100) / LevelRatios.total_monsters;
	} else {
		ratio = 0;
	}

	my_snprintf( string, sizeof( string ), "KILL     %%%d", ratio );
	R_put_line( 20, 278, string );

	/*
	 * Secret Ratio
	 */

	if( LevelRatios.total_secrets != 0 ) {
		ratio = (LevelRatios.found_secrets * 100) / LevelRatios.total_secrets;
	} else {
		ratio = 0;
	}

	my_snprintf( string, sizeof( string ), "SECRET   %%%d", ratio );
	R_put_line( 20, 318, string );

	/*
	 * Treasure Ratio
	 */

	if( LevelRatios.total_treasure != 0 ) {
		ratio = (LevelRatios.found_treasure * 100) / LevelRatios.total_treasure;
	} else {
		ratio = 0;
	}

	my_snprintf( string, sizeof( string ), "TREASURE %%%d", ratio );
	R_put_line( 20, 358, string );


	R_DrawHUD();
}

PRIVATE const char victorytextPageOnePartOne[6][1024] =
{
	"CONGRATULATIONS!\n\nYou run out of the castle and hook up with the Underground. They inform you that the rumors were true: some hideous human experiments were seen around Castle Hollehammer.",
	"CONGRATULATIONS!\n\nYou stand over Schabbs' fat, evil, swollen putrid body, glad your mission is finally over. All his journals and equipment will be destroyed. Humanity is safe from his hordes of",
	"CONGRATULATIONS!\n\nThe absolute incarnation of evil, Adolf Hitler, lies at your feet in a pool of his own blood. His wrinkled, crimson-splattered visage still strains, a jagged- toothed rictus trying to",
	"MAD OTTO GIFTMACHER IS DEAD!\n\nThe twisted scientist behind the chemical war lies at your feet, but the fruits of his labor grow elsewhere! The first wave of chemical war is",
	"THE PLANS ARE IN YOUR HANDS!\n\nGretel Grosse the giantess guard has fallen. Hope her brother, Hans, doesn't get mad about this....",
	"CONGRATULATIONS!\n\nThe General gasps his last breath, and the free world is safe from the terrifying Nazi chemical war. You return to Allied Headquarters, a Medal of Honor waiting for"
};

PRIVATE const char victorytextPageOnePartTwo[6][1024] =
{
	"So Operation Eisenfaust is real!\n\nYou must journey there and terminate the maniacal Dr. Schabbs before his undead army marches against humanity!",
	"hideous mutants.\n\nYet the Nazi atrocities continue: thousands march into death camps even as the Nazi war machine falls to its knees. There is only one way to stop the madness. . . .",
	"cry out. Insane even in death. Your lips pinched in bitter victory, you kick his head off his remains and spit on his corpse.\n\nSieg heil . . . huh. Sieg hell.",
	"already underway. In the heavily guarded fortress of Erlangen are the plans for the upcoming Giftkrieg (or Poison War). Find them and you'll know where to find General Fettgesicht, leader of the deadly assault.",
	"Now rush to the military installation at Offenbach and stop the horrible attack before thousands die under the deadly, burning clouds of chemical war. Only you can do it, B.J.",
	"you.\n\nAllied Command informs you of some nefarious activities around Castle Hollehammer. Something about some grey-skinned berserk soldiers . . . ."
};

PRIVATE const char victorytextPageTwo[6][1024] =
{
	"MORE WOLFENSTEIN\n\nAnd in episode three, Hitler hides in his titanic bunker as the Third Reich crumbles about him. It is your job to assassinate him, ending his mad reign.\n\nAnd if you like Wolfenstein, you'll love the prequel trilogy of Nocturnal Missions! Thirty more action-packed, super-challenging levels!",
	"MISSION: TERMINATE HITLER\n\nIn episode three, Hitler hides in his titanic bunker as the Third Reich crumbles about him. It is your job to assassinate him, ending his mad reign. You find he has escaped to the Reichstag, and there you must confront him.\n\nAnd if you like Wolfenstein, you'll love the prequel trilogy of \"Nocturnal Missions!\" Thirty more action-packed, super-challenging levels!",
	"BUT THE ADVENTURE IS JUST BEGINNING!\n\nAnd if you like Wolfenstein, you'll love the prequel trilogy of \"Nocturnal Missions!\" Thirty more action-packed, super-challenging levels!\n\nB.J. battles the Nazis as they plan large-scale chemical warfare. Fight Otto Giftmacher, Gretel Grosse, and General Fettgesicht!",
	"THE NEXT ADVENTURE AWAITS!\n\nSo don't wait . . . start the next adventure and find those plans!",
	"THE END IS NEAR!\n\nGo get General Fettgeischt before he begins the mad plans of pain and destruction!",
	"YOU DID IT!\n\nYou have finished the sixth Wolfenstein episode! You are truly one of the great heroes! The world cheers your name! You get your picture taken with the president! People name their babies after you! You marry a movie star! Yes! You are so cool!"
};


/* */
PRIVATE void M_Victory_Draw_PageOne(void)
{
	SW32 w, h;

	R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height, "walls/000.tga");

	M_DrawWindow(16, 16, 608, 408, colourWhite, colourBlack, colourBlack);

	TM_GetTextureSize(&w, &h, "pics/PLAQUE_PAGE.tga");
	R_Draw_Pic((int)(((viddef.width >> 1) - (unsigned long)w) >> 1),
			   (int)(viddef.height - 48),
			   "pics/PLAQUE_PAGE.tga");

	TM_GetTextureSize(&w, &h, "pics/PLAQUE_BLANK.tga");
	R_Draw_Pic((int)(viddef.width - (unsigned long)w - (((viddef.width >> 1) - (unsigned long)w) >> 1)),
			   (int)(viddef.height - 48), "pics/PLAQUE_BLANK.tga");

	R_Draw_Pic(32, 32, "pics/H_BLAZEPIC.tga");

	Font_SetColour(FONT0, colourBlack);
	Font_SetSize(FONT0, (W16)2);

	Font_put_line(FONT0,
				  (int)((viddef.width - (unsigned long)w - (((viddef.width >> 1) - (unsigned long)w) >> 1)) + 32),
				  (int)((viddef.height - 48) + 6), "pg 1 of 2");

	/* (240 + 32 + 10) = 282 */
	Font_put_paragraph(FONT0, (short)(240 + 32 + 10), (short)32,
					   victorytextPageOnePartOne[(W32)episode->value], 0, 350);

	/* (32 + 4) = 38 */		/* (160 + 16 + 34) = 210 */
	Font_put_paragraph(FONT0, (short)(32 + 4), (short)(160 + 16 + 34),
					   victorytextPageOnePartTwo[(W32)episode->value], 0, 600);
}

/* */
PRIVATE void M_Victory_Draw_PageTwo(void)
{
	SW32 w, h;

	R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height, "walls/000.tga");

	M_DrawWindow(16, 16, 608, 408, colourWhite, colourBlack, colourBlack);

	TM_GetTextureSize(&w, &h, "pics/PLAQUE_PAGE.tga");
	R_Draw_Pic((int)(((viddef.width >> 1) - (unsigned long)w) >> 1),
			   (int)(viddef.height - 48), "pics/PLAQUE_PAGE.tga");

	TM_GetTextureSize(&w, &h, "pics/PLAQUE_BLANK.tga");
	R_Draw_Pic((int)(viddef.width - (unsigned long)w - (((viddef.width >> 1) - (unsigned long)w) >> 1)),
			   (int)(viddef.height - 48), "pics/PLAQUE_BLANK.tga");

	Font_SetColour(FONT0, colourBlack);
	Font_SetSize(FONT0, (W16)2);

	Font_put_line(FONT0,
				  (int)((viddef.width - (unsigned long)w - (((viddef.width >> 1) - (unsigned long)w) >> 1)) + 32),
				  (int)((viddef.height - 48) + 6), "pg 2 of 2");

	Font_put_paragraph(FONT0, (short)(32 + 4), (short)34,
					   victorytextPageTwo[(W32)episode->value], 0, 600);
}


PRIVATE W16 page = 0;

/* */
PRIVATE void M_Victory_Text_Draw(void)
{
	if (page == 0) {
		M_Victory_Draw_PageOne();
	} else {
		M_Victory_Draw_PageTwo();
	}
}

/* */
PRIVATE char *M_Victory_Key(int key)
{
	if (nvictory) {
		M_PopMenu();
		/* gosh, what an ugly cast: */
		M_PushMenu(M_Victory_Text_Draw, (const char *(*)(int))M_Victory_Key);
		nvictory = 0;
		page = 0;
		ClientStatic.state = ca_disconnected;

		return NULL;
	}

	switch (key) {
		case K_ESCAPE:
			nvictory = 1;
			M_ForceMenuOff();
			M_Menu_Main_f();
			break;

		case K_KP_LEFTARROW:
		case K_LEFTARROW:
			if (page == 1) {
				page = 0;
			}
			break;

		case K_KP_RIGHTARROW:
		case K_RIGHTARROW:
			if (page == 0) {
				page = 1;
			}
			break;

	}

	return NULL;
}


/*
 *	Spear Victory
 */

#define NUM_VICTORY_SLIDES	11

colour3_t viewcolour = { 0, 64, 64 };

typedef struct
{
	void	(*draw) (void);
	W32 delay_time;

} SpearVictoryLayer_t;

PRIVATE SpearVictoryLayer_t sodvl[12];
PRIVATE W16 victory_slide = 0;
PRIVATE W32 victory_basetime;


PRIVATE W32 collapse_basetime;
PRIVATE W8 collapse_slide = 1;

/* */
PRIVATE void SOD_Victory_BJCOLLAPSE_Draw(void)
{
	W32 w, h;
	static char name[32];


	R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, viewcolour);

	if (((unsigned long)ClientStatic.realtime >= (collapse_basetime + 2000)) &&
		(collapse_slide != 4)) {
		collapse_basetime = (W32)ClientStatic.realtime;
		++collapse_slide;
	}

	my_snprintf(name, sizeof(name), "pics/BJCOLLAPSE%dPIC.tga", collapse_slide);


	TM_GetTextureSize((SW32 *)&w, (SW32 *)&h, name);
	R_Draw_Pic((int)((viddef.width - w) >> 1), (int)((viddef.height - h) >> 1),
			   name);
}


#define STR_ENDGAME1 "We owe you a great debt, Mr. Blazkowicz.\nYou have served your country well."
#define STR_ENDGAME2 "With the spear gone, the Allies will finally\nbe able to destroy Hitler..."

/* */
PRIVATE void SOD_EndScreen2_Draw(void)
{
	W32 w, h;

	R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, viewcolour);

	TM_GetTextureSize((SW32 *)&w, (SW32 *)&h, "pics/ENDSCREEN3PIC.tga");
	R_Draw_Pic((int)((viddef.width - w) >> 1), (int)((viddef.height - h) >> 1),
			   "pics/ENDSCREEN3PIC.tga" );

	Font_SetSize(FONT0, (W16)2);
	Font_SetColour(FONT0, colourWhite);

	if (victory_slide == 3) {
		Font_GetMsgDimensions(FONT0, STR_ENDGAME1, (int *)&w, (int *)&h);
		Font_put_line(FONT0, (int)((viddef.width - w) >> 1),
					  (int)(viddef.height - h), STR_ENDGAME1);
	} else {
		Font_GetMsgDimensions(FONT0, STR_ENDGAME2, (int *)&w, (int *)&h);
		Font_put_line(FONT0, (int)((viddef.width - w) >> 1),
					  (int)(viddef.height - h), STR_ENDGAME2 );
	}

}


/* */
PRIVATE void SOD_EndScreen_Draw(void)
{
	W32 w, h;
	static char texname[64];

	R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, viewcolour);


	switch (victory_slide) {
		case 2:
			R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height,
						"pics/C_BACKDROPPIC.tga");
			my_strlcpy(texname, "pics/ENDSCREEN11PIC.tga", sizeof(texname));
			break;

		case 5:
			my_strlcpy(texname, "pics/ENDSCREEN4PIC.tga", sizeof(texname));
			break;

		case 6:
			my_strlcpy(texname, "pics/ENDSCREEN5PIC.tga", sizeof(texname));
			break;

		case 7:
			my_strlcpy(texname, "pics/ENDSCREEN6PIC.tga", sizeof(texname));
			break;

		case 8:
			my_strlcpy(texname, "pics/ENDSCREEN7PIC.tga", sizeof(texname));
			break;

		case 9:
			my_strlcpy(texname, "pics/ENDSCREEN8PIC.tga", sizeof(texname));
			break;

		case 10:
			my_strlcpy(texname, "pics/ENDSCREEN9PIC.tga", sizeof(texname));
			break;

		case 11:
			R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height,
						"pics/C_BACKDROPPIC.tga");
			my_strlcpy(texname, "pics/ENDSCREEN12PIC.tga", sizeof(texname));
			break;

		default:
			my_strlcpy(texname, "", sizeof(texname));
			break;

	}


	TM_GetTextureSize((SW32 *)&w, (SW32 *)&h, texname);
	R_Draw_Pic((int)((viddef.width - w) >> 1), (int)((viddef.height - h) >> 1),
			   texname);
}

/* */
PRIVATE void M_SODVictory_Draw(void)
{
	if (victory_slide > NUM_VICTORY_SLIDES) {
		M_ForceMenuOff();
		M_Menu_Main_f();

		return;
	}

	sodvl[victory_slide].draw();

	if (sodvl[victory_slide].delay_time) {
		if ((unsigned long)ClientStatic.realtime >=
			(victory_basetime + sodvl[victory_slide].delay_time)) {
			victory_basetime = (W32)ClientStatic.realtime;

			++victory_slide;
			if (victory_slide == 1) {
				Sound_StopBGTrack();

				Sound_StartBGTrack("music/ENDLEVEL.ogg", "music/ENDLEVEL.ogg");
			}
		}
	}
}

/* */
PRIVATE const char *M_SODVictory_Key(int key)
{
	victory_basetime = (W32)ClientStatic.realtime;
	++victory_slide;

	/* dummy condition to use parameter 'key': */
	if (key == 0) {
		;
	}

	if (victory_slide == 1) {
		Sound_StopBGTrack();

		Sound_StartBGTrack("music/ENDLEVEL.ogg", "music/ENDLEVEL.ogg");
	}

	if (victory_slide > NUM_VICTORY_SLIDES) {
		M_ForceMenuOff();
		M_Menu_Main_f();
	}

	return NULL;
}

/*
 *	End of Spear Victory
 */

/* */
PUBLIC void M_Intermission_f(void)
{
	Sound_StopAllSounds();
	Sound_StopBGTrack();

	Sound_StartBGTrack("music/ENDLEVEL.ogg", "music/ENDLEVEL.ogg");

	bgive_bonus = false;

	leveltime = (W32)levelstate.time;


	LevelRatios.time += leveltime;

	LevelRatios.found_secrets += levelstate.found_secrets;
	LevelRatios.total_secrets += levelstate.total_secrets;

	LevelRatios.killed_monsters += levelstate.killed_monsters;
	LevelRatios.total_monsters += levelstate.total_monsters;

	LevelRatios.found_treasure += levelstate.found_treasure;
	LevelRatios.total_treasure += levelstate.total_treasure;

	if (g_version->value == SPEAROFDESTINY) {
		switch (levelstate.floornum) {
			case 17:
				collapse_slide = 1;
				victory_slide = 0;

				sodvl[0].draw = SOD_Victory_BJCOLLAPSE_Draw;
				sodvl[0].delay_time = 12000;

				sodvl[1].draw = M_Victory_Draw;
				sodvl[1].delay_time = 0;

				sodvl[2].draw = SOD_EndScreen_Draw;
				sodvl[2].delay_time = 0;

				sodvl[3].draw = SOD_EndScreen2_Draw;
				sodvl[3].delay_time = 9000;

				sodvl[4].draw = SOD_EndScreen2_Draw;
				sodvl[4].delay_time = 9000;

				sodvl[5].draw = SOD_EndScreen_Draw;
				sodvl[5].delay_time = 0;

				sodvl[5].draw = SOD_EndScreen_Draw;
				sodvl[5].delay_time = 0;

				sodvl[6].draw = SOD_EndScreen_Draw;
				sodvl[6].delay_time = 0;

				sodvl[7].draw = SOD_EndScreen_Draw;
				sodvl[7].delay_time = 0;

				sodvl[8].draw = SOD_EndScreen_Draw;
				sodvl[8].delay_time = 0;

				sodvl[9].draw = SOD_EndScreen_Draw;
				sodvl[9].delay_time = 0;

				sodvl[10].draw = SOD_EndScreen_Draw;
				sodvl[10].delay_time = 0;

				sodvl[11].draw = SOD_EndScreen_Draw;
				sodvl[11].delay_time = 0;

				collapse_basetime = victory_basetime = (W32)ClientStatic.realtime;

				Sound_StartBGTrack("music/XTHEEND.ogg", "music/XTHEEND.ogg");

				M_PushMenu(M_SODVictory_Draw, M_SODVictory_Key);
				return;

			case 4:
			case 9:
			case 15:
			case 18:
			case 19:
				PL_GivePoints(&Player, 15000);
				M_PushMenu(M_Secret_Draw, (const char *(*)(int))M_Secret_Key);
				return;
		}

		M_PushMenu(M_Intermission_Draw,
				   (const char *(*)(int))M_Intermission_Key);
	}

    if ((strstr(levelstate.level_name, "Boss") != NULL) &&
		(g_version->value == WOLFENSTEINWL6)) {
		Sound_StartBGTrack("music/URAHERO.ogg", "music/URAHERO.ogg");

		M_PushMenu(M_Victory_Draw, (const char *(*)(int))M_Victory_Key);
	} else if ((levelstate.floornum == 9) &&
			   (g_version->value == WOLFENSTEINWL6)) {
		PL_GivePoints(&Player, (W32)15000);
		M_PushMenu(M_Secret_Draw, (const char *(*)(int))M_Secret_Key);
	} else {
		M_PushMenu(M_Intermission_Draw, (const char *(*)(int))M_Intermission_Key);
	}

}

/* EOF */
