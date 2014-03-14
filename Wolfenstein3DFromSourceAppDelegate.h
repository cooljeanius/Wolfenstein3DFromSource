/*
 *  Wolfenstein3DFromSourceAppDelegate.h
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 11/13/11.
 *  Copyright 2011 George Washington University. All rights reserved.
 *
 *  License: GPL-2+, like the rest of the source.
 *
 */

#import <Cocoa/Cocoa.h>

@interface Wolfenstein3DFromSourceAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end
