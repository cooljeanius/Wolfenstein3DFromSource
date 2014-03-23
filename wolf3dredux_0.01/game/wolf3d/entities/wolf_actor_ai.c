/*
 *	Copyright (C) 2004 Michael Liebscher
 *	Copyright (C) 2001 by DarkOne the Hacker
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
 *	wolf_actor_ai.c: Wolfenstein3-D artificial intelligence.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from NewWolf, and was originally
 *	written by DarkOne the Hacker.
 *
 */


#include <stdio.h>


#include "wolf_actor_ai.h"
#include "wolf_actors.h"
#include "wolf_player.h"
#include "wolf_powerups.h"
#include "wolf_sprites.h"
#include "../wolf_local.h"
#include "../level/wolf_level.h"

#include "../../../math/random_number.h"
#include "../../../sound/sound.h"

const char dsounds[7][32] =
{
	"sfx/025.wav",
	"sfx/026.wav",
	"sfx/086.wav",
	"sfx/088.wav",
	"sfx/105.wav",
	"sfx/107.wav",
	"sfx/109.wav"
};

const char dsodsounds[7][32] =
{
	"sfx/021.wav",
	"sfx/022.wav",
	"sfx/052.wav",
	"sfx/054.wav",
	"sfx/057.wav",
	"sfx/059.wav",
	"sfx/061.wav"
};

/*
-----------------------------------------------------------------------------
 Function: A_DeathScream() -Do a death scream sound depending on actor type.

 Parameters: self -[in] Valid Pointer to an entity_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void A_DeathScream(entity_t *self)
{
	switch (self->type) {
		case en_mutant:
			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/033.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/037.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			}
			break;

		case en_guard:
			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound(dsodsounds[(US_RndT() % 6)]),
								 (float)1.0, (float)ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_VOICE, Sound_RegisterSound(dsounds[(US_RndT() % 6)]),
								 (float)1.0, (float)ATTN_NORM, 0);
			}
			break;

		case en_officer:
			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/046.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/074.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			}
			break;

		case en_ss:
			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/035.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/046.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			}
			break;

		case en_dog:
			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/031.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/035.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			}
			break;

		case en_boss:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/019.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_schabbs:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/061.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_fake:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/069.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_mecha:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/084.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_hitler:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/044.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_gretel:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/115.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_gift:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/091.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_fat:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/119.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_spectre:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("lsfx/062.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_angel:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/098.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_trans:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/070.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_uber:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/082.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_will:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/072.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;

		case en_death:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/090.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			break;
	}
}

/*
-----------------------------------------------------------------------------
 Function: A_FirstSighting() -Puts an actor into attack mode and possibly
								reverses the direction if the player is
								behind it.

 Parameters: self -[in] Valid Pointer to an entity_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void A_FirstSighting(entity_t *self)
{
	switch (self->type) {
		case en_guard:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/001.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 3;	/* go faster when chasing player */
			break;

		case en_officer:
			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/043.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_VOICE,
								 Sound_RegisterSound("sfx/071.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			}
			self->speed *= 5;	/* go faster when chasing player */
			break;

		case en_mutant:
			self->speed *= 3;	/* go faster when chasing player */
			break;

		case en_ss:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/015.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 4;			/* go faster when chasing player */
			break;

		case en_dog:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/002.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 2;			/* go faster when chasing player */
			break;

		case en_boss:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/017.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed = SPDPATROL * 3;	/* go faster when chasing player */
			break;

		case en_gretel:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/112.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 3;			/* go faster when chasing player */
			break;

		case en_gift:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/096.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 3;			/* go faster when chasing player */
			break;

		case en_fat:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/102.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 3;			/* go faster when chasing player */
			break;

		case en_schabbs:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/065.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 3;			/* go faster when chasing player */
			break;

		case en_fake:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/054.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 3;			/* go faster when chasing player */
			break;

		case en_mecha:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/040.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 3;			/* go faster when chasing player */
			break;

		case en_hitler:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/040.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed *= 5;			/* go faster when chasing player */
			break;

		case en_blinky:
		case en_clyde:
		case en_pinky:
		case en_inky:
			self->speed *= 2;			/* go faster when chasing player */
			break;

/*
 * Spear of Destiny
 */
		case en_spectre:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("lsfx/003.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed = 800;			/* go faster when chasing player */
			break;

		case en_angel:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/095.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed = 1536;			/* go faster when chasing player */
			break;

		case en_trans:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/066.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed = 1536;			/* go faster when chasing player */
			break;

		case en_uber:
			self->speed = 3000;			/* go faster when chasing player */
			break;

		case en_will:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/073.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed = 2048;			/* go faster when chasing player */
			break;

		case en_death:
			Sound_StartSound(NULL, 1, CHAN_VOICE,
							 Sound_RegisterSound("sfx/085.wav"),
							 (float)1.0, (float)ATTN_NORM, 0);
			self->speed = 2048;			/* go faster when chasing player */
			break;

		default:
			return;
	}

	A_StateChange(self, st_chase1);
	if (self->waitfordoor) {
		self->waitfordoor = NULL;	/* ignore the door opening command */
	}

	self->dir = dir8_nodir;
	self->flags |= FL_ATTACKMODE | FL_FIRSTATTACK;
}


/*
-----------------------------------------------------------------------------
 Function: A_KillActor() -Actor has been killed, so give points and spawn
							powerups.

 Parameters: self -[in] Valid Pointer to an entity_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void A_KillActor(entity_t *self)
/* entity_t is a struct typedef-ed in "wolf-actors.h" */
{
	int	tilex, tiley; /* these are declared as "char" when they are used as
					   * members in the typedef for the entity_t struct */

	tilex = (self->x >> TILESHIFT); /* drop item on center */
	tiley = (self->y >> TILESHIFT);

	/* split into a separate assignment because it only needs to be casted to
	 * 'char' when 'tilex' and 'tiley' refer to the members in 'self': */
	self->tilex = (char)(self->x >> TILESHIFT); /* drop item on center */
	self->tiley = (char)(self->y >> TILESHIFT);

	switch (self->type) {
		case en_guard:
			PL_GivePoints(&Player, (W32)100);
			Powerup_Spawn(tilex, tiley, pow_clip2, r_world);
			break;

		case en_officer:
			PL_GivePoints(&Player, (W32)400);
			Powerup_Spawn(tilex, tiley, pow_clip2, r_world);
			break;

		case en_mutant:
			PL_GivePoints(&Player, (W32)700);
			Powerup_Spawn(tilex, tiley, pow_clip2, r_world);
			break;

		case en_ss:
			PL_GivePoints(&Player, (W32)500);
			if (Player.items & ITEM_WEAPON_3) { /* have a schmeiser? */
				Powerup_Spawn(tilex, tiley, pow_clip2, r_world);
			} else {
				Powerup_Spawn(tilex, tiley, pow_machinegun, r_world);
			}
			break;

		case en_dog:
			PL_GivePoints(&Player, (W32)200);
			break;

		case en_boss:
			PL_GivePoints(&Player, (W32)5000);
			Powerup_Spawn(tilex, tiley, pow_key1, r_world);
			break;

		case en_gretel:
			PL_GivePoints(&Player, (W32)5000);
			Powerup_Spawn(tilex, tiley, pow_key1, r_world);
			break;

		case en_gift:
			PL_GivePoints(&Player, (W32)5000);
			A_StartDeathCam(self);
			break;

		case en_fat:
			PL_GivePoints(&Player, (W32)5000);
			A_StartDeathCam(self);
			break;

		case en_schabbs:
			PL_GivePoints(&Player, (W32)5000);
			A_DeathScream(self);
			A_StartDeathCam(self);
			break;

		case en_fake:
			PL_GivePoints(&Player, (W32)2000);
			break;

		case en_mecha:
			PL_GivePoints(&Player, (W32)5000);
			break;

		case en_hitler:
			PL_GivePoints(&Player, (W32)5000);
			A_DeathScream(self);
			A_StartDeathCam(self);
			break;

		case en_spectre:
			PL_GivePoints(&Player, (W32)200);
			break;

		case en_angel:
			PL_GivePoints(&Player, (W32)5000);
			break;

		case en_trans:
			PL_GivePoints(&Player, (W32)5000);
			Powerup_Spawn(tilex, tiley, pow_key1, r_world);
			break;

		case en_uber:
			PL_GivePoints(&Player, (W32)5000);
			Powerup_Spawn(tilex, tiley, pow_key1, r_world);
			break;

		case en_will:
			PL_GivePoints(&Player, (W32)5000);
			Powerup_Spawn(tilex, tiley, pow_key1, r_world);
			break;

		case en_death:
			PL_GivePoints(&Player, (W32)5000);
			Powerup_Spawn(tilex, tiley, pow_key1, r_world);
			break;
	}

	A_StateChange(self, st_die1);
	levelstate.killed_monsters++;
	self->flags &= ~FL_SHOOTABLE;
	self->flags |= FL_NONMARK;
}


/*
-----------------------------------------------------------------------------
 Function: A_DamageActor() -Called when the player succesfully hits an enemy.

 Parameters: self -[in] Valid Pointer to an entity_t structure.

 Returns: Nothing.

 Notes:
	Does damage points to enemy ob, either putting it into a stun frame or
	killing it.
-----------------------------------------------------------------------------
*/
PUBLIC void A_DamageActor(entity_t *self, int damage)
{
	Player.madenoise = 1;

	/* do double damage if shooting a non attack mode actor */
	if (! (self->flags & FL_ATTACKMODE)) {
		damage <<= 1;
	}

	self->health -= damage;

	if (self->health <= 0) {
		A_KillActor(self);
	} else {
		if(!(self->flags & FL_ATTACKMODE)) {
			A_FirstSighting(self);		/* put into combat mode */
		}

		switch (self->type) { /* dogs only have one hit point */
			case en_guard:
			case en_officer:
			case en_mutant:
			case en_ss:
				if (self->health & 1) {
					A_StateChange(self, st_pain);
				} else {
					A_StateChange(self, st_pain1);
				}
				break;
		}
	}
}

/*************************
 *
 *	Hitler
 *
 *************************/


/*
-----------------------------------------------------------------------------
 Function: A_MechaSound -Play Mecha sound.

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void A_MechaSound(entity_t *self)
{
	/* areabyplayer is declared in "../level/wolf_level.h": */
	if (areabyplayer[(int)self->areanumber]) {
		Sound_StartSound(NULL, 1, CHAN_VOICE, Sound_RegisterSound("sfx/080.wav"),
						 (float)1.0, (float)ATTN_NORM, 0);
	}
}

/*
-----------------------------------------------------------------------------
 Function: A_Slurpie -Play Slurpie sound.

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void A_Slurpie(entity_t *self)
{
	/* TODO: use parameter 'self' */
	Sound_StartSound(NULL, 1, CHAN_VOICE, Sound_RegisterSound("lsfx/061.wav"),
					 (float)1.0, (float)ATTN_NORM, 0);
}


/*
-----------------------------------------------------------------------------
 Function: A_HitlerMorph() -Spawn new actor, when Mecha Hitler is dead.

 Parameters: self -[in] Valid Pointer to an entity_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void A_HitlerMorph(entity_t *self)
{
	int hitpoints[4] = { 500, 700, 800, 900 };
	entity_t *hitler;

	hitler = GetNewActor();
	if (! hitler) {
		return;
	}

	hitler->x = self->x;/**/
	hitler->y = self->y;/**/
	hitler->distance = self->distance;
	hitler->tilex = self->tilex;/**/
	hitler->tiley = self->tiley;/**/
	hitler->angle = self->angle;/**/
	hitler->dir = self->dir;/**/
	hitler->health = hitpoints[(int)skill->value];
	hitler->areanumber = self->areanumber;
	hitler->state = st_chase1;/**/
	hitler->type = en_hitler; /**/
	hitler->speed = (SPDPATROL * 5);/**/
	hitler->ticcount = 0;/**/
	hitler->flags = (self->flags | FL_SHOOTABLE); /**/
	hitler->sprite = Sprite_GetNewSprite();

	levelstate.killed_monsters++;
}

/**************************
 *
 *	Angel of Death
 *
 *************************/

/*
 *	Angel cannot shoot more then 3 sparks in a row.
 *	It will get tired!
 */

PRIVATE int angel_temp = 0;

/**/

/*
-----------------------------------------------------------------------------
 Function: A_Breathing -Play Angel of Death Breathing sound.

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void A_Breathing(entity_t *self)
{
	/* TODO: use parameter 'self' */
	Sound_StartSound(NULL, 0, CHAN_VOICE, Sound_RegisterSound("lsfx/080.wav"),
					 (float)1.0, (float)ATTN_NORM, 0);
}

/*
-----------------------------------------------------------------------------
 Function: A_StartAttack

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void A_StartAttack(entity_t *self)
{
	/* TODO: use parameter 'self' */
	angel_temp = 0;
}

/*
-----------------------------------------------------------------------------
 Function: A_Relaunch

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void A_Relaunch(entity_t *self)
{
	if (++angel_temp == 3) {
		A_StateChange(self, st_pain);
		return;
	}

	if (US_RndT() & 1) {
		A_StateChange(self, st_chase1);
		return;
	}
}

extern void M_Intermission_f(void);



/*
-----------------------------------------------------------------------------
 Function: A_Victory

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void A_Victory(entity_t *self)
{
	/* TODO: use parameter 'self' */
#if 0
	playstate=ex_victory; /* undeclared */
	M_Victory_f(); /* unimplemented */
#endif /* 0 */

	M_Intermission_f();
}


/*
-----------------------------------------------------------------------------
 Function: A_Dormant() -Entity is dormant state.

 Parameters: self -[in] Valid Pointer to an entity_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void A_Dormant(entity_t *self)
{
	int deltax, deltay;
	int xl, xh, yl, yh, x, y, n;

	deltax = (int)(self->x - Player.position.origin[0]);

	if ((deltax < -MINACTORDIST) || (deltax > MINACTORDIST)) {
		goto moveok;
	}

	deltay = (int)(self->y - Player.position.origin[1]);
	if ((deltay < -MINACTORDIST) || (deltay > MINACTORDIST)) {
		goto moveok;
	}

	return;

moveok:
	xl = ((self->x - MINDIST) >> TILESHIFT);
	xh = ((self->x + MINDIST) >> TILESHIFT);
	yl = ((self->y - MINDIST) >> TILESHIFT);
	yh = ((self->y + MINDIST) >> TILESHIFT);

	for ((y = yl); (y <= yh); ++y)
		for ((x = xl); (x <= xh); ++x) {
			if (r_world->tilemap[x][y] & SOLID_TILE) {
				return;
			}

			for ((n = 0); (n < NumGuards); ++n) {
				if (Guards[n].state >= st_die1) {
					continue;
				}

				if ((Guards[n].tilex == x) && (Guards[n].tiley == y)) {
					return; /* another guard in path */
				}
			}
		}

	self->flags |= FL_AMBUSH | FL_SHOOTABLE;
	self->flags &= ~FL_ATTACKMODE;
	self->dir = dir8_nodir;
	A_StateChange(self, st_path1);
}


extern void M_Intermission_f(void);


/*
-----------------------------------------------------------------------------
 Function: A_StartDeathCam

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void A_StartDeathCam(entity_t *self)
{
	/* TODO: use parameter 'self' */
	M_Intermission_f();
}


/*
-----------------------------------------------------------------------------
 Function: A_Smoke() -Rockets emmit smoke.

 Parameters: self -[in] Valid Pointer to an entity_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void A_Smoke(entity_t *self)
{
	entity_t *smoke;

	smoke = GetNewActor();
	if (! smoke) {
		return;
	}

	smoke->x = self->x;
	smoke->y = self->y;
	smoke->tilex = self->tilex;
	smoke->tiley = self->tiley;
	smoke->state = st_die1;
	smoke->type = ((self->type == en_hrocket) ? en_hsmoke : en_smoke);
	smoke->ticcount = 6;
	smoke->flags = FL_NEVERMARK;
	smoke->sprite = Sprite_GetNewSprite();
}


/*
-----------------------------------------------------------------------------
 Function: ProjectileTryMove() -Called when projectile is airborne.

 Parameters: self -[in] Valid Pointer to an entity_t structure.
			 lvl -[in] Valid Pointer to LevelData_t structure.

 Returns: true if move ok, otherwise false.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE _boolean ProjectileTryMove(entity_t *self, LevelData_t *lvl)
{
	#define PROJSIZE	0x2000


	int xl, yl, xh, yh, x, y;

	y = 0;

	/* dummy to silence clang static analyzer warning about value stored to
	 * 'y' never being read: */
	if (y == 0) {
		;
	}

	xl = ((self->x - PROJSIZE) >> TILESHIFT);
	yl = ((self->y - PROJSIZE) >> TILESHIFT);

	xh = ((self->x + PROJSIZE) >> TILESHIFT);
	yh = ((self->y + PROJSIZE) >> TILESHIFT);

	/* Checking for solid walls: */
	for ((y = yl); (y <= yh); ++y) {
		for ((x = xl); (x <= xh); ++x) {
/* FIXME: decide what to do with statics & Doors! */
			if (lvl->tilemap[x][y] & (WALL_TILE | BLOCK_TILE)) {
				return false;
			}

			if (lvl->tilemap[x][y] & DOOR_TILE) {
				if (Door_Opened(&lvl->Doors, x, y) != DOOR_FULLOPEN) {
					return false;
				}
			}
		}
	}
/* FIXME: Projectile will fly through objects (even guards & columns) - must fix to create rocket launcher! */
	return true;
}


/*
-----------------------------------------------------------------------------
 Function: T_Projectile() -Called when projectile is airborne.

 Parameters: self -[in] Valid Pointer to an entity_t structure.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_Projectile(entity_t *self)
{
	#define PROJECTILESIZE	0xC000


	int deltax, deltay, speed, damage;

	damage = 0;

	/* dummy to silence clang static analyzer warning about value stored to
	 * 'damage' never being read: */
	if (damage == 0) {
		;
	}

	speed = self->speed * tics;

	deltax = (int)(speed * cos(FINE2RAD(self->angle)));
	deltay = (int)(speed * sin(FINE2RAD(self->angle)));

	if (deltax > TILEGLOBAL) {
		deltax = TILEGLOBAL;
	}

	if (deltax < -TILEGLOBAL) {
		deltax = -TILEGLOBAL; /* my */
	}

	if (deltay > TILEGLOBAL) {
		deltay = TILEGLOBAL;
	}

	if (deltay < -TILEGLOBAL) {
		deltay = -TILEGLOBAL; /* my */
	}

	self->x += deltax;
	self->y += deltay;

	deltax = (int)ABS(self->x-Player.position.origin[0]);
	deltay = (int)ABS(self->y-Player.position.origin[1]);

	if (! ProjectileTryMove(self, r_world)) {
		if ((self->type == en_rocket) || (self->type == en_hrocket)) {
			/* rocket ran into obstacle, draw explosion! */
			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_WEAPON,
								 Sound_RegisterSound("lsfx/001.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_WEAPON,
								 Sound_RegisterSound("lsfx/086.wav"),
								 (float)1.0, (float)ATTN_NORM, 0);
			}
			A_StateChange(self, st_die1);
		} else {
			A_StateChange(self, st_remove); /* mark for removal */
		}
		return;
	}

	if ((deltax < PROJECTILESIZE) && (deltay < PROJECTILESIZE)) {
		/* hit the player */
		switch (self->type) {
			case en_needle:
				damage = (int)((US_RndT() >> 3) + 20);
				break;

			case en_rocket:
			case en_hrocket:
			case en_spark:
				damage = (int)((US_RndT() >> 3) + 30);
				break;

			case en_fire:
				damage = (int)(US_RndT() >> 3);
				break;

			default:
				damage = 0;
				break;
		}

		PL_Damage(&Player, self, damage);
		A_StateChange(self, st_remove); /* mark for removal */
		return;
	}

	self->tilex = (char)(self->x >> TILESHIFT);
	self->tiley = (char)(self->y >> TILESHIFT);
}

/* EOF */
