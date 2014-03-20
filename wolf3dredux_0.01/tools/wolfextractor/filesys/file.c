/*
 *	Copyright (C) 2004 Michael Liebscher
 *	Copyright (C) 1997-2001 Id Software, Inc.
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
 */

/*
 *	file.c:   Portable file system services for wolfextractor.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	This code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <string.h>
#include <time.h>

#include "file.h"
#include "../../../common/common_utils.h"
#include "../string/com_string.h"
#include "../memory/memory.h"



/*
-----------------------------------------------------------------------------
 Function: DOSTIME -Convert the date y/n/d and time h:m:s to a four byte
					DOS date and time (date in high two bytes, time in low
					two bytes).

 Parameters:
			y -[in] year
			c -[in] month
			d -[in] day
			h -[in] hour
			m -[in] minute
			s -[in] second

 Returns: dos time

 Notes:
-----------------------------------------------------------------------------
*/
/* dos time start date is January 1, 1980 */
#define DOSTIME_STARTDATE         0x00210000L

#define DOSTIME( y, c, d, h, m, s ) \
(((y) < 1980) ? DOSTIME_STARTDATE : (((W32)(y) - 1980) << 25) | ((W32)(c) << 21) | ((W32)(d) << 16) | ((W32)(h) << 11) | ((W32)(m) << 5) | ((W32)(s) >> 1))


/*
-----------------------------------------------------------------------------
 Function: UnixTimeToDosTime -Converts Unix time_t into DOS format.

 Parameters:
			t -[in] unix time to convert.

 Returns:
		Unix time_t in DOS format, rounded up to the next two second
		boundary.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC W32 UnixTimeToDosTime(time_t *t)
{
  time_t t_even;
  struct tm *s;

  /* Round up to even seconds. */
  t_even = (time_t)(((W32)(*t) + 1) & (unsigned long)(~1));

  s = localtime(&t_even);
  if (s == (struct tm *)NULL) {
      /* time conversion error; use current time instead */
      t_even = (time_t)(((W32)time(NULL) + 1) & (unsigned long)(~1));
      s = localtime(&t_even);
  }

  return DOSTIME((s->tm_year + 1900), (s->tm_mon + 1), s->tm_mday,
                 s->tm_hour, s->tm_min, s->tm_sec);
}



/*******************************************************************
 *
 *	File path/name manipulation
 *
 *******************************************************************/

/* TODO: put this function in a shared library */
#ifndef FS_SkipPath /* this ifdef is bad */
PUBLIC char *FS_SkipPath(char *pathname)
#else
PUBLIC char *file_FS_SkipPath(char *pathname)
#endif /* !FS_SkipPath */
{
	char	*last;

	last = pathname;
	while (*pathname) {
		if (*pathname == '/') {
			last = (pathname + 1);
		}
		pathname++;
	}

	return last;
}


PUBLIC void FS_StripExtension(char *in, char *out)
{
	while (*in && (*in != '.')) {
		*out++ = *in++;
	}
	*out = 0;
}


PUBLIC char *FS_FileExtension(char *in)
{
	static char exten[8];
	int		i;

	while (*in && (*in != '.')) {
		in++;
	}

	if (!*in) {
		return "";
	}

	in++;
	for ((i = 0); (i < 7) && *in; i++, in++) {
		exten[i] = *in;
	}

	exten[i] = 0;
	return exten;
}


PUBLIC void FS_FileBase(char *in, char *out)
{
	char *s, *s2;

	s = (in + strlen(in) - 1);

	while ((s != in) && (*s != '.')) {
		s--;
	}

	for ((s2 = s); (s2 != in) && (*s2 != '/'); s2--) {
		;
		/* do nothing (?) */
	}

	if ((s - s2) < 2) {
		out[0] = 0;
	} else {
		s--;
		strncpy(out, (s2 + 1), (size_t)(s - s2));
		out[(s - s2)] = 0;
	}
}


PUBLIC void FS_FilePath(char *in, char *out)
{
	char *s;

	s = (in + strlen(in) - 1);

	while ((s != in) && (*s != '/')) {
		s--;
	}

	strncpy(out, in, (size_t)(s - in));
	out[s-in] = '\0';
}



/*******************************************************************
 *
 *	File I/O functions
 *
 *******************************************************************/


/*
-----------------------------------------------------------------------------
 Function: FS_FileLength() -Calculate file position.

 Parameters: filestream -[in] Pointer to valid FILE structure.

 Returns: The position as an offset relative to the beginning of
			the stream.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC SW32 FS_FileLength(FILE *filestream)
{
	SW32	pos;
	SW32	end;

	pos = ftell(filestream);
	fseek(filestream, (size_t)0, SEEK_END);

	end = ftell(filestream);
	fseek(filestream, pos, SEEK_SET);

	return end;
}

/*
-----------------------------------------------------------------------------
 Function: FS_Read

 Parameters:

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void FS_Read(void *buffer, int len, FILE *f)
{
	int	block, remaining;
	int	read;
	W8	*buf;
	int	tries;

	buf = (PW8)buffer;

	/* read in chunks for progress bar */
	remaining = len;
	tries = 0;


	#define	MAX_READ	0x10000		/* read in blocks of 64k */

	while (remaining) {
		block = remaining;
		if (block > MAX_READ) {
			block = MAX_READ;
		}
		read = (int)(fread(buf, (size_t)1, (size_t)(block), f));
		if (read == 0) {
			/* we might have been trying to read from a CD */
			if (! tries) {
				tries = 1;
			} else {
				printf("FS_Read(): 0 bytes read");
			}
		}

		if (read == -1) {
			printf("FS_Read(): -1 bytes read");
		}

		remaining -= read;
		buf += read;
	}
}

/*
-----------------------------------------------------------------------------
 Function: FS_FOpenFile

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC SW32 FS_FOpenFile( const char *filename, FILE **file )
{
	*file = fopen(filename, "rb");
	if (! *file) {
		*file = NULL;

		return -1;
	}

	return FS_FileLength(*file);

}

/*
-----------------------------------------------------------------------------
 Function: FS_LoadFile

 Parameters:

 Returns:

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC SW32 FS_LoadFile(const char *path, void **buffer)
{
	FILE	*fhandle;
	W8		*buf;
	long	length;

	buf = NULL;	/* quiet compiler warning */

/* look for it in the filesystem or pack files */
	length = FS_FOpenFile(path, &fhandle);
	if (! fhandle) {
		if (buffer) {
			*buffer = NULL;
		}

		return -1;
	}

	if (! buffer) {
		fclose(fhandle);
		return length;
	}

	buf = MM_MALLOC((size_t)(length));
	*buffer = buf;

	FS_Read(buf, (int)(length), fhandle);

	fclose(fhandle);

	return length;
}

/*
-----------------------------------------------------------------------------
 Function: FS_FreeFile

 Parameters: Nothing (?)

 Returns: Nothing.

 Notes: Basically a simple wrapper around MM_FREE() for now.
-----------------------------------------------------------------------------
*/
PUBLIC void FS_FreeFile(void *buffer)
{
	MM_FREE(buffer);
}

/* EOF */
