/*
 *  wolf_client.h
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/14/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 *  License: GPL-2+, like the rest of the source.
 *
 */

/*
 * 	wolf_client.h:  Wolfenstein3-D client.
 */

/*
 * 	Notes:
 * 	This module is implemented by wolf_client_main.c and wolf_sv_ccmds.c
 */

#ifndef __WOLF_CLIENT_H__
#define __WOLF_CLIENT_H__

/* prototypes from wolf_client_main.c: */
extern void Client_Screen_UpdateScreen(void);
extern void Client_PrepRefresh(const char *r_mapname);

/* prototypes from wolf_sv_ccmds.c: */
extern void Map_f(void);

#endif /* __WOLF_CLIENT_H__ */
