/*
 *  OSXClipboard.h
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/25/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 */

#ifndef __OSXCLIPBOARD_H__
#define __OSXCLIPBOARD_H__

#if defined(__APPLE__) && defined(__OBJC__)
# if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#  import <Cocoa/Cocoa.h>
# else
#  include <Cocoa/Cocoa.h>
# endif /* __GNUC__ && !__STRICT_ANSI__ */

/* prototypes: */
extern BOOL OpenClipboard(char *ptr);
extern BOOL CloseClipboard(void);
extern void *GetClipboardData(unsigned int);
extern char *GlobalLockClipboardData(void *);
extern void GlobalUnlockClipboardData(void *);

@interface OSXClipboard : NSObject {
	int dummy_osx_clipboard_struct_member;
}

@end

#else /* not both (__APPLE__ && __OBJC__): */
# ifdef __APPLE__
#  if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#   import <Carbon/Carbon.h>
#  else
#   include <Carbon/Carbon.h>
#  endif /* __GNUC__ && !__STRICT_ANSI__ */
# endif /* __APPLE__ */

#include "../../common/arch.h" /* types */

/* prototypes: */
extern _boolean OpenClipboard(char *ptr);
extern _boolean CloseClipboard(void);
extern void *GetClipboardData(unsigned int text);
extern char *GlobalLockClipboardData(void *data);
extern void GlobalUnlockClipboardData(void *data);

#endif /* __APPLE__ && __OBJC__ */

#endif /* !__OSXCLIPBOARD_H__ */

/* EOF */
