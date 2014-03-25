/*
 *  pak.h
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/24/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 *  License: GPL-2+, like the rest of the source.
 *
 */

/*
 * pak.h: header for PAK files
 */

/*
 * 	Notes:
 * 	This module is implemented by pak.c
 */

#ifndef __PAK_H__
#define __PAK_H__

#include "../../common/arch.h" /* for types */

/* prototypes: */
extern void PAK_builder(const char *packname, W16 version);

#endif /* !__PAK_H__ */

/* EOF */
