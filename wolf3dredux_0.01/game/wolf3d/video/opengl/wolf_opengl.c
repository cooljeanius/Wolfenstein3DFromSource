/*
 *	Copyright (C) 2004 Michael Liebscher
 *	Copyright (C) 2000-2002 by DarkOne the Hacker
 *	Copyright (C) 1997-2001 Id Software, Inc.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 *	wolf_opengl.c:  Wolfenstein3-D OpenGL renderer.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	Portion of this code was derived from NewWolf, and was originally
 *	written by DarkOne the Hacker.
 *
 *	Portion of this code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <string.h>

#include "../../math/wolf_math.h"
#include "../../../../math/angle.h"
#include "../../../../video/video.h"
#include "../../../../video/opengl/myopengl.h"
#include "../../../../video/opengl/opengl_local.h"
#include "../../../../string/com_string.h"


#include "../../level/wolf_level.h"
#include "../../entities/wolf_player.h"
#include "../../entities/wolf_sprites.h"
#include "../../wolf_local.h"
#include "../wolf_raycast.h"

extern viddef_t	viddef;

/* width and height in 2D */
#define WIDTH_2D		640
#define HEIGHT_2D		480


float cur_x_fov, cur_y_fov; /* x & y field of view (in degrees) */
float ratio; /* viewport width/height */


/*
-----------------------------------------------------------------------------
 Function: GL_SetDefaultState

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void GL_SetDefaultState(void)
{

	pfglClearColor((GLclampf)1, (GLclampf)0, (GLclampf)0.5, (GLclampf)0.5);
	pfglCullFace(GL_FRONT);
	pfglEnable(GL_TEXTURE_2D);

	pfglEnable(GL_ALPHA_TEST);
	pfglAlphaFunc((GLenum)GL_GREATER, (GLclampf)0.666f);

	pfglDisable(GL_DEPTH_TEST);
	pfglDisable(GL_CULL_FACE);
	pfglDisable(GL_BLEND);

	pfglColor4f((GLfloat)1, (GLfloat)1, (GLfloat)1, (GLfloat)1);

	pfglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	pfglShadeModel(GL_FLAT);


	pfglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	GL_UpdateSwapInterval();
}


/*
-----------------------------------------------------------------------------
 Function: R_CheckFOV

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void R_CheckFOV(void)
{
#if 0
	/* 'vid_fov' is undeclared */
	if (!vid_fov->modified) {
		return;
	}
	vid_fov->modified=false;

	if ((vid_fov->value < 1) || (vid_fov->value > 179)) {
		Com_Printf("Wrong FOV: %f\n", vid_fov->value);
		Cvar_SetValue(vid_fov->name,
					  (((cur_x_fov >= 1) && (cur_x_fov <= 179))
					   ? cur_x_fov : DEFAULT_FOV));
	}
#endif /* 0 */
	ratio = ((float)viddef.width / (float)viddef.height); /* FIXME: move away */
	cur_x_fov = 75;
	cur_y_fov = CalcFov(cur_x_fov, (float)viddef.width, (float)viddef.height);

}

/*
-----------------------------------------------------------------------------
 Function: R_SetGL3D

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_SetGL3D(placeonplane_t viewport)
{

	R_CheckFOV();


	pfglMatrixMode(GL_PROJECTION);
	pfglLoadIdentity();
	MYgluPerspective((cur_y_fov - 2.0f), ratio, 0.2f, 64.0f);
	pfglMatrixMode(GL_MODELVIEW);
	pfglLoadIdentity();

    pfglRotatef((GLfloat)(90 - RAD2DEG(viewport.angle)), (GLfloat)0,
				(GLfloat)1, (GLfloat)0);
	pfglTranslatef((GLfloat)(-viewport.origin[0] / FLOATTILE), (GLfloat)0,
				   (GLfloat)(viewport.origin[1] / FLOATTILE));

	pfglCullFace(GL_BACK);

	pfglEnable(GL_DEPTH_TEST);
	pfglEnable(GL_CULL_FACE);
	pfglEnable(GL_BLEND);

}

/*
-----------------------------------------------------------------------------
 Function: R_DrawBox

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DrawBox( int x, int y, int w, int h, W32 color )
{
	pfglDisable( GL_TEXTURE_2D );

	pfglEnable( GL_BLEND );
	pfglBlendFunc( GL_SRC_COLOR, GL_DST_COLOR );

	pfglColor4ubv( (GLubyte *) & color );

	pfglBegin( GL_QUADS );


	pfglVertex2i( x, y );
	pfglVertex2i( x, y + h);
	pfglVertex2i( x + w, y + h );
	pfglVertex2i( x + w, y );

	pfglEnd();

	pfglColor3f( 1, 1, 1 );
	pfglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	pfglDisable( GL_BLEND );
	pfglEnable( GL_TEXTURE_2D );
}


/*
-----------------------------------------------------------------------------
 Function: R_Draw_Wall

 Parameters:

 Returns: Nothing.

 Notes:

           north (y)
          __________
          |        |
 west (x) |        | east (x)
          |________|
           south (y)

-----------------------------------------------------------------------------
*/
PUBLIC void R_Draw_Wall(float x, float y, float z1, float z2, int type, int tex)
{
	float x1, x2, y1, y2; /* y1 has a global declaration apparently... */
	texture_t *twall;

	x1 = 0; /* initialize, just in case */
	x2 = 0; /* initialize, just in case */
	y1 = 0; /* initialize, just in case */
	y2 = 0; /* initialize, just in case */

	switch (type) {
	/* X wall */
		case dir4_east:
			x1 = x2 = (x + 1);
			y1 = (-1 - y);
			y2 = -y;
			break;

		case dir4_west:
			x1 = x2 = x;
			y1 = -y;
			y2 = (-1 - y);
			break;

	/* Y wall */
		case dir4_north:
			y1 = y2 = (-y - 1);
			x1 = x;
			x2 = (x + 1);
			break;

		case dir4_south:
			y1 = y2 = -y;
			x1 = (x + 1);
			x2 = x;
			break;
	}

    twall = TM_FindTexture_Wall((W32)tex);

    R_Bind((int)twall->texnum);


	pfglBegin(GL_QUADS);

	pfglTexCoord2f((GLfloat)1.0, (GLfloat)0.0);
	pfglVertex3f((GLfloat)x1, (GLfloat)z2, (GLfloat)y1);
	pfglTexCoord2f((GLfloat)0.0, (GLfloat)0.0);
	pfglVertex3f((GLfloat)x2, (GLfloat)z2, (GLfloat)y2);
	pfglTexCoord2f((GLfloat)0.0, (GLfloat)1.0);
	pfglVertex3f((GLfloat)x2, (GLfloat)z1, (GLfloat)y2);
	pfglTexCoord2f((GLfloat)1.0, (GLfloat)1.0);
	pfglVertex3f((GLfloat)x1, (GLfloat)z1, (GLfloat)y1);

	pfglEnd();
}

/*
-----------------------------------------------------------------------------
 Function: R_Draw_Door

 Parameters: Yikes, there are a lot of them...

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_Draw_Door(int x, int y, float z1, float z2, _boolean vertical,
						_boolean backside, int tex, int amount)
{
	float x1, x2, y1, y2, amt;  /* y1 has a global declaration apparently... */
	texture_t *twall;


	if (amount == DOOR_FULLOPEN) {
		return;
	}

	amt = ((float)amount / DOOR_FULLOPEN);


	if (vertical) {
		x1 = x2 = ((float)x + 0.5f);
		y1 = -((float)y - amt);
		y2 = -((float)y - amt); /* -1 */
		if (backside) {
			y1 -= 1;
		} else {
			y2 -= 1;
		}
	} else {
		y1 = y2 = (-(float)y - 0.5f);
		x1 = ((float)x + amt); /* +1 */
		x2 = ((float)x + amt);
		if (backside) {
			x2 += 1;
		} else {
			x1 += 1;
		}
	}

    twall = TM_FindTexture_Wall((W32)tex);

    R_Bind((int)twall->texnum);


	pfglBegin(GL_QUADS);

	pfglTexCoord2f((GLfloat)(backside ? 0.0f : 1.0f), (GLfloat)0.0);
	pfglVertex3f((GLfloat)x1, (GLfloat)z2, (GLfloat)y1);
	pfglTexCoord2f((GLfloat)(backside ? 1.0f : 0.0f), (GLfloat)0.0);
	pfglVertex3f((GLfloat)x2, (GLfloat)z2, (GLfloat)y2);
	pfglTexCoord2f((GLfloat)(backside ? 1.0f : 0.0f), (GLfloat)1.0);
	pfglVertex3f((GLfloat)x2, (GLfloat)z1, (GLfloat)y2);
	pfglTexCoord2f((GLfloat)(backside ? 0.0f : 1.0f), (GLfloat)1.0);
	pfglVertex3f((GLfloat)x1, (GLfloat)z1, (GLfloat)y1);

	pfglEnd();
}


/*
-----------------------------------------------------------------------------
 Function: R_DrawSprites -Draws all visible sprites.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DrawSprites(void)
{
	float sina, cosa;
	float Ex, Ey, Dx, Dy;
	int n_sprt, n;
    float ang;
	texture_t *twall;


/* build visible sprites list */
	n_sprt = Sprite_CreateVisList();
	if (! n_sprt) {
		return; /* nothing to draw */
	}

    /* prepare values for billboarding */
    ang = (float)angle_normalize((float)(Player.position.angle + (M_PI / 2)));
	/* FIXME: take viewport */

    sina = (float)(0.5 * sin(ang));
	cosa = (float)(0.5 * cos(ang));


	for ((n = 0); (n < n_sprt); ++n ) {
		if (vislist[n].dist < (MINDIST / 2)) {
			continue; /* little hack to save speed & z-buffer */
		}

        twall = TM_FindTexture_Sprite((W32)vislist[n].tex);

		R_Bind((int)twall->texnum);

		pfglBegin(GL_QUADS);

		Ex = Dx = (vislist[n].x / FLOATTILE);
		Ey = Dy = (vislist[n].y / FLOATTILE);
		Ex += cosa; Ey += sina;
		Dx -= cosa; Dy -= sina;

		pfglTexCoord2f((GLfloat)0.0, (GLfloat)0.0);
		pfglVertex3f((GLfloat)Ex, (GLfloat)UPPERZCOORD, (GLfloat)-Ey);
		pfglTexCoord2f((GLfloat)0.0, (GLfloat)1.0);
		pfglVertex3f((GLfloat)Ex, (GLfloat)LOWERZCOORD, (GLfloat)-Ey);
		pfglTexCoord2f((GLfloat)1.0, (GLfloat)1.0);
		pfglVertex3f((GLfloat)Dx, (GLfloat)LOWERZCOORD, (GLfloat)-Dy);
		pfglTexCoord2f((GLfloat)1.0, (GLfloat)0.0);
		pfglVertex3f((GLfloat)Dx, (GLfloat)UPPERZCOORD, (GLfloat)-Dy);

		pfglEnd();
	}

}

/*
-----------------------------------------------------------------------------
 Function: R_DrawWeapon

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DrawWeapon(void)
{
	char name[32];
	texture_t *tex;
	static int w;
	static int h;
	static int scale;
	int x;
	int y;

	w = 128;
	h = 128;
	scale = 2;
	x = ((viddef.width - (unsigned int)(128 * scale)) >> 1);
	y = (int)((viddef.height - (unsigned int)(128 * scale)) - 79);

	my_snprintf(name, sizeof(name), "%s/%d.tga", spritelocation,
				(Player.weapon * 5 + Player.weaponframe + SPR_KNIFEREADY));

	tex = TM_FindTexture(name, TT_Pic);


	R_Bind((int)tex->texnum);


	pfglAlphaFunc(GL_GREATER, 0.3f);

	pfglEnable(GL_BLEND);

	pfglBegin(GL_QUADS);

	pfglTexCoord2f(0.0f, 0.0f); pfglVertex2i((x), (y));
	pfglTexCoord2f(1.0f, 0.0f); pfglVertex2i((x + (w * scale)), (y));
	pfglTexCoord2f(1.0f, 1.0f); pfglVertex2i((x + (w * scale)), (y + (h * scale)));
	pfglTexCoord2f(0.0f, 1.0f); pfglVertex2i((x), (y + (h * scale)));

	pfglEnd();

	pfglDisable(GL_BLEND);

	pfglAlphaFunc(GL_GREATER, 0.666f);

}


/*
-----------------------------------------------------------------------------
 Function: R_DrawNumber

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DrawNumber(int x, int y, int number)
{
	texture_t *tex;
	int	col;
	float fcol;
	static float w = 0.1f;
	int i;
	char string[20];
	W32 length;


	my_snprintf(string, sizeof(string), "%d", number);
	length = strlen(string);

	tex = TM_FindTexture("pics/N_NUMPIC.tga", TT_Pic);


	pfglEnable(GL_TEXTURE_2D);

	R_Bind((int)tex->texnum);

	pfglBegin(GL_QUADS);

	for ((i = (int)(length - 1)); (i >= 0); --i) {
		col = (string[i] - 48);

		fcol = (col * w);

		pfglTexCoord2f((fcol),	0);		pfglVertex2i(x, y);
		pfglTexCoord2f((fcol + w), 0);	pfglVertex2i((x + 18), y);
		pfglTexCoord2f((fcol + w), 1);	pfglVertex2i((x + 18), (y + 32));
		pfglTexCoord2f((fcol),	1);		pfglVertex2i(x, (y + 32));

		x -= 18;
	}

	pfglEnd();

}


W8 wfont[ ] = {
32, 15, 32, 32, 32, 32, 32, 12, 32, 32, 32, 32, 32, 32, 32, 32,
32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 16, 32, 32, 32, 32, 32,
32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
};

/*
-----------------------------------------------------------------------------
 Function: R_put_line

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_put_line(int x, int y, const char *string)
{
	texture_t *tex;
	int mx = x;
	int num;
	float frow, fcol;
	static float h = 0.25f;	/* (32 / 128.0f); */
	static float w = 0.0625f; /* (32 / 512.0f); */


	tex = TM_FindTexture("pics/L_FONTPIC.tga", TT_Pic);


	R_Bind((int)tex->texnum);

	pfglBegin(GL_QUADS);

	while (*string) {
		if (*string == '\n') {
			mx = x;
			y += 32;
			++string;
			continue;
		}

		num = *string;

		num &= 255;

		if ((num & 127) == 32) {
			mx += 32;
			++string;
			continue;		/* space */
		}


		frow = (((num >> 4) - 2) * h);
		fcol = ((num & 15) * w);


		pfglTexCoord2f((fcol), (frow));			pfglVertex2i((mx), (y));
		pfglTexCoord2f((fcol + w), (frow));		pfglVertex2i((mx + 32), (y));
		pfglTexCoord2f((fcol + w), (frow + h));	pfglVertex2i((mx + 32), (y + 32));
		pfglTexCoord2f((fcol), (frow + h));		pfglVertex2i((mx), (y + 32));


		mx += wfont[((num & 127) - 32)];
		++string;
	}

	pfglEnd();
}

/* EOF */
