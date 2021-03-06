/*
 *	Copyright (C) 2004 Michael Liebscher
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
 *	wolf_moptions.c:   Options menu.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <string.h>


#include "wolf_menu.h"

#include "../wolf_local.h"

#include "../../../common/cvar.h"
#include "../../../client/client.h"
#include "../../../string/com_string.h"
#include "../../../device/timer/timer.h"
#include "../../../client/console.h"
#include "../../../video/renderer.h"



/*******************************************************************
 *
 *	OPTIONS MENU
 *
 *******************************************************************/

extern void M_Menu_Keys_f(void);

extern cvar_t *in_joystick;

PRIVATE cvar_t *win_noalttab;



PRIVATE menuframework_s	s_options_menu;
PRIVATE menuaction_s	s_options_defaults_action;
PRIVATE menuaction_s	s_options_customize_options_action;
PRIVATE menuslider_s	s_options_sensitivity_slider;
PRIVATE menulist_s		s_options_freelook_box;
PRIVATE menulist_s		s_options_noalttab_box;
PRIVATE menulist_s		s_options_alwaysrun_box;
PRIVATE menulist_s		s_options_invertmouse_box;
PRIVATE menulist_s		s_options_lookspring_box;
PRIVATE menulist_s		s_options_lookstrafe_box;
PRIVATE menulist_s		s_options_joystick_box;
PRIVATE menulist_s		s_options_console_action;

/* is the 'unused' parameter really necessary? */
PRIVATE void JoystickFunc(void *unused)
{
	Cvar_SetValue("in_joystick", (float)s_options_joystick_box.curvalue);
}

/* is the 'unused' parameter really necessary? */
PRIVATE void CustomizeControlsFunc(void *unused)
{
	M_Menu_Keys_f();
}

/* is the 'unused' parameter really necessary? */
PRIVATE void AlwaysRunFunc(void *unused)
{
	Cvar_SetValue("cl_run", (float)s_options_alwaysrun_box.curvalue);
}

/* is the 'unused' parameter really necessary? */
PRIVATE void FreeLookFunc(void *unused)
{
	Cvar_SetValue("freelook", (float)s_options_freelook_box.curvalue);
}

/* is the 'unused' parameter really necessary? */
PRIVATE void MouseSpeedFunc(void *unused)
{
	Cvar_SetValue("m_sensitivity",
				  (s_options_sensitivity_slider.curvalue * 100));
}

/* is the 'unused' parameter really necessary? */
PRIVATE void NoAltTabFunc(void *unused)
{
	Cvar_SetValue("win_noalttab", (float)s_options_noalttab_box.curvalue);
}

PRIVATE float ClampCvar(float min, float max, float value)
{
	if (value < min) {
		return min;
	}

	if (value > max) {
		return max;
	}


	return value;
}

PRIVATE void ControlsSetMenuItemValues(void)
{
	s_options_sensitivity_slider.curvalue	= (sensitivity->value / 100);

	Cvar_SetValue("cl_run", ClampCvar(0, 1, cl_run->value));
	s_options_alwaysrun_box.curvalue		= (int)cl_run->value;

	s_options_invertmouse_box.curvalue		= (m_pitch->value < 0);

	Cvar_SetValue("lookspring", ClampCvar(0, 1, lookspring->value));
	s_options_lookspring_box.curvalue		= (int)lookspring->value;

	Cvar_SetValue("lookstrafe", ClampCvar(0, 1, lookstrafe->value));
	s_options_lookstrafe_box.curvalue		= (int)lookstrafe->value;

	Cvar_SetValue("freelook", ClampCvar(0, 1, freelook->value));
	s_options_freelook_box.curvalue			= (int)freelook->value;


	Cvar_SetValue("in_joystick", ClampCvar(0, 1, in_joystick->value));
	s_options_joystick_box.curvalue		= (int)in_joystick->value;

#if 0 || __clang_analyzer__
	s_options_noalttab_box.curvalue			= (int)win_noalttab->value;
#endif /* 0 || __clang_analyzer__ */

}

/* is the 'unused' parameter really necessary? */
PRIVATE void ControlsResetDefaultsFunc(void *unused)
{
	Cbuf_AddText("exec DEFAULT.CFG\n");
	Cbuf_Execute();

	ControlsSetMenuItemValues();
}

/* is the 'unused' parameter really necessary? */
PRIVATE void InvertMouseFunc( void *unused )
{
	Cvar_SetValue("m_pitch", -m_pitch->value);
}

/* is the 'unused' parameter really necessary? */
PRIVATE void LookspringFunc(void *unused)
{
	Cvar_SetValue("lookspring", !lookspring->value);
}

/* is the 'unused' parameter really necessary? */
PRIVATE void LookstrafeFunc(void *unused)
{
	Cvar_SetValue("lookstrafe", ! lookstrafe->value);
}

extern void Key_ClearTyping(void); /* moved out here for -Wnested-externs */

/* is the 'unused' parameter really necessary? */
PRIVATE void ConsoleFunc(void *unused)
{
	/*
	** the proper way to do this is probably to have ToggleConsole_f accept a parameter
	*/

#if 0
	if (cl.attractloop) { /* 'cl' is undeclared */
		Cbuf_AddText("killserver\n");
		return;
	}
#endif /* 0 */

	Key_ClearTyping();
	Con_ClearNotify();

	M_ForceMenuOff();
	ClientStatic.key_dest = key_console;
}



PRIVATE void Options_MenuInit(void)
{
	static const char *yesno_names[] =
	{
		"no",
		"yes",
		0
	};


	int nYOffset = 27;
	int y = 0;

	win_noalttab = Cvar_Get("win_noalttab", "0", CVAR_ARCHIVE);

	/*
	** configure controls menu and menu items
	*/
	s_options_menu.x = ((viddef.width >> 1) + 90);
	s_options_menu.y = ((viddef.height >> 1) - 110);
	s_options_menu.nitems = 0;


	s_options_sensitivity_slider.generic.type	= MTYPE_SLIDER;
	s_options_sensitivity_slider.generic.x		= 0;
	s_options_sensitivity_slider.generic.y		= y;
	s_options_sensitivity_slider.generic.fs		= FONT1;
	s_options_sensitivity_slider.generic.fontBaseColour = &textcolour;
	s_options_sensitivity_slider.generic.name	= "Mouse Speed:";
	s_options_sensitivity_slider.generic.callback = MouseSpeedFunc;
	s_options_sensitivity_slider.minvalue		= 1;
	s_options_sensitivity_slider.maxvalue		= 10;

	s_options_alwaysrun_box.generic.type = MTYPE_SPINCONTROL;
	s_options_alwaysrun_box.generic.x	= 0;
	s_options_alwaysrun_box.generic.y	= y += nYOffset;
	s_options_alwaysrun_box.generic.fs		= FONT1;
	s_options_alwaysrun_box.generic.fontBaseColour = &textcolour;
	s_options_alwaysrun_box.generic.fontHighColour = &readcolour;
	s_options_alwaysrun_box.generic.name	= "Always Run:";
	s_options_alwaysrun_box.generic.callback = AlwaysRunFunc;
	s_options_alwaysrun_box.itemnames = yesno_names;

	s_options_invertmouse_box.generic.type = MTYPE_SPINCONTROL;
	s_options_invertmouse_box.generic.x	= 0;
	s_options_invertmouse_box.generic.y	= y += nYOffset;
	s_options_invertmouse_box.generic.fs		= FONT1;
	s_options_invertmouse_box.generic.fontBaseColour = &textcolour;
	s_options_invertmouse_box.generic.fontHighColour = &readcolour;
	s_options_invertmouse_box.generic.name	= "Invert Mouse:";
	s_options_invertmouse_box.generic.callback = InvertMouseFunc;
	s_options_invertmouse_box.itemnames = yesno_names;

	s_options_lookspring_box.generic.type = MTYPE_SPINCONTROL;
	s_options_lookspring_box.generic.x	= 0;
	s_options_lookspring_box.generic.y	= y += nYOffset;
	s_options_lookspring_box.generic.fs		= FONT1;
	s_options_lookspring_box.generic.fontBaseColour = &textcolour;
	s_options_lookspring_box.generic.fontHighColour = &readcolour;
	s_options_lookspring_box.generic.name	= "Lookspring:";
	s_options_lookspring_box.generic.callback = LookspringFunc;
	s_options_lookspring_box.itemnames = yesno_names;

	s_options_lookstrafe_box.generic.type = MTYPE_SPINCONTROL;
	s_options_lookstrafe_box.generic.x	= 0;
	s_options_lookstrafe_box.generic.y	= y += nYOffset;
	s_options_lookstrafe_box.generic.fs		= FONT1;
	s_options_lookstrafe_box.generic.fontBaseColour = &textcolour;
	s_options_lookstrafe_box.generic.fontHighColour = &readcolour;
	s_options_lookstrafe_box.generic.name	= "Lookstrafe:";
	s_options_lookstrafe_box.generic.callback = LookstrafeFunc;
	s_options_lookstrafe_box.itemnames = yesno_names;

	s_options_freelook_box.generic.type = MTYPE_SPINCONTROL;
	s_options_freelook_box.generic.x	= 0;
	s_options_freelook_box.generic.y	= y += nYOffset;
	s_options_freelook_box.generic.fs		= FONT1;
	s_options_freelook_box.generic.fontBaseColour = &textcolour;
	s_options_freelook_box.generic.fontHighColour = &readcolour;
	s_options_freelook_box.generic.name	= "free look";
	s_options_freelook_box.generic.callback = FreeLookFunc;
	s_options_freelook_box.itemnames = yesno_names;

#if 0 || __clang_analyzer__
	s_options_noalttab_box.generic.type = MTYPE_SPINCONTROL;
	s_options_noalttab_box.generic.x	= 0;
	s_options_noalttab_box.generic.y	= y += nYOffset;
	s_options_noalttab_box.generic.name	= "disable alt-tab";
	s_options_noalttab_box.generic.callback = NoAltTabFunc;
	s_options_noalttab_box.itemnames = yesno_names;
#endif /* 0 || __clang_analyzer__ */
	s_options_joystick_box.generic.type = MTYPE_SPINCONTROL;
	s_options_joystick_box.generic.x	= 0;
	s_options_joystick_box.generic.y	= y += nYOffset;
	s_options_joystick_box.generic.fs		= FONT1;
	s_options_joystick_box.generic.fontBaseColour = &textcolour;
	s_options_joystick_box.generic.fontHighColour = &readcolour;
	s_options_joystick_box.generic.name	= "Use Joystick:";
	s_options_joystick_box.generic.callback = JoystickFunc;
	s_options_joystick_box.itemnames = yesno_names;

	s_options_customize_options_action.generic.type	= MTYPE_ACTION;
	s_options_customize_options_action.generic.x	= 0;
	s_options_customize_options_action.generic.y	= y += nYOffset;
	s_options_customize_options_action.generic.fs	= FONT1;
	s_options_customize_options_action.generic.fontBaseColour = &textcolour;
	s_options_customize_options_action.generic.fontHighColour = &highlight;
	s_options_customize_options_action.generic.name	= "Customize Controls";
	s_options_customize_options_action.generic.callback = CustomizeControlsFunc;

	s_options_defaults_action.generic.type	= MTYPE_ACTION;
	s_options_defaults_action.generic.x		= 0;
	s_options_defaults_action.generic.y		= y += nYOffset;
	s_options_defaults_action.generic.fs		= FONT1;
	s_options_defaults_action.generic.fontBaseColour = &textcolour;
	s_options_defaults_action.generic.fontHighColour = &highlight;
	s_options_defaults_action.generic.name	= "Reset Defaults";
	s_options_defaults_action.generic.callback = ControlsResetDefaultsFunc;

	s_options_console_action.generic.type	= MTYPE_ACTION;
	s_options_console_action.generic.x		= 0;
	s_options_console_action.generic.y		= (y += nYOffset);
	/* dummy to silence clang static analyzer warning about value stored to
	 * 'y' never actually being read: */
	if (s_options_console_action.generic.y == y) {
		;
	}
	s_options_console_action.generic.fs		= FONT1;
	s_options_console_action.generic.fontBaseColour = &textcolour;
	s_options_console_action.generic.fontHighColour = &highlight;
	s_options_console_action.generic.name	= "Go to Console";
	s_options_console_action.generic.callback = ConsoleFunc;

	ControlsSetMenuItemValues();


	Menu_AddItem(&s_options_menu, (void *)&s_options_sensitivity_slider);
	Menu_AddItem(&s_options_menu, (void *)&s_options_alwaysrun_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_invertmouse_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_lookspring_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_lookstrafe_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_freelook_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_joystick_box);
	Menu_AddItem(&s_options_menu, (void *)&s_options_customize_options_action);
	Menu_AddItem(&s_options_menu, (void *)&s_options_defaults_action);
	Menu_AddItem(&s_options_menu, (void *)&s_options_console_action);
}

/* */
PRIVATE void Options_MenuDraw(void)
{
	if (g_version->value == SPEAROFDESTINY) {
		R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height,
					"pics/C_BACKDROPPIC.tga");
		M_Banner("pics/SC_CONTROLPIC.tga", (W16)0);
		M_DrawWindow((int)((viddef.width - 550) >> 1),
					 (int)(((viddef.height - 335) >> 1) + 40), 550, 335,
					 sodbkgdcolour, sodbord2colour, soddeactive);
	} else {
		R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, bgcolour);

		M_Banner("pics/C_CONTROLPIC.tga", (W16)0);
		M_DrawWindow((int)((viddef.width - 550) >> 1),
					 (int)(((viddef.height - 335) >> 1) + 40), 550, 335,
					 bkgdcolour, bord2colour, deactive);
	}

	M_DrawInfoBar();
	Menu_AdjustCursor(&s_options_menu, 1);
	Menu_Draw(&s_options_menu);
}

/* */
PRIVATE const char *Options_MenuKey(int key)
{
	return Default_MenuKey(&s_options_menu, key);
}

/* */
PUBLIC void M_Menu_Options_f(void)
{
	Options_MenuInit();
	M_PushMenu ( Options_MenuDraw, Options_MenuKey );
}



/********************************************************************
 *
 *	CUSTOMIZE KEYS MENU
 *
 *******************************************************************/

/* Is this array correct? */
char *bindnames[][2] =
{
	{"+attack", 		"Attack"},
	{"weapnext", 		"Next Weapon"},
	{"weapprev",		"Prev Weapon"},
	{"+forward", 		"Walk Forward"},
	{"+back", 			"Backpedal"},
	{"+left", 			"Turn Left"},
	{"+right", 			"Turn Right"},
	{"+speed", 			"Run / Walk"},
	{"+moveleft", 		"Step Left"},
	{"+moveright", 		"Step Right"},
	{"+strafe", 		"Sidestep"},
	{"+mlook", 			"Mouse Look"},
	{"+use", 			"Use"},

	{ 0, 0 }
};

int				keys_cursor;
PRIVATE int		bind_grab;

PRIVATE menuframework_s	s_keys_menu;
PRIVATE menuaction_s	s_keys_attack_action;
PRIVATE menuaction_s	s_keys_change_weapon_action;
PRIVATE menuaction_s	s_keys_prev_weapon_action;
PRIVATE menuaction_s	s_keys_walk_forward_action;
PRIVATE menuaction_s	s_keys_backpedal_action;
PRIVATE menuaction_s	s_keys_turn_left_action;
PRIVATE menuaction_s	s_keys_turn_right_action;
PRIVATE menuaction_s	s_keys_run_action;
PRIVATE menuaction_s	s_keys_step_left_action;
PRIVATE menuaction_s	s_keys_step_right_action;
PRIVATE menuaction_s	s_keys_sidestep_action;
PRIVATE menuaction_s	s_keys_mouse_look_action;
PRIVATE menuaction_s	s_keys_use_action;
PRIVATE menuaction_s	s_keys_move_up_action;
PRIVATE menuaction_s	s_keys_move_down_action;



PRIVATE void M_UnbindCommand(char *command)
{
	int		j;
	int		length;
	char	*b;

	length = (int)strlen(command);

	for ((j = 0); (j < 256); ++j) {
		b = keybindings[j];
		if (! b) {
			continue;
		}

		if (! strncmp(b, command, (size_t)length)) {
			Key_SetBinding(j, "");
		}
	}
}

PRIVATE void M_FindKeysForCommand(const char *command, int *twokeys)
{
	int		count;
	int		j;
	int		length;
	char	*b;

	twokeys[0] = twokeys[1] = -1;
	length = (int)strlen(command);
	count = 0;

	for ((j = 0); (j < 256); ++j) {
		b = keybindings[j];
		if (! b) {
			continue;
		}

		if (! strncmp(b, command, (size_t)length)) {
			twokeys[count] = j;
			count++;
			if (count == 2) {
				break;
			}
		}
	}
}

/* */
PRIVATE void KeyCursorDrawFunc(menuframework_s *menu)
{
	static const char ccursor[2] = { ' ', '>' };

	if (bind_grab) {
		Font_put_character(FONT0, (int)menu->x,
						   (int)(menu->y + (menu->cursor * 20)), (W16)'=');
	} else {
		Font_put_character(FONT0, (int)menu->x, (menu->y + (menu->cursor * 20)),
						   (W16)ccursor[(0 + ((int)(Sys_Milliseconds() / 250) & 1))]);
	}
}

/* */
PRIVATE void DrawKeyBindingFunc(void *self)
{
	int keys[2];
	menuaction_s *a = (menuaction_s *)self;

	Font_SetSize(a->generic.fs, (W16)2);
	Font_SetColour(a->generic.fs, *a->generic.fontBaseColour);

	M_FindKeysForCommand(bindnames[a->generic.localdata[0]][0], keys);

	if (keys[0] == -1) {
		Font_put_line(a->generic.fs, (a->generic.x + a->generic.parent->x + 30),
					  (a->generic.y + a->generic.parent->y), "???");
	} else {
		char name[128];

		my_strlcpy(name, Key_KeynumToString(keys[0]), sizeof(name));

		if (keys[1] != -1) {
			my_strlcat(name, " or ", sizeof(name));
			my_strlcat(name, Key_KeynumToString(keys[1]), sizeof(name));
		}

		Font_put_line(a->generic.fs, (a->generic.x + a->generic.parent->x + 30),
					  (a->generic.y + a->generic.parent->y), name);
	}

}

PRIVATE void KeyBindingFunc(void *self)
{
	menuaction_s *a = (menuaction_s *)self;
	int keys[2];

	M_FindKeysForCommand(bindnames[a->generic.localdata[0]][0], keys);

	if (keys[1] != -1) {
		M_UnbindCommand( bindnames[a->generic.localdata[0]][0]);
	}

	bind_grab = true;

	Menu_SetStatusBar(&s_keys_menu, "Press a key or button for this action");
}

PRIVATE void Keys_MenuInit(void)
{
	int y = 0;
	int i = 0;
	int nYOffset = 20;

	s_keys_menu.x = (int)(viddef.width * 0.40);
	s_keys_menu.nitems = 0;
	s_keys_menu.cursordraw = KeyCursorDrawFunc;

	s_keys_attack_action.generic.type	= MTYPE_ACTION;
	s_keys_attack_action.generic.flags  = MF_GRAYED;
	s_keys_attack_action.generic.x		= 0;
	s_keys_attack_action.generic.y		= y;
	s_keys_attack_action.generic.fs		= FONT0;
	s_keys_attack_action.generic.fontBaseColour = &textcolour;
	s_keys_attack_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_attack_action.generic.localdata[0] = i;
	s_keys_attack_action.generic.name	= bindnames[s_keys_attack_action.generic.localdata[0]][1];

	s_keys_change_weapon_action.generic.type	= MTYPE_ACTION;
	s_keys_change_weapon_action.generic.flags  = MF_GRAYED;
	s_keys_change_weapon_action.generic.x		= 0;
	s_keys_change_weapon_action.generic.y		= y += nYOffset;
	s_keys_change_weapon_action.generic.fs		= FONT0;
	s_keys_change_weapon_action.generic.fontBaseColour = &textcolour;
	s_keys_change_weapon_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_change_weapon_action.generic.localdata[0] = ++i;
	s_keys_change_weapon_action.generic.name	= bindnames[s_keys_change_weapon_action.generic.localdata[0]][1];

	s_keys_prev_weapon_action.generic.type	= MTYPE_ACTION;
	s_keys_prev_weapon_action.generic.flags  = MF_GRAYED;
	s_keys_prev_weapon_action.generic.x		= 0;
	s_keys_prev_weapon_action.generic.y		= y += nYOffset;
	s_keys_prev_weapon_action.generic.fs		= FONT0;
	s_keys_prev_weapon_action.generic.fontBaseColour = &textcolour;
	s_keys_prev_weapon_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_prev_weapon_action.generic.localdata[0] = ++i;
	s_keys_prev_weapon_action.generic.name	= bindnames[ s_keys_prev_weapon_action.generic.localdata[0] ][1];

	s_keys_walk_forward_action.generic.type	= MTYPE_ACTION;
	s_keys_walk_forward_action.generic.flags  = MF_GRAYED;
	s_keys_walk_forward_action.generic.x		= 0;
	s_keys_walk_forward_action.generic.y		= y += nYOffset;
	s_keys_walk_forward_action.generic.fs		= FONT0;
	s_keys_walk_forward_action.generic.fontBaseColour = &textcolour;
	s_keys_walk_forward_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_walk_forward_action.generic.localdata[0] = ++i;
	s_keys_walk_forward_action.generic.name	= bindnames[s_keys_walk_forward_action.generic.localdata[0]][1];

	s_keys_backpedal_action.generic.type	= MTYPE_ACTION;
	s_keys_backpedal_action.generic.flags  = MF_GRAYED;
	s_keys_backpedal_action.generic.x		= 0;
	s_keys_backpedal_action.generic.y		= y += nYOffset;
	s_keys_backpedal_action.generic.fs		= FONT0;
	s_keys_backpedal_action.generic.fontBaseColour = &textcolour;
	s_keys_backpedal_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_backpedal_action.generic.localdata[0] = ++i;
	s_keys_backpedal_action.generic.name	= bindnames[s_keys_backpedal_action.generic.localdata[0]][1];

	s_keys_turn_left_action.generic.type	= MTYPE_ACTION;
	s_keys_turn_left_action.generic.flags  = MF_GRAYED;
	s_keys_turn_left_action.generic.x		= 0;
	s_keys_turn_left_action.generic.y		= y += nYOffset;
	s_keys_turn_left_action.generic.fs		= FONT0;
	s_keys_turn_left_action.generic.fontBaseColour = &textcolour;
	s_keys_turn_left_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_turn_left_action.generic.localdata[0] = ++i;
	s_keys_turn_left_action.generic.name	= bindnames[s_keys_turn_left_action.generic.localdata[0]][1];

	s_keys_turn_right_action.generic.type	= MTYPE_ACTION;
	s_keys_turn_right_action.generic.flags  = MF_GRAYED;
	s_keys_turn_right_action.generic.x		= 0;
	s_keys_turn_right_action.generic.y		= y += nYOffset;
	s_keys_turn_right_action.generic.fs		= FONT0;
	s_keys_turn_right_action.generic.fontBaseColour = &textcolour;
	s_keys_turn_right_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_turn_right_action.generic.localdata[0] = ++i;
	s_keys_turn_right_action.generic.name	= bindnames[s_keys_turn_right_action.generic.localdata[0]][1];

	s_keys_run_action.generic.type	= MTYPE_ACTION;
	s_keys_run_action.generic.flags  = MF_GRAYED;
	s_keys_run_action.generic.x		= 0;
	s_keys_run_action.generic.y		= y += nYOffset;
	s_keys_run_action.generic.fs		= FONT0;
	s_keys_run_action.generic.fontBaseColour = &textcolour;
	s_keys_run_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_run_action.generic.localdata[0] = ++i;
	s_keys_run_action.generic.name	= bindnames[s_keys_run_action.generic.localdata[0]][1];

	s_keys_step_left_action.generic.type	= MTYPE_ACTION;
	s_keys_step_left_action.generic.flags  = MF_GRAYED;
	s_keys_step_left_action.generic.x		= 0;
	s_keys_step_left_action.generic.y		= y += nYOffset;
	s_keys_step_left_action.generic.fs		= FONT0;
	s_keys_step_left_action.generic.fontBaseColour = &textcolour;
	s_keys_step_left_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_step_left_action.generic.localdata[0] = ++i;
	s_keys_step_left_action.generic.name	= bindnames[s_keys_step_left_action.generic.localdata[0]][1];

	s_keys_step_right_action.generic.type	= MTYPE_ACTION;
	s_keys_step_right_action.generic.flags  = MF_GRAYED;
	s_keys_step_right_action.generic.x		= 0;
	s_keys_step_right_action.generic.y		= y += nYOffset;
	s_keys_step_right_action.generic.fs		= FONT0;
	s_keys_step_right_action.generic.fontBaseColour = &textcolour;
	s_keys_step_right_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_step_right_action.generic.localdata[0] = ++i;
	s_keys_step_right_action.generic.name	= bindnames[s_keys_step_right_action.generic.localdata[0]][1];

	s_keys_sidestep_action.generic.type	= MTYPE_ACTION;
	s_keys_sidestep_action.generic.flags  = MF_GRAYED;
	s_keys_sidestep_action.generic.x		= 0;
	s_keys_sidestep_action.generic.y		= y += nYOffset;
	s_keys_sidestep_action.generic.fs		= FONT0;
	s_keys_sidestep_action.generic.fontBaseColour = &textcolour;
	s_keys_sidestep_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_sidestep_action.generic.localdata[0] = ++i;
	s_keys_sidestep_action.generic.name	= bindnames[s_keys_sidestep_action.generic.localdata[0]][1];

	s_keys_mouse_look_action.generic.type	= MTYPE_ACTION;
	s_keys_mouse_look_action.generic.flags  = MF_GRAYED;
	s_keys_mouse_look_action.generic.x		= 0;
	s_keys_mouse_look_action.generic.y		= y += nYOffset;
	s_keys_mouse_look_action.generic.fs		= FONT0;
	s_keys_mouse_look_action.generic.fontBaseColour = &textcolour;
	s_keys_mouse_look_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_mouse_look_action.generic.localdata[0] = ++i;
	s_keys_mouse_look_action.generic.name	= bindnames[s_keys_mouse_look_action.generic.localdata[0]][1];

	s_keys_use_action.generic.type	= MTYPE_ACTION;
	s_keys_use_action.generic.flags  = MF_GRAYED;
	s_keys_use_action.generic.x		= 0;
	s_keys_use_action.generic.y		= y += nYOffset;
	s_keys_use_action.generic.fs		= FONT0;
	s_keys_use_action.generic.fontBaseColour = &textcolour;
	s_keys_use_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_use_action.generic.localdata[0] = ++i;
	s_keys_use_action.generic.name	= bindnames[s_keys_use_action.generic.localdata[0]][1];

	s_keys_move_up_action.generic.type	= MTYPE_ACTION;
	s_keys_move_up_action.generic.flags  = MF_GRAYED;
	s_keys_move_up_action.generic.x		= 0;
	s_keys_move_up_action.generic.y		= y += nYOffset;
	s_keys_move_up_action.generic.fs		= FONT0;
	s_keys_move_up_action.generic.fontBaseColour = &textcolour;
	s_keys_move_up_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_move_up_action.generic.localdata[0] = ++i;
	s_keys_move_up_action.generic.name	= bindnames[s_keys_move_up_action.generic.localdata[0]][1];

	s_keys_move_down_action.generic.type	= MTYPE_ACTION;
	s_keys_move_down_action.generic.flags  = MF_GRAYED;
	s_keys_move_down_action.generic.x		= 0;
	s_keys_move_down_action.generic.y		= (y += nYOffset);
	/* dummy to silence clang static analyzer warning about value stored to
	 * 'y' never actually being read: */
	if (s_keys_move_down_action.generic.y == y) {
		;
	}
	s_keys_move_down_action.generic.fontBaseColour = &textcolour;
	s_keys_move_down_action.generic.ownerdraw = DrawKeyBindingFunc;
	s_keys_move_down_action.generic.ownerdraw = DrawKeyBindingFunc;
	if ((size_t)s_keys_move_down_action.generic.localdata[0] < sizeof(bindnames))
		s_keys_move_down_action.generic.localdata[0] = ++i;
	/* FIXME: array subscript here: */
	s_keys_move_down_action.generic.name = bindnames[s_keys_move_down_action.generic.localdata[0]][1];

	Menu_AddItem(&s_keys_menu, (void *)&s_keys_attack_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_change_weapon_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_prev_weapon_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_walk_forward_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_backpedal_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_turn_left_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_turn_right_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_run_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_step_left_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_step_right_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_sidestep_action);

	Menu_AddItem(&s_keys_menu, (void *)&s_keys_mouse_look_action);
	Menu_AddItem(&s_keys_menu, (void *)&s_keys_use_action);


	Menu_SetStatusBar(&s_keys_menu, "Enter to change, backspace to clear");
	Menu_Center(&s_keys_menu);
}

PRIVATE void Keys_MenuDraw(void)
{
	if (g_version->value == SPEAROFDESTINY) {
		R_Draw_Tile(0, 0, (int)viddef.width, (int)viddef.height,
					"pics/C_BACKDROPPIC.tga");

		M_Banner("pics/SC_CUSTOMIZEPIC.tga", (W16)0);
		M_DrawWindow((int)((viddef.width - 550) >> 1),
					 (int)(((viddef.height - 300) >> 1) + 10), 550, 300,
					 sodbkgdcolour, sodbord2colour, soddeactive);
	} else {
		R_Draw_Fill(0, 0, (int)viddef.width, (int)viddef.height, bgcolour);

		M_Banner("pics/C_CUSTOMIZEPIC.tga", (W16)0);
		M_DrawWindow((int)((viddef.width - 550) >> 1),
					 (int)(((viddef.height - 300) >> 1) + 10), 550, 300,
					 bkgdcolour, bord2colour, deactive );
	}


	Menu_AdjustCursor(&s_keys_menu, 1);
	Menu_Draw(&s_keys_menu);
}

PRIVATE const char *Keys_MenuKey(int key)
{
	menuaction_s *item = (menuaction_s *)Menu_ItemAtCursor(&s_keys_menu);

	if (bind_grab) {
		if ((key != K_ESCAPE) && (key != '`')) {
			char cmd[1024];

			my_snprintf(cmd, sizeof(cmd), "bind \"%s\" \"%s\"\n",
						Key_KeynumToString(key),
						bindnames[item->generic.localdata[0]][0]);
			Cbuf_InsertText(cmd);
		}

		Menu_SetStatusBar(&s_keys_menu, "enter to change, backspace to clear");
		bind_grab = false;
		return menu_out_sound;
	}

	switch (key) {
		case K_KP_ENTER:
		case K_ENTER:
			KeyBindingFunc(item);
			return menu_in_sound;

		case K_BACKSPACE:		/* delete bindings */
		case K_DEL:				/* delete bindings */
		case K_KP_DEL:
			M_UnbindCommand(bindnames[item->generic.localdata[0]][0]);
			return menu_out_sound;

		default:
			return Default_MenuKey(&s_keys_menu, key);
	}
}

PUBLIC void M_Menu_Keys_f(void)
{
	Keys_MenuInit();
	M_PushMenu(Keys_MenuDraw, Keys_MenuKey);
}

/* EOF */
