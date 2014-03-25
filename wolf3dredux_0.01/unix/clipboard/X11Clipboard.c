/*
 *  X11Clipboard.c
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/25/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 */

#include "X11Clipboard.h"

#include "../../common/arch.h" /* types */
#include "../../common/common_utils.h" /* for PUBLIC and PRIVATE */

/* this conditional is only to prevent conflicting symbols when building from
 * Xcode: */
#ifndef __APPLE__
/* (TODO: allow building against X11 on Apple platforms) */
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
#endif /* !__APPLE__ */

/* EOF */
