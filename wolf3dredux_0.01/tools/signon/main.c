/*
 *	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/*
 *	main.c:   Extract Wolfenstein 3-D sign on image from signon.obj
 *
 *	Author:  Michael Liebscher    <johnnycanuck@users.sourceforge.net>
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


#include "WriteTGA_minimal.h"


/* TODO: allow these paths to be configurable instead of hardcoded: */
#define INPUT_FNAME "signon.obj"
#define OUTPUT_FNAME "signon.tga"

#define IMAGE_WIDTH   320
#define IMAGE_HEIGHT  200
#define BYTESPERPIXEL   3
#define IMAGE_SIZE  (IMAGE_WIDTH * IMAGE_HEIGHT * BYTESPERPIXEL)


extern char gamepal[];	/* Wolf3D palette */


/*
-----------------------------------------------------------------------------
 Function: main -Extract Wolf3D sign on image from signon.obj

 Parameters: Nothing.

 Returns: 0 on success, otherwise nonzero (or more specifically, '1')

 Notes: This is the application entry point.

 			1.  Open input file, read in data, close file.
			2.  Write targa file.

-----------------------------------------------------------------------------
*/
#ifndef main /* this ifdef is bad */
int main(int argc, char *argv[])
#else
int signon_main(int argc, char *argv[])
#endif /* !main */
{
    unsigned int i, temp, signon_index;
    unsigned long count;
    unsigned char *buffer; /* tga file buffer. */
    unsigned char signon[64512]; /* Buffer to hold raw sign on screen data. */

    FILE *filestream = fopen(INPUT_FNAME, "rb");

	printf("signon: running with path '%s' with '%i' args.\n",
		   argv[0], argc);

    if (filestream == NULL) {
		printf("Could not open file \"%s\" for read!\n", INPUT_FNAME);

		return 1;
	}

    /* Seek past header info */
    if (fseek(filestream, (long)(0x79), SEEK_SET ) ) {
        if (fclose(filestream)) {
            printf("[Error]: The filestream was not closed\n");
        }

        printf("[Error]: Could not seek past header data!\n");

		return 1;
    }

    /* On the last iteration, we read in the seven filler bytes,
     * output will discard them. */
    count = 0;
    for ((i = 0); (i < 63); ++i) {
        /* Read in 1k of Data */
        count += fread((signon + (i * 1024)), sizeof(char), (size_t)1024,
					   filestream);

        /* Seek past seven bytes of filler */
        fseek(filestream, (long)(7), SEEK_CUR);
    }

    if (fclose(filestream)) {
        printf( "[Error]: The filestream was not closed\n");
    }

    if (count != 64006) {
        printf("[Error]: Could not read from input file\n");

		return 1;
    }

/*
 * Output Targa image
 */

    buffer = malloc((size_t)(IMAGE_SIZE));
    if (buffer == NULL) {
        printf("malloc: insufficient memory available!\n");

		return 1;
    }

    /* Convert palette to rgb */
	for ((i = 0), (signon_index = 0); (i < IMAGE_SIZE); (i += BYTESPERPIXEL),
		 ++signon_index) {
        temp = (signon[signon_index] * 3);

        buffer[(i)]		= (unsigned char)(gamepal[(temp)] << 2);	/* Red */
        buffer[(i + 1)] = (unsigned char)(gamepal[(temp + 1)] << 2);/* Green */
        buffer[(i + 2)] = (unsigned char)(gamepal[(temp + 2)] << 2);/* Blue	*/
    }

	/* this matches the prototype; I have no clue why GCC says otherwise... */
    (void)WriteTGA((const char *)(OUTPUT_FNAME), (unsigned short)(24),
				   (unsigned short)(IMAGE_WIDTH),
				   (unsigned short)(IMAGE_HEIGHT), (void *)(buffer),
				   (unsigned char)(0), (unsigned char)(1));

    if (buffer) {
        free(buffer);
        buffer = NULL;
    }

    return 0;
}

/* EOF */
