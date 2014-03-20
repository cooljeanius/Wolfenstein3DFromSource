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
 *	wolf_mcredits.c:   Credits menu.
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

#include "../../../client/client.h"
#include "../../../video/renderer.h"

/********************************************************************
 *
 *	Credits
 *
 ********************************************************************/


PRIVATE int credits_start_time;
PRIVATE const char **credits;
PRIVATE char *creditsIndex[256];


PRIVATE const char *reduxcredits[] =
{
	"Wolfenstein 3-D Redux",
	"",
	"+Code Conductor",
	"Michael Liebscher",
	"email: johnnycanuck@users.sourceforge.net",
	"",
	"Be sure to check out",
	"+http://wolf3dredux.sourceforge.net",
	"for the latest releases and news.",
	"",
	"",
	"+This project would not be possible without",
	"+the contributions made by the following:",
	"Ahmes, Pythagoras of Samos,",
	"Euclid of Alexandria, Archimedes of Syracuse,",
	"Pierre de Fermat, Sir Isaac Newton,",
	"Jean Baptiste Joseph Fourier,",
	"Carl Friedrich Gauss, Charles Babbage,",
	"Alan Mathison Turing, David A. Huffman,",
	"Id Software, NewWolf, MAME DEV Team,",
	"zlib (Jean-loup Gailly and Mark Adler),",
	"The GIMP (Spencer Kimball and Peter Mattis),",
	"Todd C. Miller, Steven Fuller, Peter A. Friend",
	"and players like you.",
	0
};

PRIVATE void M_Credits_MenuDraw(void)
{
	int i, x, y;
	int w, h;

	if (g_version->value == SPEAROFDESTINY) {
		R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height,
					"pics/C_BACKDROPPIC.tga");
	} else {
		R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, bgcolour);
	}

	Font_SetSize(FONT0, (W16)1);
	Font_SetColour(FONT0, colourWhite);

/*
 *	Draw the credits
 */
	for ((i = 0),
		 (y = (int)((int)(viddef.height) -
					((ClientStatic.realtime - credits_start_time) / 40.0F)));
		 credits[i] && (y < (int)viddef.height);
		 (y += (int)Font_GetSize(FONT0)), ++i) {
		int stringoffset;
		int bold; /* why is this an 'int' type instead of a boolean type? */

		stringoffset = 0; /* initialize separately */
		bold = false; /* initialize separately with a boolean value */

		/* the first half of this condition is the only part that matters: */
		if ((y <= -8) && (stringoffset == 0)) {
			continue;
		}

		/* dummy condition to use value stored to 'bold': */
		if (bold == false) {
			;
		}

		/* dummy condition to use global variable 'creditsIndex[]': */
		if (creditsIndex[i] == NULL) {
			;
		}

		if (credits[i][0] == '+') {
			bold = true;
			stringoffset = 1;
		} else {
			bold = false;
			stringoffset = 0;
		}

		Font_GetMsgDimensions(FONT0, credits[i] + stringoffset, &w, &h);

		x = (((int)viddef.width - w) >> 1);

		if (bold) {
			Font_SetColour(FONT0, readhcolour);
			Font_put_line(FONT0, x, y, (credits[i] + stringoffset));
			Font_SetColour(FONT0, colourWhite );
		} else {
			Font_put_line(FONT0, x, y, (credits[i] + stringoffset));
		}
	} /* end of for-loop */

	if (y < 0) {
		credits_start_time = ClientStatic.realtime;
	}

}


PRIVATE const char *M_Credits_Key(int key)
{
	/* dummy condition to use parameter 'key': */
	if (key == 0) {
		;
	}
	M_PopMenu();

	return menu_out_sound;
}



PUBLIC void M_Menu_Credits_f( void )
{
	credits = reduxcredits;
	credits_start_time = ClientStatic.realtime;

	M_PushMenu( M_Credits_MenuDraw, M_Credits_Key );
}

/* EOF */
