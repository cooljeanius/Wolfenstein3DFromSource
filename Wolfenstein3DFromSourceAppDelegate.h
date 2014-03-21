/*
 *  Wolfenstein3DFromSourceAppDelegate.h
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

#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	void vibrateDevice_type_alert(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

@interface Wolfenstein3DFromSourceAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
    int		lastAccelUpdateMsec;
}

@property (assign) IBOutlet NSWindow *window;

@end
