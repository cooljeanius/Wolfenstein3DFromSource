/*
 *  Wolfenstein3DFromSourceAppDelegate.m
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 11/13/11.
 *  Copyright 2011 George Washington University. All rights reserved.
 *
 *  License: GPL-2+, like the rest of the source.
 *
 *  Some portions adapted from code from the open-source release of
 *  Wolfenstein 3D for iOS, Copyright (C) 2009 Id Software, Inc.
 *
 */

#import "Wolfenstein3DFromSourceAppDelegate.h"
/* to add: "EAGLView.h" */
#import <AudioToolbox/AudioServices.h>
#include "wolf3dredux_0.01/device/system/system.h" /* for Sys_OS_Init() */
#include "wolf3dredux_0.01/game/wolf3d/wolf_local.h" /* for Game_Init() and Game_Shutdown() */

extern int deviceStartup();
extern int deviceShutdown();

char deviceDocDirectory[1024];
char deviceAppDirectory[1024];


void vibrateDevice_type_alert(void) {
	printf("vibrate-type alert\n");
	AudioServicesPlaySystemSound(kSystemSoundID_UserPreferredAlert);
}

@implementation Wolfenstein3DFromSourceAppDelegate

@synthesize window;
/* synthesize-ing glView requires EAGLView */

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	/* dummy condition to use the 'aNotification' parameter: */
	if (aNotification) {
		;
		/* (do nothing) */
	}
	/* get the documents directory, where we will write configs and save games */
	NSArray *paths;
	paths = NSSearchPathForDirectoriesInDomains((NSSearchPathDirectory)NSDocumentDirectory,
												(NSSearchPathDomainMask)NSUserDomainMask,
												(BOOL)YES);
	NSString *documentsDirectory;
	documentsDirectory = [paths objectAtIndex:(NSUInteger)0];
	[documentsDirectory getCString: deviceDocDirectory
						 maxLength: (sizeof(deviceDocDirectory ) - 1)
						  encoding: (NSStringEncoding)NSASCIIStringEncoding ];

	/* get the app directory, where our data files live */
	paths = NSSearchPathForDirectoriesInDomains((NSSearchPathDirectory)NSApplicationDirectory,
												(NSSearchPathDomainMask)NSUserDomainMask,
												(BOOL)YES);
	NSString *appDirectory = documentsDirectory = [paths objectAtIndex:(NSUInteger)0];
	[appDirectory getCString: deviceAppDirectory
				   maxLength: (sizeof(deviceAppDirectory) - 1)
					encoding: (NSStringEncoding)NSASCIIStringEncoding ];
	/* dummy condition to use value stored to 'documentsDirectory': */
	if (documentsDirectory == NULL) {
		;
	}

	Sys_OS_Init();
	Game_Init(); /* this might have already been called if we got here from the
				  * end of unix/unix_main.c... how to check? */
}

- (void)applicationWillResignActive:(NSApplication *)application {
	/* dummy condition to use the 'application' parameter: */
	if (application) {
		;
	}
}


- (void)applicationDidBecomeActive:(NSApplication *)application {
	/* dummy condition to use the 'application' parameter: */
	if (application) {
		;
	}
}

- (void)applicationWillTerminate:(NSApplication *)application {
	/* dummy condition to use the 'application' parameter: */
	if (application) {
		;
	}
	Game_Shutdown();
}



- (void)dealloc {
	[window release];
	/* release 'glView' once available */
	[super dealloc];
}


@end

/* EOF */
