/*
 *  OSXClipboard.m
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/25/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 *  Some of this code is adapted from Apple's ClipboardViewer sample code,
 *  which is copyright (C) 2012 Apple Inc.
 */

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
# import "OSXClipboard.h"
#else
# include "OSXClipboard.h"
#endif /* __GNUC__ && !__STRICT_ANSI__ */

#include "../../../common/common_utils.h" /* for PUBLIC and PRIVATE */

/* uncomment the '&& 0' to test the other half of the conditional: */
#if defined(__APPLE__) && defined(__OBJC__) /* && 0 */
@interface OSXClipboard()
@property(readwrite, retain) NSPasteboard *whichPboard;
@property(readwrite, retain) NSArray *types;
@end

@implementation OSXClipboard

BOOL clipboardIsOpen;

PUBLIC BOOL OpenClipboard(char *ptr)
{
	/* not sure which one of these is better to use, so just use both of them
	 * for now: */
	NSPasteboard *defaultPboard;
	NSString *actualdefaultPboard;
	defaultPboard = [NSPasteboard generalPasteboard];
	actualdefaultPboard = NSGeneralPboard;
	if ((((NSPasteboard *)ptr != defaultPboard) ||
		 ((NSString *)ptr != actualdefaultPboard))
		&& (ptr != NULL)) {
		NSLog(@"requested pasteboard '%s' was not the general pasteboard",
			  ptr);
	}
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

/* Called once the nib is done loading; we do various setup actions here so that
 * everything goes smoothly.
 */
- (void)applicationDidFinishLaunching:(NSNotification *)note {
    /* Set up our own data storage: */
    NSDictionary *plainTextAttributes = [[NSDictionary alloc] initWithObjectsAndKeys:
                                         [NSFont userFixedPitchFontOfSize:(CGFloat)10.0],
										 NSFontAttributeName,
                                         nil];
    [plainTextAttributes release];

    /* Set up our 2 text containers: 1 for fixed width, and 1 normal: */
    fixedWidthContainer = [[NSTextContainer alloc] initWithContainerSize:NSMakeSize((CGFloat)1.0e7,
																					(CGFloat)1.0e7)];
    [fixedWidthContainer setWidthTracksTextView:(BOOL)NO];
    [fixedWidthContainer setHeightTracksTextView:(BOOL)NO];
    [fixedWidthContainer setLineFragmentPadding:(CGFloat)[sizableContainer lineFragmentPadding]];

    /* Load initial data: */
    self.whichPboard = [NSPasteboard generalPasteboard];
}

/* Free up resources, remember to remove this instance as an observer. This
 * should only be called if when quitting anyway, though.
 */
- (void)dealloc {
    [sizableContainer release];
    [fixedWidthContainer release];

    self.types = nil;
    self.whichPboard = nil;

    [super dealloc];
}

@synthesize whichPboard;
@synthesize types;
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

_boolean clipboardIsOpen;

PUBLIC _boolean OpenClipboard(char *ptr)
{
#if !defined(__LP64__)
	/* LoadScrap() is from the Scrap Manager, which is 32-bit only
	 * (and deprecated): */
	LoadScrap();
	/* kScrapClipboardScrap would be the one to load if it took an arg... */
#endif /* !__LP64__ */
	/* maybe the two arguments should be switched here?
	 * first is the name of the pasteboard to reference, while the second
	 * is the variable to use to reference the first one... */
	PasteboardCreate((CFStringRef)ptr,
					 (PasteboardRef *)kPasteboardClipboard);
	clipboardIsOpen = true;
	return clipboardIsOpen;
}

PUBLIC _boolean CloseClipboard(void)
{
	/* not sure if this is the right thing to do when closing;
	 * the docs say that you are supposed to clear the pasteboard _before_ using
	 * it, so that your application will then own it. Instead, we are clearing
	 * it here when we are _done_ using it: */
	PasteboardClear((PasteboardRef)kPasteboardClipboard);
#if !defined(__LP64__)
	/* UnloadScrap is from the Scrap Manager, which is 32-bit only
	 * (and deprecated): */
	UnloadScrap();
	/* kScrapClipboardScrap would be the one to unload if it took an arg... */
#endif /* !__LP64__ */
	clipboardIsOpen = false;
	return clipboardIsOpen;
}

PUBLIC void *GetClipboardData(unsigned int text)
{
#if !defined(__LP64__)
	/* GetCurrentScrap() is from the Scrap Manager, which is 32-bit only
	 * (and deprecated): */
	GetCurrentScrap((ScrapRef *)&text);
	/* maybe kScrapClipboardScrap should be used instead? */
	/* that would look something like this, I think: */
# if 0
	GetScrapByName(kScrapClipboardScrap, kScrapGetNamedScrap,
				   (ScrapRef *)&text);
# endif /* 0 */
#endif /* !__LP64__ */
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
