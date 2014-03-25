/*
 * unix_joystick.c
 */

#ifdef HAVE_CONFIG_H
# include "../../../config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_MATH_H
# include <math.h>
#endif /* HAVE_MATH_H */

#ifndef strcmp
# include <string.h>
#endif /* !strcmp */

#include "../../../client/keys.h"
#include "../../../client/client.h"
#include "../input.h"


/* joystick defines and variables */
/* where should defines be moved? */
#define JOY_ABSOLUTE_AXIS 0x00000000 /* control like a joystick */
#define JOY_RELATIVE_AXIS 0x00000010 /* control like a mouse, spinner, trackball */
#define JOY_MAX_AXES      6			 /* X, Y, Z, R, U, V */
#define JOY_AXIS_X		  0
#define JOY_AXIS_Y		  1
#define JOY_AXIS_Z		  2
#define JOY_AXIS_R		  3
#define JOY_AXIS_U		  4
#define JOY_AXIS_V		  5

/* these defines are taken from Wine's <mmsystem.h>: */
#ifndef JOY_RETURNX
# define JOY_RETURNX 0x00000001
#endif /* !JOY_RETURNX */
#ifndef JOY_RETURNY
# define JOY_RETURNY 0x00000002
#endif /* !JOY_RETURNY */
#ifndef JOY_RETURNZ
# define JOY_RETURNZ 0x00000004
#endif /* !JOY_RETURNZ */
#ifndef JOY_RETURNR
# define JOY_RETURNR 0x00000008
#endif /* !JOY_RETURNR */
#ifndef JOY_RETURNU
# define JOY_RETURNU 0x00000010
#endif /* !JOY_RETURNU */
#ifndef JOY_RETURNV
# define JOY_RETURNV 0x00000020
#endif /* !JOY_RETURNV */
#ifndef JOY_RETURNPOV
# define JOY_RETURNPOV 0x00000040
#endif /* !JOY_RETURNPOV */
#ifndef JOY_RETURNBUTTONS
# define JOY_RETURNBUTTONS 0x00000080
#endif /* !JOY_RETURNBUTTONS */
#ifndef JOY_RETURNCENTERED
# define JOY_RETURNCENTERED	0x00000400
#endif /* !JOY_RETURNCENTERED */

enum _ControlList
{
	AxisNada = 0,
	AxisForward,
	AxisLook,
	AxisSide,
	AxisTurn,
	AxisUp
};

unsigned int dwAxisFlags[JOY_MAX_AXES] =
{
	JOY_RETURNX, JOY_RETURNY, JOY_RETURNZ, JOY_RETURNR, JOY_RETURNU, JOY_RETURNV
};

unsigned int   dwAxisMap[JOY_MAX_AXES];
unsigned int   dwControlMap[JOY_MAX_AXES];
unsigned int *pdwRawValue[JOY_MAX_AXES];

extern _boolean mlooking;

cvar_t  *in_joystick;


/* None of these cvars are saved over a session.
 * This means that advanced controller configuration needs to be executed
 * each time. This avoids any problems with getting back to a default usage
 * or when changing from one controller to another. This way at least something
 * works. */
cvar_t  *joy_name;
cvar_t  *joy_advanced;
cvar_t  *joy_advaxisx;
cvar_t  *joy_advaxisy;
cvar_t  *joy_advaxisz;
cvar_t  *joy_advaxisr;
cvar_t  *joy_advaxisu;
cvar_t  *joy_advaxisv;
cvar_t  *joy_forwardthreshold;
cvar_t  *joy_sidethreshold;
cvar_t  *joy_pitchthreshold;
cvar_t  *joy_yawthreshold;
cvar_t  *joy_forwardsensitivity;
cvar_t  *joy_sidesensitivity;
cvar_t  *joy_pitchsensitivity;
cvar_t  *joy_yawsensitivity;
cvar_t  *joy_upthreshold;
cvar_t  *joy_upsensitivity;

_boolean		joy_avail, joy_advancedinit, joy_haspov;
unsigned int	joy_oldbuttonstate, joy_oldpovstate;

int				joy_id;
unsigned int	joy_flags;
unsigned int	joy_numbuttons;


/*
 ================
 RawValuePointer
 ================
 */
unsigned int *RawValuePointer(int axis);
/* TODO: move prototype into header once this func actually works for unix: */
unsigned int *RawValuePointer(int axis)
{
	switch (axis) {
		/* TODO: actually do something here */
		default:
			return NULL;
	}

	return NULL;
}

/*
 =====================
 Joy_AdvancedUpdate_f
 =====================
 */
void Joy_AdvancedUpdate_f(void);
/* TODO: move prototype into header once this func actually works for unix: */
void Joy_AdvancedUpdate_f(void)
{
	/* called once by IN_ReadJoystick and by user whenever an update is
	 * needed cvars are now available */
	int   i;
	unsigned int dwTemp;

	/* initialize all the maps */
	for ((i = 0); (i < JOY_MAX_AXES); i++) {
		dwAxisMap[i] = AxisNada;
		dwControlMap[i] = JOY_ABSOLUTE_AXIS;
		pdwRawValue[i] = RawValuePointer(i);
	}

	if (joy_advanced->value == 0.0) {
		/* default joystick initialization */
		/* 2 axes only with joystick control */
		dwAxisMap[JOY_AXIS_X] = AxisTurn;
#if 0
		dwControlMap[JOY_AXIS_X] = JOY_ABSOLUTE_AXIS;
#endif /* 0 */
		dwAxisMap[JOY_AXIS_Y] = AxisForward;
#if 0
		dwControlMap[JOY_AXIS_Y] = JOY_ABSOLUTE_AXIS;
#endif /* 0 */
	} else {
		if (strcmp(joy_name->string, "joystick") != 0) {
			/* notify user of advanced controller */
			Com_Printf ("\n%s configured\n\n", joy_name->string);
		}

		/* advanced initialization here */
		/* data supplied by user via joy_axisn cvars */
		dwTemp = (unsigned int)joy_advaxisx->value;
		dwAxisMap[JOY_AXIS_X] = (dwTemp & 0x0000000f);
		dwControlMap[JOY_AXIS_X] = (dwTemp & JOY_RELATIVE_AXIS);
		dwTemp = (unsigned int)joy_advaxisy->value;
		dwAxisMap[JOY_AXIS_Y] = (dwTemp & 0x0000000f);
		dwControlMap[JOY_AXIS_Y] = (dwTemp & JOY_RELATIVE_AXIS);
		dwTemp = (unsigned int)joy_advaxisz->value;
		dwAxisMap[JOY_AXIS_Z] = (dwTemp & 0x0000000f);
		dwControlMap[JOY_AXIS_Z] = (dwTemp & JOY_RELATIVE_AXIS);
		dwTemp = (unsigned int)joy_advaxisr->value;
		dwAxisMap[JOY_AXIS_R] = (dwTemp & 0x0000000f);
		dwControlMap[JOY_AXIS_R] = (dwTemp & JOY_RELATIVE_AXIS);
		dwTemp = (unsigned int)joy_advaxisu->value;
		dwAxisMap[JOY_AXIS_U] = (dwTemp & 0x0000000f);
		dwControlMap[JOY_AXIS_U] = (dwTemp & JOY_RELATIVE_AXIS);
		dwTemp = (unsigned int)joy_advaxisv->value;
		dwAxisMap[JOY_AXIS_V] = (dwTemp & 0x0000000f);
		dwControlMap[JOY_AXIS_V] = (dwTemp & JOY_RELATIVE_AXIS);
	}

	/* compute the axes to collect from DirectInput */
	joy_flags = (JOY_RETURNCENTERED | JOY_RETURNBUTTONS | JOY_RETURNPOV);
	for ((i = 0); (i < JOY_MAX_AXES); ++i) {
		if (dwAxisMap[i] != AxisNada) {
			joy_flags |= dwAxisFlags[i];
		}
	}
}

/*-----------------------------------------------------------------------------
 * Function: IN_StartupJoystick
 * Parameters: Nothing
 * Returns: Nothing
 * Notes: just a bunch of calls to Cvar_Get() so far:
 *---------------------------------------------------------------------------*/
void IN_StartupJoystick(void)
{
	/* joystick variables */
	in_joystick	= Cvar_Get("in_joystick", "0", CVAR_ARCHIVE);
	joy_name = Cvar_Get("joy_name", "joystick", 0);
	joy_advanced = Cvar_Get("joy_advanced", "0", 0);
	joy_advaxisx = Cvar_Get("joy_advaxisx", "0", 0);
	joy_advaxisy = Cvar_Get("joy_advaxisy", "0", 0);
	joy_advaxisz = Cvar_Get("joy_advaxisz", "0", 0);
	joy_advaxisr = Cvar_Get("joy_advaxisr", "0", 0);
	joy_advaxisu = Cvar_Get("joy_advaxisu", "0", 0);
	joy_advaxisv = Cvar_Get("joy_advaxisv", "0", 0);
	joy_forwardthreshold = Cvar_Get("joy_forwardthreshold", "0.15", 0);
	joy_sidethreshold = Cvar_Get("joy_sidethreshold", "0.15", 0);
	joy_upthreshold	= Cvar_Get("joy_upthreshold", "0.15", 0);
	joy_pitchthreshold = Cvar_Get("joy_pitchthreshold", "0.15", 0);
	joy_yawthreshold = Cvar_Get("joy_yawthreshold", "0.15", 0);
	joy_forwardsensitivity = Cvar_Get("joy_forwardsensitivity", "-1", 0);
	joy_sidesensitivity = Cvar_Get("joy_sidesensitivity", "-1", 0);
	joy_upsensitivity = Cvar_Get("joy_upsensitivity", "-1", 0);
	joy_pitchsensitivity = Cvar_Get("joy_pitchsensitivity", "1", 0);
	joy_yawsensitivity = Cvar_Get("joy_yawsensitivity", "-1", 0);

	Cmd_AddCommand("joy_advancedupdate", Joy_AdvancedUpdate_f);

    /* assume no joystick: */
    joy_avail = false;

	/* old button and POV states default to "no buttons pressed": */
	joy_oldbuttonstate = joy_oldpovstate = 0;
}

/* EOF */
