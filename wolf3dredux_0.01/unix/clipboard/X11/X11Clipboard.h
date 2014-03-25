/*
 *  X11Clipboard.h
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/25/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 */

#ifndef __X11CLIPBOARD_H__
#define __X11CLIPBOARD_H__

#include "../../../common/arch.h" /* for types */

extern _boolean OpenClipboard(char *ptr);
extern _boolean CloseClipboard(void);
extern void *GetClipboardData(unsigned int text);
extern char *GlobalLockClipboardData(void *data);
extern void GlobalUnlockClipboardData(void *data);

#endif /* !__X11CLIPBOARD_H__ */

/* EOF */
