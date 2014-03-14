/*
 *  main.m
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 11/13/11.
 *  Copyright 2011 George Washington University. All rights reserved.
 *
 *  License: GPL-2+, like the rest of the source.
 *
 */

#import <Cocoa/Cocoa.h>

/* This one goes first because it has the actual main function in it: */
#ifndef _UNIX_MAIN_C
# if 0
#  include "wolf3dredux_0.01/unix/unix_main.c"
# endif /* 0 */
#endif /* !_UNIX_MAIN_C */

#if !defined(main) && !defined(_UNIX_MAIN_C) && 0
int main(int argc, char *argv[])
{
    return NSApplicationMain(argc,  (const char **) argv);
}
#else
typedef int dummy_Cocoa_main_t;
#endif /* !main && !_UNIX_MAIN_C && 0 */

/* EOF */
