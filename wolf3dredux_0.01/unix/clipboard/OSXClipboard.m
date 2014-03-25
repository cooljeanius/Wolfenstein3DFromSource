/*
 *  OSXClipboard.m
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/25/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 */

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
# import "OSXClipboard.h"
#else
# include "OSXClipboard.h"
#endif /* __GNUC__ && !__STRICT_ANSI__ */

#include "../../common/common_utils.h" /* for PUBLIC and PRIVATE */

#if defined(__APPLE__) && defined(__OBJC__)

/* TODO: see about moving these inside the '@implementation' below: */
BOOL clipboardIsOpen;

PUBLIC BOOL OpenClipboard(char *ptr)
{
	clipboardIsOpen = YES;
	return clipboardIsOpen;
}

PUBLIC BOOL CloseClipboard(void)
{
	clipboardIsOpen = NO;
	return clipboardIsOpen;
}

PUBLIC void *GetClipboardData(unsigned int text)
{
	return NULL;
}

PUBLIC char *GlobalLockClipboardData(void *data)
{
	return NULL;
}

PUBLIC void GlobalUnlockClipboardData(void *data)
{
	return;
}

@implementation OSXClipboard

@end
#else /* not both (__APPLE__ && __OBJC__): */

#include "../../common/arch.h" /* types */

_boolean clipboardIsOpen;

PUBLIC _boolean OpenClipboard(char *ptr)
{
	clipboardIsOpen = true;
	return clipboardIsOpen;
}

PUBLIC _boolean CloseClipboard(void)
{
	clipboardIsOpen = false;
	return clipboardIsOpen;
}

PUBLIC void *GetClipboardData(unsigned int text)
{
	return NULL;
}

PUBLIC char *GlobalLockClipboardData(void *data)
{
	return NULL;
}

PUBLIC void GlobalUnlockClipboardData(void *data)
{
	return;
}

#endif /* __APPLE__ && __OBJC__ */

/* EOF */
