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

/*
 *	wolf_ai_com.c: Wolfenstein3-D actor manager.
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

#include "wolf_actors.h"
#include "wolf_player.h"
#include "wolf_actor_ai.h"
#include "wolf_ai_com.h"
#include "wolf_sprites.h"
#include "../wolf_local.h"
#include "../../../math/random_number.h"
#include "../../../math/angle.h"
#include "../../../math/mymath.h"
#include "../../../common/common.h"

#include "../../../sound/sound.h"


#define RUNSPEED	6000


/*
-----------------------------------------------------------------------------
 Function: AI_ChangeDir() -Entity is going to move in a new direction.

 Parameters:

 Returns: 1 if direction is OK, otherwise 0.

 Notes:
	Called, when actor finished previous moving & located in the 'center' of
	the tile. Entity will try walking in direction.

-----------------------------------------------------------------------------
*/
PRIVATE int AI_ChangeDir(entity_t *self, dir8type new_dir, LevelData_t *lvl)
{
	int oldx, oldy, newx, newy; /* all it tiles */
	int n;

	oldx = POS2TILE(self->x);
	oldy = POS2TILE(self->y);
	newx = oldx + dx8dir[ new_dir ];
	newy = oldy + dy8dir[ new_dir ];

	if (new_dir & 0x01) { /* same as %2 (diagonal dir) */
		if ((lvl->tilemap[newx][oldy] & SOLID_TILE) ||
			(lvl->tilemap[oldx][newy] & SOLID_TILE) ||
			(lvl->tilemap[newx][newy] & SOLID_TILE)) {
			return 0;
		}

		for ((n = 0); (n < NumGuards); ++n ) {
			if (Guards[n].state >= st_die1) {
				continue;
			}

			if ((Guards[n].tilex == newx) && (Guards[n].tiley == newy)) {
				return 0; /* another guard in path */
			}

			if ((Guards[n].tilex == oldx) && (Guards[n].tiley == newy)) {
				return 0; /* another guard in path */
			}

			if ((Guards[n].tilex == newx) && (Guards[n].tiley == oldy)) {
				return 0; /* another guard in path */
			}
		}
	} else { /* linear dir (E, N, W, S) */
		if (lvl->tilemap[newx][newy] & SOLID_TILE) {
			return 0;
		}

		if (lvl->tilemap[newx][newy] & DOOR_TILE) {
			if ((self->type == en_fake) || (self->type == en_dog)) {
				/* fake enemies and dogs cannot open doors */
				if (lvl->Doors.DoorMap[newx][newy].action != dr_open) {
					/* path is blocked by a closed opened door */
					return 0;
				}
			} else {
				self->waitfordoor = &lvl->Doors.DoorMap[newx][newy];
				goto moveok;
			}
		} for ((n = 0); (n < NumGuards); ++n) {
			if (Guards[n].state >= st_die1) {
				continue;
			}

			if ((Guards[n].tilex == newx) && (Guards[n].tiley == newy)) {
				return 0; /* another guard in path */
			}
		}
	}

moveok:
	self->tilex = (char)newx;
	self->tiley = (char)newy;

	lvl->tilemap[oldx][oldy] &= ~ACTOR_TILE; /* update map status */
	lvl->tilemap[newx][newy] |=	ACTOR_TILE;

	if (lvl->areas[newx][newy] > 0) {
		self->areanumber = (char)lvl->areas[newx][newy];
	}

	self->distance = TILEGLOBAL;
	self->dir = new_dir;

	return 1;
}

/*
-----------------------------------------------------------------------------
 Function: AI_Path() -Entity is going to turn on a way point.

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void AI_Path(entity_t *self)
{
	if (r_world->tilemap[(self->x >> TILESHIFT)][(self->y >> TILESHIFT)] &
		WAYPOINT_TILE) {
		long tileinfo = r_world->tilemap[self->x>>TILESHIFT][self->y>>TILESHIFT];
		if (tileinfo & TILE_IS_E_TURN) {
			self->dir = dir8_east;
		} else if (tileinfo & TILE_IS_NE_TURN) {
			self->dir = dir8_northeast;
		} else if (tileinfo & TILE_IS_N_TURN) {
			self->dir = dir8_north;
		} else if (tileinfo & TILE_IS_NW_TURN) {
			self->dir = dir8_northwest;
		} else if (tileinfo & TILE_IS_W_TURN) {
			self->dir = dir8_west;
		} else if (tileinfo & TILE_IS_SW_TURN) {
			self->dir = dir8_southwest;
		} else if (tileinfo & TILE_IS_S_TURN) {
			self->dir = dir8_south;
		} else if (tileinfo & TILE_IS_SE_TURN) {
			self->dir = dir8_southeast;
		}
	}

	if (! AI_ChangeDir(self, self->dir, r_world)) {
		self->dir = dir8_nodir;
	}
}

/*
-----------------------------------------------------------------------------
 Function: AI_Dodge() -Attempts to choose and initiate a movement for entity
						that sends it towards the player while dodging.

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void AI_Dodge(entity_t *self)
{
	int deltax, deltay, i;
	dir8type dirtry[5], turnaround, tdir;

	i = 0; /* initialize */

	/* dummy to silence clang static analyzer warning about value stored to
	 * 'i' never being read: */
	if (i == 0) {
		;
	}

	if (self->flags & FL_FIRSTATTACK) {
/* turning around is only ok the very first time after noticing the player */
		turnaround = dir8_nodir;
		self->flags &= ~FL_FIRSTATTACK;
	} else {
		turnaround = opposite8[self->dir];
	}

	deltax = ((int)POS2TILE(Player.position.origin[0]) - POS2TILE(self->x));
	deltay = ((int)POS2TILE(Player.position.origin[1]) - POS2TILE(self->y));

/*
 * arrange 5 direction choices in order of preference
 * the four cardinal directions plus the diagonal straight towards
 * the player
 */

	if (deltax > 0) {
		dirtry[1] = dir8_east;
		dirtry[3] = dir8_west;
	} else {
		dirtry[1] = dir8_west;
		dirtry[3] = dir8_east;
	}

	if (deltay > 0) {
		dirtry[2] = dir8_north;
		dirtry[4] = dir8_south;
	} else {
		dirtry[2] = dir8_south;
		dirtry[4] = dir8_north;
	}

/* randomize a bit for dodging */
	if (ABS(deltax) > ABS(deltay)) {
		tdir = dirtry[1]; dirtry[1]=dirtry[2]; dirtry[2]=tdir; /* => swap dirtry[1] & dirtry[2] */
		tdir = dirtry[3]; dirtry[3]=dirtry[4]; dirtry[4]=tdir; /* => swap dirtry[3] & dirtry[4] */
	}

	if (US_RndT() < 128) {
		tdir=dirtry[1]; dirtry[1]=dirtry[2]; dirtry[2]=tdir;
		tdir=dirtry[3]; dirtry[3]=dirtry[4]; dirtry[4]=tdir;
	}

	dirtry[0] = diagonal[dirtry[1]][dirtry[2]];

/* try the directions util one works */
	for ((i = 0); (i < 5); ++i) {
		if ((dirtry[i] == dir8_nodir) || (dirtry[i] == turnaround)) {
			continue;
		}

		if (AI_ChangeDir(self, dirtry[i], r_world)) {
			return;
		}
	}

/* turn around only as a last resort */
	if (turnaround != dir8_nodir) {
		if (AI_ChangeDir(self, turnaround, r_world)) {
			return;
		}
	}

	self->dir = dir8_nodir;
}

/*
-----------------------------------------------------------------------------
 Function: AI_Chase() -As AI_Dodge, but doesn't try to dodge.

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void AI_Chase(entity_t *self)
{
	int deltax, deltay;
	dir8type d[2];
	dir8type tdir, olddir, turnaround;

	olddir = self->dir;
	turnaround = opposite8[olddir];
	d[0] = d[1] = dir8_nodir;

	deltax = ((int)POS2TILE(Player.position.origin[0]) - POS2TILE(self->x));
	deltay = ((int)POS2TILE(Player.position.origin[1]) - POS2TILE(self->y));

	if (deltax > 0) {
		d[0] = dir8_east;
	} else if (deltax < 0) {
		d[0] = dir8_west;
	}

	if (deltay > 0) {
		d[1] = dir8_north;
	} else if (deltay < 0) {
		d[1] = dir8_south;
	}

	if (ABS(deltay) > ABS(deltax)) {
		tdir = d[0];
		d[0] = d[1];
		d[1] = tdir;
	} /* swap d[0] & d[1] */

	if (d[0] == turnaround) {
		d[0] = dir8_nodir;
	}

	if (d[1] == turnaround) {
		d[1] = dir8_nodir;
	}

	if (d[0] != dir8_nodir) {
		if (AI_ChangeDir(self, d[0], r_world)) {
			return;
		}
	}

	if (d[1] != dir8_nodir) {
		if (AI_ChangeDir(self, d[1], r_world)) {
			return;
		}
	}

	/* there is no direct path to the player, so pick another direction */
	if (olddir != dir8_nodir) {
		if (AI_ChangeDir(self, olddir, r_world)) {
			return;
		}
	}

	if (US_RndT() >128 ) { /* randomly determine direction of search */
		for ((tdir = dir8_east); (tdir <= dir8_south); (tdir += 2)) {
			/* * Revision */
			if (tdir != turnaround) {
				if (AI_ChangeDir(self, tdir, r_world)) {
					return;
				}
			}
		}
	} else {
		for ((tdir = dir8_south); ((int)tdir >= dir8_east); (tdir -= 2)) {
			/* * Revision */
			if (tdir != turnaround) {
				if (AI_ChangeDir(self, tdir, r_world)) {
					return;
				}
			}

		}
	}

	if (turnaround != dir8_nodir) {
		if (AI_ChangeDir(self, turnaround, r_world)) {
			return;
		}
	}

	self->dir = dir8_nodir; /* cannot move */
}


/*
-----------------------------------------------------------------------------
 Function: AI_Retreat() -Run Away from player.

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void AI_Retreat(entity_t *self)
{
	int deltax, deltay;
	dir8type d[2], tdir;

	deltax = ((int)POS2TILE(Player.position.origin[0]) - POS2TILE(self->x));
	deltay = ((int)POS2TILE(Player.position.origin[1]) - POS2TILE(self->y));

	d[0] = (deltax < 0 ? dir8_east  : dir8_west);
	d[1] = (deltay < 0 ? dir8_north : dir8_south);

	if (ABS(deltay) > ABS(deltax)) {
		tdir = d[0];
		d[0] = d[1];
		d[1] = tdir;
	} /* swap d[0] & d[1] */

	if (AI_ChangeDir(self, d[0], r_world)) {
		return;
	}

	if (AI_ChangeDir(self, d[1], r_world)) {
		return;
	}

/* there is no direct path to the player, so pick another direction */

	if (US_RndT() > 128) { /* randomly determine direction of search */
		for ((tdir = dir8_east); (tdir <= dir8_south); (tdir += 2)) {
			/* * Revision */
			if (AI_ChangeDir(self, tdir, r_world)) {
				return;
			}
		}
	} else {
		for ((tdir = dir8_south); (tdir >= dir8_east); (tdir -= 2)) {
			/* * Revision */
			if (AI_ChangeDir(self, tdir, r_world)) {
				return;
			}
		}
	}

	self->dir=dir8_nodir; /* cannot move */
}


/*
-----------------------------------------------------------------------------
 Function: AI_CheckSight() -Checks a straight line between player and
							current object.

 Parameters: buf -[out] Storage location for data.
			 offset -[in] Number of bytes from beginning of file.
			 length -[in] Maximum number of items to be read.

 Returns: true if the player has been spotted, otherwise false.

 Notes:
	If the sight is ok, check alertness and angle to see if they notice.
-----------------------------------------------------------------------------
*/
PRIVATE _boolean AI_CheckSight(entity_t *self)
{
	#define MINSIGHT 0x18000

	int deltax; /* east-west */
	int deltay; /* north-south */

/* do NOT bother tracing a line if the area is NOT connected to the player's */
	if (!(self->flags & FL_AMBUSH)) {
		if (! areabyplayer[(int)self->areanumber]) {
			return false;
		}
	}

/* if the player is real close, sight is automatic */
	deltax = ((int)Player.position.origin[0] - self->x);
	deltay = ((int)Player.position.origin[1] - self->y);

	if ((ABS(deltax) < MINSIGHT) && (ABS(deltay) < MINSIGHT)) {
		return true;
	}

/* see if they are looking in the right direction */
	switch (self->dir) {
		case dir8_north:
			if (deltay < 0) {
				return false;
			}
			break;

		case dir8_northeast:
			/* not sure if this should be an '||' instead of an '&&': */
			if ((deltay < 0) && (deltax < 0)) {
				return false;
			}
			break;

		case dir8_east:
			if (deltax < 0) {
				return false;
			}
			break;

		case dir8_southeast:
			/* not sure if this should be an '||' instead of an '&&': */
			if ((deltax < 0) && (deltay > 0)) {
				return false;
			}
			break;


		case dir8_south:
			if (deltay > 0) {
				return false;
			}
			break;

		case dir8_southwest:
			/* not sure if this should be an '||' instead of an '&&': */
			if ((deltay > 0) && (deltax > 0)) {
				return false;
			}
			break;

		case dir8_west:
			if (deltax > 0) {
				return false;
			}
			break;

		case dir8_northwest:
			/* not sure if this should be an '||' instead of an '&&': */
			if ((deltax > 0) && (deltay < 0)) {
				return false;
			}
			break;

		case dir8_nodir:
			/* I really have no clue what to do in this case, but I suppose that
			 * I can always guess...: */
			if ((deltax > 0) || (deltax < 0) || (deltay > 0) || (deltay < 0)) {
				return false;
			}
			break;
		default:
			break;
	}

/* trace a line to check for blocking tiles (corners) */
	return Level_CheckLine((SW32)self->x, (SW32)self->y,
						   Player.position.origin[0], Player.position.origin[1],
						   r_world);
}


/*
-----------------------------------------------------------------------------
 Function: AI_FindTarget() -Called by entities that ARE NOT chasing the player.

 Parameters:

 Returns:
	If the player is detected (by sight, noise, or proximity), the entity
	is put into its combat frame and true is returned.

 Notes:
	Incorporates a random reaction delay.
-----------------------------------------------------------------------------
*/
PRIVATE _boolean AI_FindTarget(entity_t *self)
{
	if (self->temp2) { /* count down reaction time */
		self->temp2 -= tics;
		if (self->temp2 > 0) {
			return false;
		}
		self->temp2 = 0; /* time to react */
	} else {
		/* check if we can/want to see/hear player */
		if (Player.flags & FL_NOTARGET) {
			return false; /* notarget cheat */
		}

		if (!(self->flags & FL_AMBUSH) && ! areabyplayer[(int)self->areanumber]) {
			return false;
		}


		if (! AI_CheckSight(self)) { /* Player is visible - normal behavior */
			if ((self->flags & FL_AMBUSH) || ! Player.madenoise) {
				return false;
			}
		}
		self->flags &= ~FL_AMBUSH;

		/* if we are here we see/hear player!!! */
		switch (self->type) {
			case en_guard:
				self->temp2 = (1 + ((int)US_RndT() / 4));
				break;

			case en_officer:
				self->temp2 = 2;
				break;

			case en_mutant:
				self->temp2 = (1 + ((int)US_RndT() / 6));
				break;

			case en_ss:
				self->temp2 = (1 + ((int)US_RndT() / 6));
				break;

			case en_dog:
				self->temp2 = (1 + ((int)US_RndT() / 8));
				break;

			case en_boss:
			case en_schabbs:
			case en_fake:
			case en_mecha:
			case en_hitler:
			case en_gretel:
			case en_gift:
			case en_fat:
			case en_spectre:
			case en_angel:
			case en_trans:
			case en_uber:
			case en_will:
			case en_death:
				self->temp2 = 1;
				break;
		}

		return false;  /* we are amazed & waiting to understand what to do! */
	}

	A_FirstSighting(self);

	return true;
}


/*
-----------------------------------------------------------------------------
 Function: T_Move() -Moves object for distance in global units,
					in ob->dir direction.

 Parameters:

 Returns:
	If the player is detected (by sight, noise, or proximity), the entity
	is put into its combat frame and true is returned.

 Notes:
	ob->x	= adjusted for new position
	ob->y

	Actors are not allowed to move inside the player.
	Does NOT check to see if the move is tile map valid.
-----------------------------------------------------------------------------
*/
PRIVATE void T_Move(entity_t *self, long dist)
{

	if ((self->dir == dir8_nodir) || ! dist) {
		return;
	}

	self->x += (int)(dist * dx8dir[self->dir]);
	self->y += (int)(dist * dy8dir[self->dir]);

	/* check to make sure it is/was not on top of player */
	if (ABS(self->x - Player.position.origin[0]) <= MINACTORDIST)
		if (ABS(self->y - Player.position.origin[1]) <= MINACTORDIST) {
			if ((self->type == en_blinky) || (self->type == en_clyde) ||
				(self->type == en_pinky) || (self->type == en_inky) ||
				(self->type == en_spectre)) {
				PL_Damage(&Player, self, 2); /* ghosts hurt player! */
			}
/*
 * back up
 */
			self->x -= (int)(dist * dx8dir[self->dir]);
			self->y -= (int)(dist * dy8dir[self->dir]);
			return;
		}

	self->distance -= (int)dist;
	if (self->distance < 0) {
		self->distance = 0;
	}
}


/*
-----------------------------------------------------------------------------
 Function: T_Advance() -Move object forward.

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void T_Advance( entity_t *self, think_t think )
{
	long move;

	if( ! think ) {
		Com_DPrintf( "Warning: Advance without <think> proc\n" );
		return;
	}

	move = (long) self->speed * tics;
	while( move > 0) {

		/* waiting for a door to open */
 		if( self->waitfordoor ) {
			Door_OpenDoor( self->waitfordoor );
			if( self->waitfordoor->action != dr_open ) {
				return; /* not opened yet... */
			}
			self->waitfordoor = NULL;	/* go ahead, the door is now open */
		}

		if( move < self->distance ) {
			T_Move( self, move );
			break;
		}

		/* fix position to account for round off during moving */
		self->x = TILE2POS( self->tilex );
		self->y = TILE2POS( self->tiley );

		move -= self->distance;

		/* think: Where to go now? */
 		think( self );
		self->angle = dir8angle[ self->dir ];
		if( self->dir == dir8_nodir ) {
			return; /* all movement is blocked */
		}
	}
}


/*
-----------------------------------------------------------------------------
 Function: T_Stand

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_Stand( entity_t *self )
{
	AI_FindTarget( self );
}

/*
-----------------------------------------------------------------------------
 Function: T_Path

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_Path( entity_t *self )
{

	if( AI_FindTarget( self ) ) {
		return;
	}

	if( ! self->speed ) {
		return; /* if patroling with a speed of 0 */
	}

	if( self->dir == dir8_nodir ) {
		AI_Path( self );
		if( self->dir == dir8_nodir ) {
			return; /* all movement is blocked */
		}
	}

	T_Advance(self, AI_Path);
}

/*
-----------------------------------------------------------------------------
 Function: T_Ghosts

 Parameters: Nothing (really?)

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_Ghosts( entity_t *self )
{
	if( self->dir == dir8_nodir ) {
		AI_Chase( self );
		if( self->dir == dir8_nodir ) {
			return;	/* object is blocked in */
		}

		self->angle = dir8angle[ self->dir ];
	}

	T_Advance( self, AI_Chase );
}


/*
-----------------------------------------------------------------------------
 Function: T_Chase

 Parameters: Nothing (really?)

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_Chase(entity_t *self)
{
	int	dx,dy,dist,chance;
	char dodge;

#if 0
	if (gamestate.victoryflag) { /* 'gamestate' is undeclared */
		return;
	}
#endif /* 0 */

	dodge = 0;
	if (Level_CheckLine((SW32)self->x, (SW32)self->y, Player.position.origin[0],
						Player.position.origin[1], r_world)) {
		/* got a shot at player? */
		dx = ((int)ABS(POS2TILE(self->x) -
					   POS2TILE(Player.position.origin[0])));
		dy = ((int)ABS(POS2TILE(self->y) -
					   POS2TILE(Player.position.origin[1])));
		dist = max_of_2(dx, dy);
		if (! dist || ((dist == 1) && (self->distance < 16))) {
			chance = 300;
		} else {
			chance = ((tics << 4) / dist);/*100/dist;*/
		}

		if ((int)US_RndT() < chance) {
			/* go into attack frame */
			A_StateChange(self, st_shoot1);
			return;
		}
		dodge = 1;
	}

	if (self->dir == dir8_nodir) {
		if (dodge) {
			AI_Dodge(self);
		} else {
			AI_Chase(self);
		}

		if (self->dir == dir8_nodir) {
			return; /* object is blocked in */
		}
		self->angle = dir8angle[self->dir];
	}

	T_Advance(self, dodge ? AI_Dodge : AI_Chase);
}

/*
-----------------------------------------------------------------------------
 Function: T_Bite

 Parameters: Nothing (really?)

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_Bite(entity_t *self)
{
	long dx, dy;

	Sound_StartSound(NULL, 1, CHAN_VOICE, Sound_RegisterSound("lsfx/076.wav"),
					 (float)1.0, (float)ATTN_NORM, 0);

	dx = (ABS(Player.position.origin[0] - self->x) - TILEGLOBAL);
	if (dx <= MINACTORDIST) {
		dy = (ABS(Player.position.origin[1] - self->y) - TILEGLOBAL);
		if (dy <= MINACTORDIST) {
			if (US_RndT() < 180) {
				PL_Damage(&Player, self, ((int)US_RndT() >> 4));
				return;
			}
		}
	}
}


/*
-----------------------------------------------------------------------------
 Function: T_DogChase

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_DogChase( entity_t *self )
{
	long dx, dy;

	if( self->dir == dir8_nodir ) {
		AI_Dodge( self );
		self->angle = dir8angle[ self->dir ];
		if( self->dir == dir8_nodir ) {
			return; /* object is blocked in */
		}
	}

/*
 * check for bite range
 */
	dx = ABS( Player.position.origin[ 0 ] - self->x ) - TILEGLOBAL / 2;
	if(dx <= MINACTORDIST) {
		dy = ABS( Player.position.origin[ 1 ] - self->y ) - TILEGLOBAL / 2;
		if( dy <= MINACTORDIST ) {
			A_StateChange( self, st_shoot1 );
			return; /* bite player! */
		}
	}

	T_Advance( self, AI_Dodge );
}


/*
-----------------------------------------------------------------------------
 Function: T_BossChase

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
	They retreat if too close to player.
-----------------------------------------------------------------------------
*/
PUBLIC void T_BossChase(entity_t *self)
{
	int	dx, dy, dist;
	W8 dodge;

	dodge = 0;
	dx = ((int)ABS(self->tilex - POS2TILE(Player.position.origin[0])));
	dy = ((int)ABS(self->tiley - POS2TILE(Player.position.origin[1])));
	dist = max_of_2(dx, dy);

	if (Level_CheckLine((SW32)self->x, (SW32)self->y, Player.position.origin[0],
						Player.position.origin[1], r_world)) {
		/* got a shot at player? */
		if ((int)US_RndT() < (tics << 3)) {
			/* go into attack frame */
			A_StateChange(self, st_shoot1);
			return;
		}
		dodge = 1;
	}

	if (self->dir == dir8_nodir) {
		if (dodge) {
			AI_Dodge(self);
		} else {
			AI_Chase(self);
		}

		if (self->dir == dir8_nodir) {
			return;	/* object is blocked in */
		}
	}

	T_Advance(self, dist < 4 ? AI_Retreat : (dodge ? AI_Dodge : AI_Chase));
}

/*
-----------------------------------------------------------------------------
 Function: T_Fake

 Parameters: Nothing (really?)

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_Fake(entity_t *self)
{

	if (Level_CheckLine((SW32)self->x, (SW32)self->y, Player.position.origin[0],
						Player.position.origin[1], r_world)) {
		/* got a shot at player? */
		if ((int)US_RndT() < (tics << 1)) { /* go into attack frame */
			A_StateChange(self, st_shoot1);
			return;
		}
	}

	if (self->dir == dir8_nodir) {
		AI_Dodge(self);
		if (self->dir == dir8_nodir) {
			return; /* object is blocked in */
		}
	}

	T_Advance( self, AI_Dodge );
}



/*
-----------------------------------------------------------------------------
 Function: T_Shoot -Try to damage the player.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_Shoot(entity_t *self)
{
	int	dx, dy, dist;
	int	hitchance, damage;

	if (! areabyplayer[(int)self->areanumber]) {
		return;
	}

	if (! Level_CheckLine((SW32)self->x, (SW32)self->y, Player.position.origin[0],
						  Player.position.origin[1], r_world)) {
		return; /* player is behind a wall */
	}

	dx = ((int)ABS(POS2TILE(self->x) - POS2TILE(Player.position.origin[0])));
	dy = ((int)ABS(POS2TILE(self->y) - POS2TILE(Player.position.origin[1])));
	dist = max_of_2(dx, dy);

	if ((self->type == en_ss) || (self->type == en_boss)) {
		dist = dist * 2 / 3; /* ss are better shots */
	}

	if (Player.speed >= RUNSPEED) {
		hitchance = 160;
	} else {
		hitchance = 256;
	}

#ifndef M_PI
# define M_PI	3.14159265358979323846f	/* matches value in gcc v2 math.h */
#endif /* !M_PI */

/* if guard is visible by player
 * player can see to dodge
 * (if CheckLine both player & enemy see each other)
 * So left only check if guard is in player's fov: FIXME: not fixed fov! */
 	if (angle_diff(TransformPoint(self->x, self->y, Player.position.origin[0],
								  Player.position.origin[1]),
				   (float)RAD2DEG(Player.position.angle)) < (M_PI / 3)) {
		hitchance -= dist * 16;
	} else {
		hitchance -= dist * 8;
	}

/* see if the shot was a hit */
	if ((int)US_RndT() < hitchance) {
		if (dist < 2) {
			damage = ((int)US_RndT() >> 2);
		} else if (dist < 4) {
			damage = ((int)US_RndT() >> 3);
		} else {
			damage = ((int)US_RndT() >> 4);
		}

		PL_Damage(&Player, self, damage);
	}

	switch (self->type) {
		case en_ss:
			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_WEAPON,
								 Sound_RegisterSound("sfx/020.wav"), 1,
								 ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_WEAPON,
								 Sound_RegisterSound("sfx/024.wav"), 1,
								 ATTN_NORM, 0);
			}
			break;

		case en_gift:
		case en_fat:
		case en_mecha:
		case en_hitler:
		case en_boss:
			Sound_StartSound(NULL, 1, CHAN_WEAPON,
							 Sound_RegisterSound("sfx/022.wav"), 1,
							 ATTN_NORM, 0);
			break;

		default:
			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_WEAPON,
								 Sound_RegisterSound("sfx/038.wav"), 1,
								 ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_WEAPON,
								 Sound_RegisterSound("sfx/049.wav"), 1,
								 ATTN_NORM, 0);
			}
			break;
	}
}

/*
-----------------------------------------------------------------------------
 Function: T_UShoot -[UberMutant]

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_UShoot(entity_t *self)
{
	int dx, dy, dist;

	T_Shoot(self);

	dx = ((int)ABS(self->tilex - POS2TILE(Player.position.origin[0])));
	dy = ((int)ABS(self->tiley - POS2TILE(Player.position.origin[1])));
	dist = max_of_2(dx, dy);

	if (dist <= 1) {
		PL_Damage(&Player, self, 10);
	}
}

/*
-----------------------------------------------------------------------------
 Function: T_Launch

 Parameters: Nothing (really?)

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void T_Launch(entity_t *self)
{
	entity_t *proj;
	float iangle;

	iangle = (float)((float)TransformPoint(self->x, self->y,
										   Player.position.origin[0],
										   Player.position.origin[1]) + M_PI);
	if (iangle > (2 * M_PI)) {
		iangle -= (float)(2 * M_PI);
	}

	if (self->type == en_death) {
		/* death knight launches 2 rockets with 4 degree shift each. */
		T_Shoot(self);
		if (self->state == st_shoot2) {
			iangle = (angle_normalize((float)iangle - (float)(DEG2RAD(4))));
		} else {
			iangle = (angle_normalize((float)iangle + (float)(DEG2RAD(4))));
		}
	}

	proj = GetNewActor();
	if (proj == NULL) {
		return;
	}

	proj->x = self->x;
	proj->y = self->y;

	proj->tilex = self->tilex;
	proj->tiley = self->tiley;

	proj->state = st_stand;
	proj->ticcount = 1;
	proj->dir = dir8_nodir;

	proj->angle = (int)RAD2FINE(iangle);
	proj->speed = 0x2000;
	proj->flags = FL_NONMARK; /* FL_NEVERMARK; */
	proj->sprite = Sprite_GetNewSprite();

	switch (self->type) {
		case en_death:
			proj->type = en_hrocket;
			Sound_StartSound(NULL, 1, CHAN_WEAPON,
							 Sound_RegisterSound("lsfx/078.wav"), 1,
							 ATTN_NORM, 0);
			break;

		case en_angel:
			proj->type = en_spark;
			proj->state = st_path1;
			Sound_StartSound(NULL, 1, CHAN_WEAPON,
							 Sound_RegisterSound("lsfx/069.wav"), 1,
							 ATTN_NORM, 0);
			break;

		case en_fake:
			proj->type = en_fire;
			proj->state = st_path1;
			proj->flags = FL_NEVERMARK;
			proj->speed = 0x1200;
			Sound_StartSound(NULL, 1, CHAN_WEAPON,
							 Sound_RegisterSound("lsfx/069.wav"), 1,
							 ATTN_NORM, 0);
			break;

		case en_schabbs:
			proj->type = en_needle;
			proj->state = st_path1;
			Sound_StartSound(NULL, 1, CHAN_WEAPON,
							 Sound_RegisterSound("lsfx/008.wav"), 1,
							 ATTN_NORM, 0);
			break;

		default:
			proj->type = en_rocket;

			if (g_version->value == SPEAROFDESTINY) {
				Sound_StartSound(NULL, 1, CHAN_WEAPON,
								 Sound_RegisterSound("lsfx/008.wav"), 1,
								 ATTN_NORM, 0);
			} else {
				Sound_StartSound(NULL, 1, CHAN_WEAPON,
								 Sound_RegisterSound("lsfx/085.wav"), 1,
								 ATTN_NORM, 0);
			}
	}

}

/* EOF */
