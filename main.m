/*
 *  main.m
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

#import <Cocoa/Cocoa.h>
#include <unistd.h>
#include <string.h>

/* This one goes first because it has the actual main function in it: */
#ifndef _UNIX_MAIN_C
# if 0 /* (actually nvm) */
#  include "wolf3dredux_0.01/unix/unix_main.c"
# endif /* 0 */
#endif /* !_UNIX_MAIN_C */

#if !defined(main) && !defined(_UNIX_MAIN_C) && 0
int main(int argc, char *argv[])
{
    int i, len, retval;
    if (argc == 1) {
    	char cwd[256];
    	strcpy(cwd, argv[0]);
    	len = (int)strlen(cwd);
    	for (i = (len - 1); (i >= 0); i--) {
    		if (cwd[i] == '/') {
    			cwd[i] = 0;
    			break;
    		}
    		cwd[i] = 0;
    	}
    	setenv("CWD", cwd, 1);
    }
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    retval = NSApplicationMain(argc,  (const char **)argv);
    [pool release];
    return retval;
}
#else
typedef int dummy_Cocoa_main_t;
#endif /* !main && !_UNIX_MAIN_C && 0 */

/* EOF */
