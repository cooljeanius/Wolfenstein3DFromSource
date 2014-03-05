/*
 * wolf_menu.h
 */

#ifndef __WOLF_MENU_H__
#define __WOLF_MENU_H__

#include "../../../common/arch.h"
#include "../../../common/common_utils.h"
#include "../../../video/video.h"

#include "../../../client/menu_conf.h"

/* most of these prototypes are for functions from wolf_menu.c */

extern colour3_t bgcolour;
extern colour3_t bord2colour;
extern colour3_t deactive;
extern colour3_t bkgdcolour;

extern colour3_t readcolour;
extern colour3_t readhcolour;
extern colour3_t textcolour;
extern colour3_t highlight;


extern colour3_t bannerline;

extern colour3_t interbkgnd;


extern colour3_t sodbgcolour;

extern colour3_t sodbord2colour;
extern colour3_t soddeactive;
extern colour3_t sodbkgdcolour;


extern const char *menu_in_sound;
extern const char *menu_move_sound;
extern const char *menu_out_sound;

extern void M_PopMenu( void );
extern void M_ForceMenuOff( void );
extern void M_PushMenu( void (*draw) (void), const char *(*key) (int k) );

#define NUM_CURSOR_FRAMES 2
extern void M_DrawCursor( int x, int y, int f );
extern void M_BannerString( const char *string, W16 nYOffset );
extern void M_Banner( const char *name, W16 nYOffest );
extern void M_DrawWindow(int x, int y, int w, int h, colour3_t bg,
						 colour3_t act, colour3_t deact );
extern void M_DrawInfoBar( void );

extern const char *Default_MenuKey( menuframework_s *m, int key );

extern void M_DrawCharacter( int cx, int cy, int num );
extern void M_Print( int cx, int cy, const char *str );
extern void M_PrintWhite( int cx, int cy, const char *str );
extern void M_DrawPic( int x, int y, const char *pic );
extern void M_DrawTextBox (int x, int y, int width, int lines);
extern void M_Main_Draw( void );
extern const char *M_Main_Key( int key );
extern void M_SMain_Draw( void );
extern void Create_Savestrings( void );
extern void LoadGameCallback( void *self );
extern void LoadGame_MenuInit( void );
extern void LoadGame_MenuDraw( void );
extern const char *LoadGame_MenuKey( int key );
extern void SaveGameCallback( void *self );
extern void SaveGame_MenuDraw( void );
extern void SaveGame_MenuInit( void );
extern const char *SaveGame_MenuKey( int key );

/* prototypes for functions from wolf_mcredits.c: */
extern void M_Menu_Credits_f( void );

/* prototypes for functions from wolf_mintro.c: */
extern void M_Intro_f( void );

/* prototypes for functions from wolf_mintermission.c: */
extern void M_Intermission_f( void );

/* prototypes for functions from wolf_moptions.c: */
extern void M_Menu_Options_f( void );

/* prototypes for functions from wolf_mnewgame.c: */
extern void M_Menu_Game_f (void);

/* prototypes for functions from wolf_mquit.c: */
extern void M_Menu_Quit_f( void );

/* prototypes for functions from wolf_mvideo.c: */
extern void Video_MenuDraw( void );
extern const char *Video_MenuKey( int key );

/* prototypes for functions from wolf_msound.c: */
extern void M_Menu_Sound_f( void );

#endif /* __WOLF_MENU_H__ */

/* EOF */
