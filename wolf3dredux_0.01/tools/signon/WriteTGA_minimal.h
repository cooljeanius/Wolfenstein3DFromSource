/*
 *  WriteTGA_minimal.h
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/19/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 *  License: GPL-2+, like the rest of the source
 *
 */

/*
 * WriteTGA_minimal.h: simply exists to provide a prototype for WriteTGA()
 * TODO: get rid of this file once I have a common shared library (that is
 * shared between all the targets) that includes this function...
 */

#ifndef __WRITETGA_MINIMAL_H__
# define __WRITETGA_MINIMAL_H__ 1
/* just one prototype: */
extern unsigned char WriteTGA(const char *filename, unsigned short bpp,
							  unsigned short width, unsigned short height,
							  void *Data, unsigned char upsideDown,
							  unsigned char rle);

#endif /* !__WRITETGA_MINIMAL_H__ */

/* EOF */
