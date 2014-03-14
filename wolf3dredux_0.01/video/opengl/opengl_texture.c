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
 *	opengl_texture.c:  OpenGL Texture Manager.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	Portion of this code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */

#include <string.h>

#include "opengl_local.h"

#include "../../memory/memory.h"
#include "../../memory/zmem.h"
#include "../../common/arch.h"
#include "../../common/common_utils.h"

int currentTextures[ 4 ];
int currenttmu;



/**************************************************************************/

#ifndef INLINECALL
# ifdef __STRICT_ANSI__
#  ifdef __inline__
#   define INLINECALL __inline__
#  else
#   ifdef __NO_INLINE__
#    warning "INLINECALL will be unavailable when using the '-ansi' compiler flag"
#   endif /* __NO_INLINE__ */
#   define INLINECALL /* nothing */
#  endif /* __inline__ */
# else
#  define INLINECALL inline
# endif /* __STRICT_ANSI__ */
#endif /* !INLINECALL */
PRIVATE INLINECALL GLenum WrapToGL( TWrapMode mode )
{
	if( mode == Repeat ) {
		return GL_REPEAT;
	} else {
		if( gl_config.Version_1_2 ) {
			return GL_CLAMP_TO_EDGE;
		} else {
			return GL_CLAMP;
		}
	}
}

#ifndef INLINECALL
# ifdef __STRICT_ANSI__
#  ifdef __inline__
#   define INLINECALL __inline__
#  else
#   ifdef __NO_INLINE__
#    warning "INLINECALL will be unavailable when using the '-ansi' compiler flag"
#   endif /* __NO_INLINE__ */
#   define INLINECALL /* nothing */
#  endif /* __inline__ */
# else
#  define INLINECALL inline
# endif /* __STRICT_ANSI__ */
#endif /* !INLINECALL */
PRIVATE INLINECALL GLenum MagFilterToGL( TMagFilter MagFilter )
{
	switch( MagFilter ) {
		case Nearest:
			return GL_NEAREST;

		case Linear:
			return GL_LINEAR;

		default:
			break;
	}

	return GL_LINEAR;
}


#ifndef INLINECALL
# ifdef __STRICT_ANSI__
#  ifdef __inline__
#   define INLINECALL __inline__
#  else
#   ifdef __NO_INLINE__
#    warning "INLINECALL will be unavailable when using the '-ansi' compiler flag"
#   endif /* __NO_INLINE__ */
#   define INLINECALL /* nothing */
#  endif /* __inline__ */
# else
#  define INLINECALL inline
# endif /* __STRICT_ANSI__ */
#endif /* !INLINECALL */
PRIVATE INLINECALL GLenum MinFilterToGL( _boolean MipMap, TMinFilter MinFilter )
{
	if( MipMap ) {
		switch( MinFilter ) {
			case NearestMipMapOff:
				return GL_NEAREST;

			case NearestMipMapNearest:
				return GL_NEAREST_MIPMAP_NEAREST;

			case NearestMipMapLinear:
				return GL_NEAREST_MIPMAP_LINEAR;

			case LinearMipMapOff:
				return GL_LINEAR;

			case LinearMipMapNearest:
				return GL_LINEAR_MIPMAP_NEAREST;

			case LinearMipMapLinear:
				return GL_LINEAR_MIPMAP_LINEAR;

			default:
				break;
		}
	} else {
		switch( MinFilter ) {
			case NearestMipMapOff:
			case NearestMipMapNearest:
			case NearestMipMapLinear:
				 return GL_NEAREST;

			case LinearMipMapOff:
			case LinearMipMapNearest:
			case LinearMipMapLinear:
				 return GL_LINEAR;

			default:
				break;
		}
	}

	return GL_LINEAR;
}

/**************************************************************************/



/*
-----------------------------------------------------------------------------
 Function: R_UploadTexture

 Parameters:

 Returns:

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC _boolean R_UploadTexture( texture_t *tex, PW8 data )
{
	int samples;
	W8 *scaled;
	W16 scaled_width, scaled_height;
	int i, c;
	W8 *scan;
	int comp = tex->bytes;

	samples = 0;
	i = 0;
	c = 0;

	/* dummy to silence clang static analyzer warnings about values stored to
	 * 'samples' and 'i' and 'c' never being read: */
	if ((samples == 0) || (i == 0) || (c == 0)) {
		;
	}

	pfglGenTextures( 1, &tex->texnum );
	pfglBindTexture( GL_TEXTURE_2D, tex->texnum );


	for( scaled_width = 1 ; scaled_width < tex->width ; scaled_width <<= 1 ) {
		;
		/* do nothing (?) */
	}

	if( gl_round_down->value && scaled_width > tex->width && tex->MipMap ) {
		scaled_width >>= 1;
	}


	for( scaled_height = 1 ; scaled_height < tex->height ; scaled_height <<= 1 ) {
		;
		/* do nothing (?) */
	}

	if( gl_round_down->value && scaled_height > tex->height && tex->MipMap ) {
		scaled_height >>= 1;
	}


	/* let people sample down the world textures for speed */
	if( tex->MipMap ) {
		scaled_width >>= (int)gl_picmip->value;
		scaled_height >>= (int)gl_picmip->value;
	}


	/* do NOT ever bother with > glMaxTexSize textures */
	if( scaled_width > glMaxTexSize ) {
		scaled_width = glMaxTexSize;
	}
	if( scaled_height > glMaxTexSize ) {
		scaled_height = glMaxTexSize;
	}

	if( scaled_width < 1 ) {
		scaled_width = 1;
	}
	if( scaled_height < 1 ) {
		scaled_height = 1;
	}


	tex->upload_width = scaled_width;
	tex->upload_height = scaled_height;


	scaled = Z_Malloc( scaled_width * scaled_height * tex->bytes );

#if 0
	if( tex->bytes < 4 ) {
		/* scan the texture for any non-255 alpha */
		c = tex->width * tex->height;
		scan = ((PW8)data) + 3;
		samples = 3;
		for( i = 0; i < c ; ++i, scan += 4 ) {
			if ( *scan != 255 ) {
				samples = 4;
				break;
			}
		}
	}
#endif /* 0 */
	if( scaled_width == tex->width && scaled_height == tex->height ) {
		memcpy( scaled, data, tex->width * tex->height * tex->bytes );
	} else {
		TM_ResampleTexture( data, tex->width, tex->height, scaled, scaled_width, scaled_height, tex->bytes, INTERPOLATION_NONE );
	}


	/* upload base image */
	pfglTexImage2D( GL_TEXTURE_2D, 0, comp, scaled_width, scaled_height, 0, tex->bytes == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, scaled );


	/* upload mipmaps if required */
	if( tex->MipMap ) {
		int miplevel = 1;

		while( TM_MipMap( scaled, &scaled_width, &scaled_height, tex->bytes ) ) {
			pfglTexImage2D( GL_TEXTURE_2D, miplevel++, tex->bytes, scaled_width, scaled_height, 0, tex->bytes == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, scaled );
		}
	}


	Z_Free( scaled );


	if( tex->isTextureCube ) {
		pfglTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, WrapToGL( tex->WrapS ) );
		pfglTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, WrapToGL( tex->WrapT ) );
		pfglTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, WrapToGL( tex->WrapR ) );
		pfglTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, MinFilterToGL( tex->MipMap, tex->MinFilter ) );
		pfglTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, MagFilterToGL( tex->MagFilter ) );
	} else {
		pfglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapToGL( tex->WrapS ) );
		pfglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapToGL( tex->WrapT ) );
		pfglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilterToGL( tex->MipMap, tex->MinFilter ) );
		pfglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilterToGL( tex->MagFilter ) );
	}

	if( gl_ext.EXTTextureFilterAnisotropic ) {
		pfglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_ext.nMaxAnisotropy );
	}

	return true;
}


/*
-----------------------------------------------------------------------------
 Function: R_DeleteTexture

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_DeleteTexture( unsigned int texnum )
{
	pfglDeleteTextures( 1, &texnum );
}

/*
-----------------------------------------------------------------------------
 Function: R_TexEnv

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_TexEnv( GLenum mode )
{
	static int lastmodes[ 4 ] = { -1, -1, -1, -1 };

	if ( mode != lastmodes[ currenttmu ] ) {
		pfglTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode );
		lastmodes[ currenttmu ] = mode;
	}
}

/*
-----------------------------------------------------------------------------
 Function: R_SelectTexture

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_SelectTexture( GLenum texture )
{
	int tmu;

	if( ! pfglActiveTextureARB ) {
		return;
	}

	if( texture == GL_TEXTURE0 ) {
		tmu = 0;
	} else if( texture == GL_TEXTURE1 ) {
		tmu = 1;
	} else if( texture == GL_TEXTURE2 ) {
		tmu = 2;
	} else {
		tmu = 3;
	}

	if( tmu == currenttmu ) {
		return;
	}


	currenttmu = tmu;

	pfglActiveTextureARB( texture );
	pfglClientActiveTextureARB( texture );

}


/*
-----------------------------------------------------------------------------
 Function: R_Bind

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_Bind( int texnum )
{
	/* Is this texture already bound? */
	if( currentTextures[ currenttmu ] == texnum ) {
		return;
	}

	currentTextures[ currenttmu ] = texnum;

	pfglBindTexture( GL_TEXTURE_2D, texnum );
}

/*
-----------------------------------------------------------------------------
 Function: R_MBind

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_MBind( GLenum target, int texnum )
{
	R_SelectTexture( target );

	if( target == GL_TEXTURE0 ) {
		if ( currentTextures[ 0 ] == texnum ) {
			return;
		}
	} else if( target == GL_TEXTURE1 ) {
		if( currentTextures[ 1 ] == texnum ) {
			return;
		}
	} else if( target == GL_TEXTURE2 ) {
		if( currentTextures[ 2 ] == texnum ) {
			return;
		}
	} else {
		if( currentTextures[ 3 ] == texnum ) {
			return;
		}
	}

	R_Bind( texnum );
}

/*
-----------------------------------------------------------------------------
 Function: R_EnableMultitexture

 Parameters:

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PUBLIC void R_EnableMultitexture( _boolean enable )
{
	if( ! pfglActiveTextureARB ) {
		return;
	}

	if( enable ) {
		R_SelectTexture( GL_TEXTURE1 );
		pfglEnable( GL_TEXTURE_2D );
		R_TexEnv( GL_REPLACE );
	} else {
		R_SelectTexture( GL_TEXTURE1 );
		pfglDisable( GL_TEXTURE_2D );
		R_TexEnv( GL_REPLACE );
	}

	R_SelectTexture( GL_TEXTURE0 );
	R_TexEnv( GL_REPLACE );

}

/* EOF */
