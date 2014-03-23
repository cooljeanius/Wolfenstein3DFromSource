/*
 *	Copyright (C) 2004 Michael Liebscher
 *	Copyright (C) 2000-2002 by DarkOne the Hacker
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

#include <stdlib.h>

#include "wolf_player.h"
#include "../wolf_local.h"
#include "../video/wolf_raycast.h"
#include "../entities/wolf_actor_ai.h"

#include "../../../math/random_number.h"
#include "../../../sound/sound.h"

/*
-----------------------------------------------------------------------------
 Function: fire_hit

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void fire_hit(player_t *self)
{
	entity_t *closest;
	int dist, d1, n, shot_dist, damage;

	Sound_StartSound(NULL, 0, CHAN_WEAPON, Sound_RegisterSound("lsfx/023.wav"),
					 1, ATTN_NORM, 0);

/* actually fire */
	dist = 0x7fffffff;
	closest = NULL;

	for ((n = 0); (n < NumGuards); ++n ) {
		if (Guards[n].flags & FL_SHOOTABLE /*&& Guards[n].flags&FL_VISABLE*/) {
            shot_dist = Point2LineDist((int)((int)Guards[n].x -
											 self->position.origin[0]),
									   (int)((int)Guards[n].y -
											 self->position.origin[1]),
									   self->position.angle);

			if (shot_dist > (2 * TILEGLOBAL / 3))  {
				continue; /* miss */
			}

			d1 = LineLen2Point((int)((int)Guards[n].x -
									 self->position.origin[0]),
							   (int)((int)Guards[n].y -
									 self->position.origin[1]),
							   self->position.angle);

			if ((d1 < 0) || (d1 > dist)) {
				continue;
			}

			if (! Level_CheckLine((SW32)Guards[n].x,
								  (SW32)Guards[n].y,
								  Player.position.origin[0],
								  Player.position.origin[1], r_world)) {
				/* removed a commented-out condition here */
				continue; /* obscured */
			}

			dist = d1;
			closest = &Guards[n];
		}
	}

	if (! closest || (dist > TILE2POS(1))) {
		return; /* missed if further than 1.5 tiles */
	}

	damage = ((int)US_RndT() >> 4);

	A_DamageActor(closest, damage); /* hit something */
}

/*
-----------------------------------------------------------------------------
 Function: fire_lead

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void fire_lead(player_t *self)
{
	entity_t *closest;
	int damage;
	int dx, dy, dist;
	int d1, shot_dist, n;

	switch (self->weapon) {
		case WEAPON_PISTOL:
			Sound_StartSound(NULL, 0, CHAN_WEAPON,
							 Sound_RegisterSound("sfx/012.wav"), 1,
							 ATTN_NORM, 0);
			break;

		case WEAPON_AUTO:
			Sound_StartSound(NULL, 0, CHAN_WEAPON,
							 Sound_RegisterSound("sfx/011.wav"), 1,
							 ATTN_NORM, 0);
			break;

		case WEAPON_CHAIN:
			Sound_StartSound(NULL, 0, CHAN_WEAPON,
							 Sound_RegisterSound("sfx/013.wav"), 1,
							 ATTN_NORM, 0);
			break;
	}
	self->madenoise = true;

	dist = 0x7fffffffl;
	closest = NULL;

	for ((n = 0); (n < NumGuards); ++n) {
		if (Guards[n].flags & FL_SHOOTABLE /*&& Guards[n].flags&FL_VISABLE*/) {
			shot_dist = Point2LineDist((int)((int)Guards[n].x -
											 self->position.origin[0]),
									   (int)((int)Guards[n].y -
											 self->position.origin[1]),
									   self->position.angle);
			if (shot_dist > (2 * TILEGLOBAL / 3)) {
				continue; /* miss */
			}

			d1 = LineLen2Point((int)((int)Guards[n].x -
									 self->position.origin[0]),
							   (int)((int)Guards[n].y -
									 self->position.origin[1]),
							   self->position.angle);
			if ((d1 < 0) || (d1 > dist)) {
				continue;
			}

			if (! Level_CheckLine((SW32)Guards[n].x, (SW32)Guards[n].y,
								  Player.position.origin[0],
								  Player.position.origin[1], r_world)) {
				/* removed a commented-out condition here */
				continue; /* obscured */
			}

			dist = d1;
			closest = &Guards[n];
		}
	}

	if (! closest) { /* missed */
		r_trace_t trace;

        trace.angle = (angle_normalize((float)((float)(self->position.angle) -
											   DEG2RAD(2) +
											   (DEG2RAD(rand() % 4)))));

		trace.x = (int)(self->position.origin[0]);
		trace.y = (int)(self->position.origin[1]);
		trace.flags = TRACE_BULLET;
		trace.tile_vis = NULL;
		R_Trace(&trace, r_world);

		if (trace.flags & TRACE_HIT_DOOR) {
			Sound_StartSound(NULL, 0, CHAN_AUTO,
							 Sound_RegisterSound("lsfx/028.wav"), 1,
							 ATTN_NORM, 0);
		}
		return;
	}

/* hit something */
	dx = ABS(closest->tilex - self->tilex);
	dy = ABS(closest->tiley - self->tiley);
	dist = max_of_2(dx, dy);

	if (dist < 2) {
		damage = ((int)US_RndT() / 4);
	} else if( dist < 4 ) {
		damage = ((int)US_RndT() / 6);
	} else {
		if (((int)US_RndT() / 12) < dist) {
			return; /* missed */
		}

		damage = ((int)US_RndT() / 6);
	}

	A_DamageActor(closest, damage);
}

/* EOF */
