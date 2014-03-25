/*
 *  OSXClipboard.h
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/25/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 *  Some of this code is adapted from Apple's ClipboardViewer sample code,
 *  which is copyright (C) 2012 Apple Inc.
 */

#ifndef __OSXCLIPBOARD_H__
#define __OSXCLIPBOARD_H__

/* uncomment the '&& 0' to test the other half of the conditional: */
#if defined(__APPLE__) && defined(__OBJC__) /* && 0 */
# if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#  import <AppKit/NSPasteboard.h>
#  import <AppKit/NSPasteboardItem.h>
#  import <Cocoa/Cocoa.h>
# else
#  include <AppKit/NSPasteboard.h>
#  include <AppKit/NSPasteboardItem.h>
#  include <Cocoa/Cocoa.h>
# endif /* __GNUC__ && !__STRICT_ANSI__ */

@interface OSXClipboard : NSObject {
	NSPasteboard *whichPboard;
    NSArray *types;
    NSInteger lastChangeCount;

    NSTextContainer *sizableContainer;
    NSTextContainer *fixedWidthContainer;

	BOOL clipboardIsOpen;
}

/* prototypes: */
extern BOOL OpenClipboard(char *ptr);
extern BOOL CloseClipboard(void);
extern void *GetClipboardData(unsigned int);
extern char *GlobalLockClipboardData(void *);
extern void GlobalUnlockClipboardData(void *);

@end

#else /* not both (__APPLE__ && __OBJC__): */
# ifdef __APPLE__
#  if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#   import <Carbon/Carbon.h>
#   import <HIServices/Pasteboard.h> /* could also grab with <HIServices/HIServices.h>... */
#   if !defined(__LP64__) /* && (OSX version is older than 10.5): */
#    import <HIToolbox/Scrap.h>
#   endif /* !__LP64__ */
#  else
#   include <Carbon/Carbon.h>
#   include <HIServices/Pasteboard.h> /* could also grab with <HIServices/HIServices.h>... */
#   if !defined(__LP64__) /* && (OSX version is older than 10.5): */
#    include <HIToolbox/Scrap.h>
#   endif /* !__LP64__ */
#  endif /* __GNUC__ && !__STRICT_ANSI__ */
# endif /* __APPLE__ */

#include "../../../common/arch.h" /* types */

/* prototypes: */
extern _boolean OpenClipboard(char *ptr);
extern _boolean CloseClipboard(void);
extern void *GetClipboardData(unsigned int text);
extern char *GlobalLockClipboardData(void *data);
extern void GlobalUnlockClipboardData(void *data);

#endif /* __APPLE__ && __OBJC__ */

#endif /* !__OSXCLIPBOARD_H__ */

/* EOF */
