/*
 *  wolfextractor.h
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/24/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 *  License: GPL-2+, like the rest of the source.
 *
 */

/*
 * wolfextractor.h: header for misc. wolfextractor things
 */

/*
 * 	Notes:
 * 	This module is implemented by main.c
 */

#ifndef __WOLFEXTRACTOR_H__
#define __WOLFEXTRACTOR_H__

#include "../../common/arch.h" /* for types */
#include "../../common/common_utils.h" /* for PRIVATE */

/* prototypes: */
#ifdef PRIVATE
PRIVATE void deleteCacheDirectories(W16 version);
#else
static void deleteCacheDirectories(W16 version);
#endif /* PRIVATE */

void CheckForDataFiles(W16 *Wolf_Ext);

#endif /* !__WOLFEXTRACTOR_H__ */

/* EOF */
