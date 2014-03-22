/*
 * episode.c
 */

#include <stdlib.h>
#include <string.h>

#include "episode.h"

#include "../../../script/script.h"
#include "../../../common/ttree.h"


#include "../../../device/timer/timer.h"

typedef struct mapLink_s
{
	char *filename;		/* map file name */
	char *secretDef;	/* secret level mapDef id */
	char *nextDef;		/* next level mapDef id */
	W32 floornum;		/* floor number */

} mapLink_t;


typedef struct episode_s
{
	char *episodename;	/* Episode identification string */

	char *rootMapDef;	/* root map */

} episode_t;


PRIVATE episode_t *currentepisode = NULL;
PRIVATE mapLink_t *currentMap = NULL;


PRIVATE _boolean bRoot;

PRIVATE ttree_t *maptree = NULL;		/* map tree */


#define MAP_DEF		"mapDef"

#define MAP_FNAME	"filename"
#define MAP_SECRET	"secretlevel"
#define	MAP_NEXT	"nextlevel"
#define	MAP_FLOOR	"floor-number"


PRIVATE W8 *parse_maptag(W8 *in, W32 *linenumber)
{
	_boolean bfilename = false;
	_boolean bfloornum = false;
	W32 lg;
	W8 *ptr;
	mapLink_t *newMap;
	mapLink_t *oldMap;
	char *szMapDef;


	newMap = Z_TagMalloc(sizeof(mapLink_t), TAG_LEVEL_SCP);

	memset(newMap, 0, sizeof(mapLink_t));


	ptr = in;

	ptr = script_ignoreWhiteSpace(ptr);

	ptr = script_ReadString(ptr, (W8**)&szMapDef);

	ptr = script_lookforCharacter(ptr, '{', 0);

	if (*ptr == ENDOFSTREAM) {

		return ptr;
	}

	ptr++;

	while ((*ptr != ENDOFSTREAM) && (*ptr != '}')) {
		ptr = script_ignoreWhiteSpace(ptr);

		lg = strlen(MAP_FNAME);
		if (! strncmp((const char *)ptr, MAP_FNAME, lg)) {
			ptr += lg;

			ptr = script_ignoreWhiteSpace(ptr);

			if (newMap->filename) {
				Com_Printf("Error on line (%d) <filename> has already been defined!\n",
						   *linenumber);

				continue;
			}

			ptr = script_ReadQuoteString(ptr, (W8**)&newMap->filename);


			bfilename = true;

			continue;
		}

		lg = strlen(MAP_SECRET);
		if (! strncmp((const char *)ptr, MAP_SECRET, lg)) {
			ptr += lg;

			ptr = script_ignoreWhiteSpace(ptr);

			if (newMap->secretDef) {
				Com_Printf("Error on line (%d) <secretlevel> id has already been defined!\n",
						   *linenumber);

				continue;
			}

			ptr = script_ReadString(ptr, (W8**)&newMap->secretDef);

			continue;
		}

		lg = strlen(MAP_NEXT);
		if (! strncmp((const char *)ptr, MAP_NEXT, lg)) {
			ptr += lg;

			ptr = script_ignoreWhiteSpace(ptr);

			if (newMap->nextDef) {
				Com_Printf("Error on line (%d) <nextlevel> id has already been defined!\n",
						   *linenumber);

				continue;
			}

			ptr = script_ReadString(ptr, (W8**)&newMap->nextDef);


			continue;
		}

		lg = strlen(MAP_FLOOR);
		if (! strncmp((const char *)ptr, MAP_FLOOR, lg)) {
			decimalType_t dtNum;
			ptr += lg;

			ptr = script_ignoreWhiteSpace(ptr);

			if (bfloornum) {
				Com_Printf("Error on line (%d) <floornum> id has already been defined!\n",
						   *linenumber);

				continue;
			}

			ptr = script_ReadNumber(ptr, 0, &dtNum);

			newMap->floornum = (W32)dtNum.Integer;

			bfloornum = true;

			continue;
		}

		ptr = script_ignoreString(ptr);

	}

	if (*ptr == '}') {
		ptr++;
	}

	if (! bfilename || ! bfloornum) {
		Z_Free(newMap);

		return ptr;
	}

	if (! bRoot) {
		currentepisode->rootMapDef = szMapDef;
		bRoot = true;
	}

	ternary_tree_add((unsigned char *)szMapDef, (void *)newMap, maptree, 0,
					 (void **)&oldMap);


	return ptr;
}


PRIVATE W8 *parse_episodetag(W8 *in, W32 *linenumber)
{
	W32 lg;
	W8 *ptr = in;
	episode_t *newEpisode;
	episode_t *oldEpisode;


	bRoot = false;


	newEpisode = Z_TagMalloc(sizeof(episode_t), TAG_LEVEL_SCP);


	currentepisode = newEpisode;


	ptr = script_ignoreWhiteSpace(ptr);

	ptr = script_ReadString(ptr, (W8**)&newEpisode->episodename);

	ptr = script_lookforCharacter(ptr, '{', 0);

	if (*ptr == ENDOFSTREAM) {
		Z_Free(newEpisode);

		return ptr;
	}

	ptr++;

	while ((*ptr != ENDOFSTREAM) && (*ptr != '}')) {
		ptr = script_ignoreWhiteSpace(ptr);

		lg = strlen(MAP_DEF);
		if (! strncmp((const char *)ptr, MAP_DEF, lg)) {
			ptr += lg;

			ptr = parse_maptag(ptr, linenumber);
		}

		ptr = script_ignoreString(ptr);
	}

	if (*ptr == '}') {
		ptr++;
	}

	ternary_tree_add((unsigned char *)newEpisode->episodename,
					 (void *)newEpisode, maptree, 0, (void **)&oldEpisode);

	return ptr;
}


PRIVATE Tag_Property_t episode_tags[] =
{
	/* casting here to avoid a warning leads to an error: */
	{ "episodeDef", parse_episodetag }
};


PUBLIC _boolean episode_init(const char *filename)
{
	W32 startime, endtime;

	if (! filename || ! *filename) {
		return false;
	}

	Com_Printf("episode_init(): Processing script (%s)\n", filename);

	maptree = ternary_tree_init(10);

	startime = Sys_Milliseconds();

	if (! script_Parse(filename, episode_tags,
					   (sizeof(episode_tags) / sizeof(episode_tags[0])),
					   TAG_LEVEL_SCP)) {
		Com_Printf("episode_init(): Unable to open script (%s)\n", filename);

		episode_shutdown();

		return false;
	}

	endtime = Sys_Milliseconds();

	Com_Printf("script parsed in %d ms\n", (endtime - startime));


	currentMap = (mapLink_t *)ternary_tree_find((unsigned char *)currentepisode->rootMapDef,
												maptree);

	return true;
}

PUBLIC void episode_shutdown(void)
{
	ternary_tree_cleanup(maptree);
	maptree = NULL;

	Z_FreeTags(TAG_LEVEL_SCP);
}



PUBLIC _boolean episode_setEpisode(char *nameDef)
{
	currentepisode = (episode_t *)ternary_tree_find((unsigned char *)nameDef,
													maptree);

	if (! currentepisode) {
		return false;
	}

	currentMap = (mapLink_t *)ternary_tree_find((unsigned char *)currentepisode->rootMapDef,
												maptree);

	return true;
}


PUBLIC char *episode_getCurrentMapFileName(W32 *floornum)
{
	if (! currentMap) {
		*floornum = 0;
		return NULL;
	}

	*floornum = currentMap->floornum;

	return currentMap->filename;
}

PUBLIC char *episode_getNextMapName(W32 *floornum)
{
	if (! currentMap) {
		*floornum = 0;

		return NULL;
	}

	currentMap = (mapLink_t *)ternary_tree_find((unsigned char *)currentMap->nextDef,
												maptree);

	if (! currentMap) {
		*floornum = 0;

		return NULL;
	}

	*floornum = currentMap->floornum;

	return currentMap->filename;
}

PUBLIC char *episode_getNextSecretMapName(W32 *floornum)
{
	if (! currentMap) {
		*floornum = 0;

		return NULL;
	}

	currentMap = (mapLink_t *)ternary_tree_find((unsigned char *)currentMap->secretDef,
												maptree);

	if (! currentMap) {
		*floornum = 0;

		return NULL;
	}

	*floornum = currentMap->floornum;

	return currentMap->filename;
}

/* EOF */
