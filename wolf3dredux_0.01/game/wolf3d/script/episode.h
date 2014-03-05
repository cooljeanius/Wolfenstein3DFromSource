/*
 * episode.h
 */

#ifndef __EPISODE_H__
#define __EPISODE_H__

#include "../../../common/arch.h"

extern _boolean episode_init( const char *filename );
extern void episode_shutdown( void );

extern _boolean episode_setEpisode( char *nameDef );
extern char *episode_getCurrentMapFileName( W32 *floornum );
extern char *episode_getNextMapName( W32 *floornum );
extern char *episode_getNextSecretMapName( W32 *floornum );


#endif /* __EPISODE_H__ */

/* EOF */
