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
 *	wolf_player.c:  Wolfenstein3-D player management.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	Portion of this code was derived from NewWolf, and was originally
 *	written by DarkOne the Hacker.
 *
 */


#include <stdio.h>
#include <string.h>


#include "wolf_player.h"
#include "wolf_powerups.h"
#include "../wolf_local.h"
#include "../video/wolf_renderer.h"
#include "../../../math/random_number.h"
#include "../../../client/client.h"
#include "../../../common/cmd.h"
#include "../../../string/com_string.h"

#include "../../../sound/sound.h"

player_t Player; /* player struct (pos, health etc...) */


#define PLAYERSIZE	MINDIST	/* player radius */


extern void M_Intermission_f(void);


struct atkinf
{
	char tics, attack, frame; /* attack is 1 for gun, 2 for knife */

} attackinfo[4][14] = /* 4 guns, 14 frames max for every gun! */
{
	{ {6,0,1},{6,2,2},{6,0,3},{6,-1,4} },
	{ {6,0,1},{6,1,2},{6,0,3},{6,-1,4} },
	{ {6,0,1},{6,1,2},{6,3,3},{6,-1,4} },
	{ {6,0,1},{6,1,2},{6,4,3},{6,-1,4} },
};
/* that darn semicolon... */


/*
-----------------------------------------------------------------------------
 Function: PL_ChangeWeapon

 Parameters:

 Returns: true if player can change weapons, otherwise false.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE _boolean PL_ChangeWeapon(player_t *self, int weapon)
{
	unsigned itemflag;

	itemflag = ((unsigned int)ITEM_WEAPON_1 << weapon);

	if (self->ammo[AMMO_BULLETS] == 0 && weapon != WEAPON_KNIFE) {
		Com_Printf("Not enough ammo.\n");
		return false;
	}

	if (!(self->items & itemflag)) {
		Com_Printf("No weapon.\n");
		return false;
	}

	self->weapon =
	self->pendingweapon = weapon;

	self->attackframe =
	self->attackcount =
	self->weaponframe = 0;

	return true;
}


/*
-----------------------------------------------------------------------------
 Function: PL_Use - Called if player pressed USE button

 Parameters:

 Returns: returns true if player used something

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE _boolean PL_Use( player_t *self, LevelData_t *lvl )
{
	int x, y, dir; /* should "dir" really be an "int" instead of "dir4type"? */

    dir = Get4dir( self->position.angle );
	x = self->tilex + dx4dir[ dir ];
	y = self->tiley + dy4dir[ dir ];

	if( lvl->tilemap[ x ][ y ] & DOOR_TILE ) {
		return Door_TryUse(&lvl->Doors.DoorMap[ x ][ y ], (int)Player.items);
	}

	if( lvl->tilemap[ x ][ y ] & SECRET_TILE ) {
		return PushWall_Push(x, y, (dir4type)dir);
	}

	if( lvl->tilemap[ x ][ y ] & ELEVATOR_TILE ) {
		int newtex;

		switch( dir ) {
			case dir4_east:
			case dir4_west:
				newtex = (lvl->wall_tex_x[ x ][ y ] += 2);
				/* dummy to silence clang static analyzer warning about value
				 * stored to 'newtex' never being read: */
				if (newtex == 2) {
					;
				}
				break;

			case dir4_north:
			case dir4_south:
				return false; /* do NOT allow to press elevator rails */
		}

		if( lvl->tilemap[ self->tilex ][ self->tiley ] & SECRETLEVEL_TILE ) {
			self->playstate = ex_secretlevel;
		} else {
			self->playstate = ex_complete;
		}
		Sound_StartSound( NULL, 0, CHAN_BODY, Sound_RegisterSound( "lsfx/040.wav" ), 1, ATTN_NORM, 0 );

		return true;
	}

#if 0
	Sound_StartSound( NULL, 0, CHAN_BODY, Sound_RegisterSound( "lsfx/020.wav" ), 1, ATTN_NORM, 0 );
#endif /* 0 */
	return false;
}



#define STOPSPEED 0x0D00
#define FRICTION  0.25f
#define MAXMOVE 	(MINDIST*2-1)


/*
-----------------------------------------------------------------------------
 Function: PL_TryMove

 Parameters:

 Returns: returns true if move ok

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE _boolean PL_TryMove(player_t *self, LevelData_t *lvl)
{
	int xl, yl, xh, yh, x, y;
	int d, n;

	xl = ((int)POS2TILE(Player.position.origin[0] - PLAYERSIZE));
	yl = ((int)POS2TILE(Player.position.origin[1] - PLAYERSIZE));
	xh = ((int)POS2TILE(Player.position.origin[0] + PLAYERSIZE));
	yh = ((int)POS2TILE(Player.position.origin[1] + PLAYERSIZE));

	/* Checking for solid walls: */
	for ((y = yl); (y <= yh); ++y) {
		for ((x = xl); (x <= xh); ++x) {
			if (lvl->tilemap[x][y] & SOLID_TILE) {
				return 0;
			}

			if (lvl->tilemap[x][y] & DOOR_TILE &&
				Door_Opened(&lvl->Doors, x, y) != DOOR_FULLOPEN) {
				return 0;
			}
		}
	}

	/* check for actors */
	for ((n = 0); (n < NumGuards); ++n) {
		if (Guards[n].state >= st_die1) {
			continue;
		}

		d = ((int)self->position.origin[0] - Guards[n].x);

		if ((d < -MINACTORDIST) || (d > MINACTORDIST)) {
			continue;
		}

		d = ((int)self->position.origin[1] - Guards[n].y);

		if ((d < -MINACTORDIST) || (d > MINACTORDIST)) {
			continue;
		}

		return false;
	}

	return true;
}

/*
-----------------------------------------------------------------------------
 Function: PL_ClipMove

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void PL_ClipMove(player_t *self, int xmove, int ymove)
{
	int basex, basey;

	basex = ((int)self->position.origin[0]);
	basey = ((int)self->position.origin[1]);

	self->position.origin[0] += xmove;
	self->position.origin[1] += ymove;
	if (PL_TryMove(self, r_world)) {
		return; /* we moved as we wanted */
	}

#if 0 || __clang_analyzer__
	Sound_StartSound(NULL, 0, CHAN_BODY, Sound_RegisterSound("lsfx/000.wav"),
					 1, ATTN_NORM, 0);
#endif /* 0 || __clang_analyzer__ */

	if (xmove) { /* do NOT bother if we do NOT move x! */
		self->position.origin[0] = (basex + xmove);
		self->position.origin[1] = basey;
		if (PL_TryMove(self, r_world)) {
			return; /* Maybe we will move only X direction? */
		}
	}
	if (ymove)	{ /* do NOT bother if we do NOT move y! */
		self->position.origin[0] = basex;
		self->position.origin[1] = (basey + ymove);
		if (PL_TryMove(self, r_world)) {
			return; /* Maybe we will move only Y direction? */
		}
	}

	/* movement blocked; we must stay in one place... :-( */
	self->position.origin[0] = basex;
	self->position.origin[1] = basey;
}


/*
-----------------------------------------------------------------------------
 Function: PL_ControlMovement - Changes player's angle and position

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void PL_ControlMovement( player_t *self, LevelData_t *lvl )
{
	int speed;
    float angle;

	speed = 0;

	/* dummy to silence clang static analyzer warning about value stored to
	 * 'speed' never being read: */
	if (speed == 0) {
		;
	}

	/* rotation */
    angle = self->position.angle;

#ifdef cmd
	if(cmd->forwardmove || cmd->sidemove || (speed == 0)) {
		self->movx = self->movy = 0; /* clear accumulated movement */
	}
#else
	self->movx = self->movy = 0; /* clear accumulated movement */
#endif /* cmd */

	if( ClientState.cmd.forwardmove ) {
		speed = tics * ClientState.cmd.forwardmove;
		self->movx += (int)(speed * cos( angle ) );
		self->movy += (int)(speed * sin( angle ) );
	}
	if( ClientState.cmd.sidemove ) {
		speed = tics * ClientState.cmd.sidemove;
		self->movx += (int)( speed * sin( angle ) );
		self->movy -= (int)( speed * cos( angle  ) );
	}

	if( ! self->movx && ! self->movy ) {
		return;
	}

#ifdef SPEAR
	funnyticount = 0; /* ZERO FUNNY COUNTER IF MOVED! */ /* FIXME! */
#endif /* SPEAR */

	self->speed = (self->movx + self->movy);

	/* bound movement */
	if (self->movx > MAXMOVE) {
		self->movx = MAXMOVE;
	} else if (self->movx < -MAXMOVE) {
		self->movx = -MAXMOVE;
	}

	if (self->movy > MAXMOVE) {
		self->movy = MAXMOVE;
	} else if (self->movy < -MAXMOVE) {
		self->movy = -MAXMOVE;
	}

	/* move player and clip movement to walls (check for no-clip mode here) */
	PL_ClipMove(self, self->movx, self->movy);
	self->tilex = ((int)POS2TILE(self->position.origin[0]));
	self->tiley = ((int)POS2TILE(self->position.origin[1]));

	Powerup_PickUp(self->tilex, self->tiley);

	/* Checking for area change */
	if ((lvl->areas[self->tilex][self->tiley] >= 0) &&
		(lvl->areas[self->tilex][self->tiley] != Player.areanumber)) {
		Player.areanumber = lvl->areas[self->tilex][self->tiley];
		Areas_ConnectAreas(Player.areanumber);
	}

	if (lvl->tilemap[self->tilex][self->tiley] & EXIT_TILE) {
		M_Intermission_f();
	}

}

/*
-----------------------------------------------------------------------------
 Function: PL_PlayerAttack

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void PL_PlayerAttack(player_t *self, _boolean re_attack)
{
	struct atkinf *cur;

	self->attackcount -= tics;
	while (self->attackcount <= 0) {
		cur = &attackinfo[self->weapon][self->attackframe];
		switch (cur->attack) {
			case -1:
				self->flags &= (unsigned int)(~PL_FLAG_ATTCK);
				if (! self->ammo[AMMO_BULLETS]) {
					self->weapon = WEAPON_KNIFE;
				} else if (self->weapon != self->pendingweapon) {
					self->weapon = self->pendingweapon;
				}
				self->attackframe = self->weaponframe = 0;
				return;

			case 4:
				if (! self->ammo[AMMO_BULLETS]) {
					break;
				}

				if (re_attack) {
					self->attackframe -= 2;
				}

			case 1:
				if (! self->ammo[AMMO_BULLETS]) {
					/* can only happen with chain gun */
					self->attackframe++;
					break;
				}
				fire_lead(self);
				self->ammo[AMMO_BULLETS]--;
				break;

			case 2:
				fire_hit(self);
				break;

			case 3:
				if(self->ammo[AMMO_BULLETS] && re_attack) {
					self->attackframe-=2;
				}
				break;
		}

		self->attackcount += cur->tics;
		self->attackframe++;
		self->weaponframe = attackinfo[self->weapon][self->attackframe].frame;
	}

}


/*
-----------------------------------------------------------------------------
 Function: PL_Process

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void PL_Process(player_t *self, LevelData_t *lvl)
{
	int n;

	self->madenoise = false;

	PL_ControlMovement(self, lvl);

	if (self->flags & PL_FLAG_ATTCK) {
		PL_PlayerAttack(self, (ClientState.cmd.buttons & BUTTON_ATTACK));
	} else {
		if (ClientState.cmd.buttons & BUTTON_USE) {
			if(!(self->flags & PL_FLAG_REUSE) && PL_Use(self, lvl)) {
				self->flags |= PL_FLAG_REUSE;
			}
		} else {
			self->flags &= (unsigned int)(~PL_FLAG_REUSE);
		}

		if (ClientState.cmd.buttons & BUTTON_ATTACK) {
			self->flags |= PL_FLAG_ATTCK;

			self->attackframe = 0;
			self->attackcount = attackinfo[self->weapon][0].tics;
			self->weaponframe = attackinfo[self->weapon][0].frame;
		}
	}

	/* process impulses */
	switch( ClientState.cmd.impulse ) {
		case 0:
			break; /* no impulse */

		case 1:
		case 2:
		case 3:
		case 4:
			PL_ChangeWeapon( self, ClientState.cmd.impulse - 1 );
			break;

		case 10: /* next weapon /like in Quake/ FIXME: weapprev, weapnext */
			self->pendingweapon=self->weapon;
			for( n = 0 ; n < 4; ++n ) {
				if( ++self->weapon > WEAPON_CHAIN ) {
					self->weapon = WEAPON_KNIFE;
				}

				if( PL_ChangeWeapon( self, self->weapon ) ) {
					break;
				}
			}
			self->weapon = self->pendingweapon;
			break;

		default:
			Com_Printf( "Unknown Impulse: %d\n", ClientState.cmd.impulse );
			break;
	}

}



/*
-----------------------------------------------------------------------------
 Function: PL_Reset

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Simply calls memset() for the 'Player' global variable.
-----------------------------------------------------------------------------
*/
PUBLIC void PL_Reset(void)
{
	memset(&Player, 0, sizeof(Player));
	Player.playstate = ex_notingame;
}

extern SW32 r_damageflash;

/*
-----------------------------------------------------------------------------
 Function: PL_Spawn

 Parameters: Nothing (really?)

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void PL_Spawn( placeonplane_t location, LevelData_t *lvl )
{
	r_damageflash = 0;

	Player.position = location;

	Player.tilex = ((int)POS2TILE(location.origin[0]));
	Player.tiley = ((int)POS2TILE(location.origin[1]));
	Player.areanumber = lvl->areas[Player.tilex][Player.tiley];
	if (Player.areanumber < 0) {
		Player.areanumber = 36;
	}

	Areas_ConnectAreas( Player.areanumber );
}

/*
-----------------------------------------------------------------------------
 Function: Cmd_Give_f

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void Cmd_Give_f( void )
{
	char		*name;


	if( Cmd_Argc( ) < 2 ) {
		return;
	}

	name = Cmd_Argv( 1 );

	if( my_stricmp( name, "all" ) == 0 ) {
		PL_GiveHealth( &Player, 999, 0 );
		PL_GiveAmmo( &Player, AMMO_BULLETS, 99 );
		PL_GiveWeapon( &Player, WEAPON_AUTO );
		PL_GiveWeapon( &Player, WEAPON_CHAIN );
		PL_GiveKey( &Player, KEY_GOLD );
		PL_GiveKey( &Player, KEY_SILVER );
	}

}

/*
-----------------------------------------------------------------------------
 Function: Cmd_God_f

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void Cmd_God_f( void )
{
	Player.flags ^= FL_GODMODE;

	Com_Printf( "God mode %s\n", Player.flags & FL_GODMODE ? "ON":"OFF" );
}

/*
-----------------------------------------------------------------------------
 Function: PL_notarget_f

 Parameters: Nothing.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void PL_notarget_f( void )
{
	Player.flags ^= FL_NOTARGET;
}


/*
-----------------------------------------------------------------------------
 Function: PL_Init

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void PL_Init(void)
{
	PL_Reset(); /* Simply calls memset() for the 'Player' global variable. */
	PL_NewGame(&Player);

	Cmd_AddCommand("god", Cmd_God_f);
	Cmd_AddCommand("notarget", PL_notarget_f);

	Cmd_AddCommand("give", Cmd_Give_f);
}

/* ------------------------- * environment interraction * --------------------*/
#define EXTRAPOINTS 40000	/* points for an extra life */


/*
-----------------------------------------------------------------------------
 Function: PL_Damage

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void PL_Damage( player_t *self, entity_t *attacker, int points )
{
	if( self->playstate == ex_dead ) {
		return;
	}

	self->LastAttacker = attacker;

	if( self->flags & FL_GODMODE/* || gamestate.victoryflag FIXME*/) {
		return;
	}

	if( skill->value == gd_baby ) {
		points >>= 2;
	}


	self->health -= points;

	if( self->health <= 0 ) {
		self->health = 0;
		self->playstate = ex_dead;

		Sound_StartSound( NULL, 0, CHAN_BODY, Sound_RegisterSound( "lsfx/009.wav" ), 1, ATTN_NORM, 0 );
	}

	R_DamageFlash( points );
	Player.face_gotgun = false;

	/* make BJ's eyes bulge */
	if( points > 30 && Player.health != 0 ) {
		Player.face_ouch = true;
		Player.facecount = 0;
	}
}


/*
-----------------------------------------------------------------------------
 Function: PL_GiveHealth

 Parameters:

 Returns: returns true if player needs this health.

 Notes:
	gives player some HP
	max can be:
	 0 - natural player's health limit (100 or 150 with augment)
	>0 - indicates the limit
-----------------------------------------------------------------------------
*/
PUBLIC _boolean PL_GiveHealth( player_t *self, int points, int max )
{
	if( max == 0 ) {
		max = (self->items & ITEM_AUGMENT) ? 150 : 100;
	}

	if( self->health >= max ) {
		return false; /* does NOT need this health */
	}

	self->health += points;

	if( self->health > max ) {
		self->health = max;
	}

	Player.face_gotgun = false;

	return true; /* took it */
}

/*
-----------------------------------------------------------------------------
 Function: PL_GiveAmmo

 Parameters:

 Returns: returns true if player needs this ammo

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC _boolean PL_GiveAmmo( player_t *self, int type, int ammo )
{
	int max_ammo[ AMMO_TYPES ] = { 99 };
	int max;

	max = max_ammo[ type ];
	if( self->items & ITEM_BACKPACK ) {
		max *= 2;
	}

	if( self->ammo[ type ] >= max ) {
		return false; /* do NOT need */
	}

	if( ! self->ammo[ type ] && ! self->attackframe ) { /* knife was out */
		self->weapon = self->pendingweapon;
	}

	self->ammo[ type ] += ammo;
	if( self->ammo[ type ] > max ) {
		self->ammo[ type ] = max;
	}

	return true;
}

/*
-----------------------------------------------------------------------------
 Function: PL_GiveWeapon

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void PL_GiveWeapon(player_t *self, int weapon)
{
	unsigned itemflag;

	PL_GiveAmmo(self, AMMO_BULLETS, 6); /* give some ammo with a weapon */

	itemflag = ((unsigned int)ITEM_WEAPON_1 << weapon);
	if (self->items & itemflag) {
		return; /* player owns this weapon */
	} else {
		self->items |= itemflag;
		self->weapon = self->pendingweapon = weapon;
	}
}

/*
-----------------------------------------------------------------------------
 Function: PL_GiveLife

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void PL_GiveLife( player_t *self )
{
	if( self->lives < 9 ) {
		self->lives++;
	}

	Sound_StartSound( NULL, 0, CHAN_ITEM, Sound_RegisterSound( "lsfx/044.wav" ), 1, ATTN_NORM, 0 );
}


/*
-----------------------------------------------------------------------------
 Function: PL_GivePoints

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void PL_GivePoints( player_t *self, W32 points )
{
	self->score += points;
	while( self->score >= self->next_extra ) {
		self->next_extra += EXTRAPOINTS;
		PL_GiveLife( self );
	}
}

/*
-----------------------------------------------------------------------------
 Function: PL_GiveKey

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void PL_GiveKey( player_t *self, int key )
{
	self->items |= ITEM_KEY_1 << key;
}


/*
-----------------------------------------------------------------------------
 Function: PL_NewGame - Set up player for the new game

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void PL_NewGame( player_t *self )
{
	memset( self, 0, sizeof( player_t ) );

	self->health = 100;
	self->ammo[ AMMO_BULLETS ] = 8;
	self->lives = 3;

	self->weapon = self->pendingweapon = WEAPON_PISTOL;
	self->items = ITEM_WEAPON_1 | ITEM_WEAPON_2;
	self->next_extra = EXTRAPOINTS;
}

/*
-----------------------------------------------------------------------------
 Function: PL_NextLevel - Set up player for level transition

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void PL_NextLevel(player_t *self)
{
	self->old_score = self->score;
	self->attackcount = self->attackframe = self->weaponframe = 0;
	self->flags = 0;

	self->items &= (unsigned)~(ITEM_KEY_1 | ITEM_KEY_2 | ITEM_KEY_3 | ITEM_KEY_4);
}

/*
-----------------------------------------------------------------------------
 Function: PL_Reborn

 Parameters: self -[in] Player to respawn in game world.

 Returns: returns false if no lives left

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC _boolean PL_Reborn( player_t *self )
{
	if( --self->lives < 1 ) {
		return false;
	}


	self->health = 100;
	self->ammo[ AMMO_BULLETS ] = 8;
	self->score = self->old_score;
	self->attackcount = 0;
	self->attackframe = 0;
	self->weaponframe = 0;
	self->flags = 0;

	self->weapon = self->pendingweapon = WEAPON_PISTOL;
	self->items |= ITEM_WEAPON_1 | ITEM_WEAPON_2;

	self->playstate = ex_playing;

	return true;
}

/* EOF */
