/*
 *	Copyright (C) 2004 Michael Liebscher
 *	Copyright (C) 1997-2001 Id Software, Inc.
 *	Copyright (C) 1995 Spencer Kimball and Peter Mattis.
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
 *	texture_manager.c:	Texture manager.
 *
 *	Author:	Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *	Date:	2004
 *
 *	Acknowledgement:
 *	Portion of this code was derived from
 *	The GIMP (an image manipulation program) and was originally
 *	written by Spencer Kimball and Peter Mattis.
 *
 *	Portion of this code was derived from Quake II, and was originally
 *	written by Id Software, Inc.
 *
 */


#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __TEXTURE_MANAGER_H__
# warning "header-guard define for texture_manager.h present before including it!"
#endif /* __TEXTURE_MANAGER_H__ */
#include "texture_manager.h"
#ifndef __TEXTURE_MANAGER_H__
# warning "header-guard define for texture_manager.h NOT present even after including it!"
#endif /* !__TEXTURE_MANAGER_H__ */
#include "renderer.h"
#include "../loaders/tga.h"
#include "../loaders/tga_minimal.h" /* for LoadTGA() */
#include "../string/com_string.h"
#include "../common/common.h"
#include "../common/arch.h"
#include "../memory/memory.h"
#include "../math/mymath.h"

PRIVATE texture_t	_texSprites[768];  /* Holds sprites */
PRIVATE texture_t	_texWalls[256];  /* Holds Walls */
PRIVATE texture_t	ttextures[MAX_TEXTURES];
PRIVATE int			numttextures;

PRIVATE texture_t	*r_notexture;		/* use for bad texture lookups */

W32 texture_registration_sequence;



/*
-----------------------------------------------------------------------------
 Function: TM_TextureList_f	-Console function to list loaded textures.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void TM_TextureList_f(void)
{
	int		i;
	texture_t	*image;
	int		texels;
	const char *palstrings[2] =
	{
		"RGB",
		"PAL"
	};

	Com_Printf("------------------\n");
	texels = 0;

	for((i = 0), (image = ttextures); (i < numttextures); ++i, ++image) {
		if (image->texnum <= 0) {
			continue;
		}

		texels += image->upload_width * image->upload_height;

		switch(image->type) {
		case TT_Sprite:
			Com_Printf("S");
			break;

		case TT_Wall:
			Com_Printf("W");
			break;

		case TT_Pic:
			Com_Printf("P");
			break;

		default:
			Com_Printf(" ");
			break;
		}

		Com_Printf(" %3i %3i %s: %s\n", image->upload_width,
				   image->upload_height, palstrings[0], image->name);
	}

	Com_Printf("Total texel count (not counting mipmaps): %i\n", texels);

}


/*
-----------------------------------------------------------------------------
 Function: TM_LoadTexture	-Load raw image into video memory.

 Parameters:
			name -[in] Name of texture image.
			data -[in] Raw pixel data in the format described by PixelFormat.
			width -[in] Width of image in pixels.
			height -[in] Height of image in pixels.
			type -[in]
			PixelFormat [in]

 Returns: Pointer to filled out texture_t structure.

 Notes: Any texture that was not touched on this registration sequence will be freed.
-----------------------------------------------------------------------------
*/
PUBLIC texture_t *TM_LoadTexture(const char *name, W8 *data, int width,
								 int height, texturetype_t type, W16 bytes)
{
	texture_t	*tex;
	int			i;


	if (strlen(name) >= sizeof(tex->name)) {
		Com_DPrintf("TM_LoadTexture: \"%s\" is too long\n", name);
		return r_notexture;
	}


    /* find a free texture_t space */
    for ((i = 0), (tex = ttextures); (i < numttextures); ++i, ++tex) {
	    if (! tex->texnum) {
		    break;
	    }
    }

    if (i == numttextures) {
	    if (numttextures == MAX_TEXTURES) {
		    Com_DPrintf("MAX_TEXTURES reached\n");
		    return r_notexture;
	    }

	    numttextures++;
    }

    tex = &ttextures[i];


	my_strlcpy(tex->name, name, MAX_GAMEPATH);
	tex->registration_sequence = texture_registration_sequence;

	tex->width = (W16)width;
	tex->height = (W16)height;
	tex->type = type;
	tex->bytes = bytes;

	switch (type) {
		case TT_Pic:
			tex->MipMap = false;
			tex->WrapS = Clamp;
			tex->WrapT = Clamp;
			tex->MinFilter = NearestMipMapOff;
			tex->MagFilter = Nearest;
			break;

		case TT_Wall:
			tex->MipMap = true;
			tex->WrapS = Repeat;
			tex->WrapT = Repeat;
			tex->MinFilter = LinearMipMapLinear;
			tex->MagFilter = Linear;
			break;

        case TT_Sprite:
			tex->MipMap = true;
			tex->WrapS = Repeat;
			tex->WrapT = Repeat;
			tex->MinFilter = LinearMipMapLinear;
			tex->MagFilter = Nearest;
			break;

		default:
            tex->MipMap = false;
			tex->WrapS = Clamp;
			tex->WrapT = Clamp;
			tex->MinFilter = NearestMipMapOff;
			tex->MagFilter = Nearest;
			break;
	}


	R_UploadTexture(tex, data);


	return tex;
}

PUBLIC void TM_LoadTexture_DB(const char *name, texture_t *tex, W8 *data,
							  int width, int height, texturetype_t type,
							  W16 bytes)
{

	if (strlen(name) >= sizeof(tex->name)) {
		Com_DPrintf("TM_LoadTexture: \"%s\" is too long\n", name);
	}


	my_strlcpy(tex->name, name, MAX_GAMEPATH);
	tex->registration_sequence = texture_registration_sequence;

	tex->width = (W16)width;
	tex->height = (W16)height;
	tex->type = type;
	tex->bytes = bytes;

	switch (type) {
		case TT_Pic:
			tex->MipMap = false;
			tex->WrapS = Clamp;
			tex->WrapT = Clamp;
			tex->MinFilter = NearestMipMapOff;
			tex->MagFilter = Nearest;
			break;

		case TT_Wall:
			tex->MipMap = true;
			tex->WrapS = Repeat;
			tex->WrapT = Repeat;
			tex->MinFilter = LinearMipMapLinear;
			tex->MagFilter = Linear;
			break;

        case TT_Sprite:
			tex->MipMap = true;
			tex->WrapS = Repeat;
			tex->WrapT = Repeat;
			tex->MinFilter = LinearMipMapLinear;
			tex->MagFilter = Nearest;
			break;

		default:
            tex->MipMap = false;
			tex->WrapS = Clamp;
			tex->WrapT = Clamp;
			tex->MinFilter = NearestMipMapOff;
			tex->MagFilter = Nearest;
			break;
	}

	R_UploadTexture(tex, data);
}

PUBLIC void TM_FindTexture_DB(const char *name, texture_t *tex,
							  texturetype_t type)
{
	int	len;
	W8	*data = NULL;	/* raw texture data */
	W16	width, height;	/* width, height of texture */
	W16 bytes;

	if (! name || ! *name) {
		return;
	}

	/* Check for file extension */
	len = (int)strlen(name);
	if (len < 5) {
		return;
	}


/*
 * load the texture from disk
 */
	if (! strcmp((name + len - 4), ".tga")) {
		LoadTGA(name, &data, &width, &height, &bytes);
		if (! data) {
			return;
		}

		TM_LoadTexture_DB(name, tex, data, width, height, type, bytes);
	} else {
		return;
	}


	MM_FREE(data);

#if 0
	return tex; /* function is void, there should be no return value */
#endif /* 0 */
}


/*
-----------------------------------------------------------------------------
 Function: TM_FreeUnusedTextures	-Free unused textures.

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Any texture that was not touched on this registration sequence will be freed.
-----------------------------------------------------------------------------
*/
PUBLIC void TM_FreeUnusedTextures(void)
{
	W32		i;
	texture_t	*tex;

	/* never free r_notexture texture */
	r_notexture->registration_sequence = texture_registration_sequence;

	for ((i = 0), (tex = ttextures); (i < (W32)numttextures); ++i, ++tex) {
		if (tex->registration_sequence == texture_registration_sequence) {
			continue; /* used this sequence */
		}

		if (! tex->registration_sequence) {
			continue; /* free image_t slot */
		}

		if (tex->type == TT_Pic) {
			continue; /* do NOT free pics */
		}

		/* free texture */
		R_DeleteTexture(tex->texnum);
		memset(tex, 0, sizeof(*tex));
	}


    for ((i = 0); (i <  256); i++) {
        tex = &_texWalls[i];

        if (tex->registration_sequence == texture_registration_sequence) {
			continue;		/* used this sequence */
		}

		if (! tex->registration_sequence) {
			continue;		/* free image_t slot */
		}

        R_DeleteTexture(tex->texnum);
		memset(tex, 0, sizeof(*tex));
    }

    for ((i = 0); (i < 768); i++) {
        tex = &_texSprites[i];

        if (tex->registration_sequence == texture_registration_sequence) {
			continue;		/* used this sequence */
		}

		if (! tex->registration_sequence) {
			continue;		/* free image_t slot */
		}

        R_DeleteTexture(tex->texnum);
		memset(tex, 0, sizeof(*tex));
    }
}


PUBLIC texture_t *TM_FindTexture_Wall(W32 imageId)
{
    texture_t *outTexture;


    if (imageId > 256) {
        return r_notexture;
    }

    outTexture = &_texWalls[imageId];
    outTexture->registration_sequence = texture_registration_sequence;

    if (! outTexture->texnum) {
        char fileName[ 32 ];

        my_snprintf( fileName, sizeof( fileName ), "walls/%.3d.tga", imageId );
        TM_FindTexture_DB( fileName, outTexture, TT_Wall );
    }

    return outTexture;
}


PUBLIC texture_t *TM_FindTexture_Sprite( W32 imageId )
{
    texture_t *outTexture;


    if( imageId > 768 ) {
        return r_notexture;
    }

    outTexture = &_texSprites[ imageId ];
    outTexture->registration_sequence = texture_registration_sequence;

    if( ! outTexture->texnum ) {
        char fileName[ 32 ];

        my_snprintf( fileName, sizeof( fileName ), "%s/%.3d.tga", spritelocation, imageId );
        TM_FindTexture_DB( fileName, outTexture, TT_Sprite );
    }

    return outTexture;
}




/*
-----------------------------------------------------------------------------
 Function: TM_FindTexture	-Find texture.

 Parameters: name -[in] Name of the texture to find.
			 type -[in] Type of texture (see texturetype_t).

 Returns: r_notexture if the texture is not found, otherwise it will
			return a valid texture_t structure.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC texture_t *TM_FindTexture(const char *name, texturetype_t type)
{
	texture_t	*tex;
	int	i, len;
	W8	*data;			/* raw texture data */
	W16	width, height;	/* width, height of texture */
	W16 bytes;
	struct _TGA the_TGA;
	/* use tga_error_strings[] global variable in this file: */
	the_TGA.error_string = tga_error_strings[0];

	i = 0;

	/* dummy to silence clang static analyzer warning about value stored to
	 * 'i' never being read: */
	if (i == 0) {
		;
	}

	if (! name || ! *name) {
		return r_notexture;
	}

	/* Check for file extension */
	len = (int)strlen(name);
	if (len < 5) {
		return r_notexture;
	}

	/* look for it in the texture cache */
	for ((i = 0), (tex = ttextures); (i < numttextures); ++i, ++tex) {
		if (! strcmp(name, tex->name)) {
			tex->registration_sequence = texture_registration_sequence;
			return tex;
		}
	}

/*
 * load the texture from disk
 */
	data = NULL;
	if (! strcmp((name + len - 4), ".tga")) {
		LoadTGA(name, &data, &width, &height, &bytes);
		if (! data) {
			return r_notexture;
		}

		tex = TM_LoadTexture(name, data, width, height, type, bytes);
	} else {
		return r_notexture;
	}

	MM_FREE(data);

	return tex;
}

/*
-----------------------------------------------------------------------------
 Function: TM_GetTextureSize	-Find texture.

 Parameters:width -[out] Width of texture.
			height -[out] Height of texture.
			name -[in] Name of the texture to get dimensions of.


 Returns: Nothing.

 Notes: If texture is not found, width and height are -1.
-----------------------------------------------------------------------------
*/
PUBLIC void TM_GetTextureSize(SW32 *width, SW32 *height, const char *name)
{
	texture_t *tex;

	tex = TM_FindTexture( name, TT_Pic );
	if( ! tex ) {
		*width = *height = -1;
		return;
	}

	*width = tex->width;
	*height = tex->height;
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
/* Note: cubic function no longer clips result */
PRIVATE INLINECALL double
cubic (double dx,
       int    jm1,
       int    j,
       int    jp1,
       int    jp2)
{
  /* Catmull-Rom - not bad */
  return (double) ((( ( - jm1 + 3 * j - 3 * jp1 + jp2 ) * dx +
                       ( 2 * jm1 - 5 * j + 4 * jp1 - jp2 ) ) * dx +
                     ( - jm1 + jp1 ) ) * dx + (j + j) ) / 2.0;
}

_boolean pixel_region_has_alpha( int bytes )
{
	if( bytes == 2 || bytes == 4 ) {
		return true;
	} else {
		return false;
	}
}


PRIVATE void
expand_line( double               *dest,
             double               *src,
             int                   bytes,
             int                   old_width,
             int                   width )
{
	double  ratio;
	int     x,b;
	int     src_col;
	double  frac;
	double *s;

	ratio = old_width / (double) width;

	/* we can overflow src's boundaries, so we expect our caller to have
	 * allocated extra space for us to do so safely (see scale_region ()) */

	/* this could be optimized much more by precalculating the coefficients for
	 * each x */

	for( x = 0; x < width; ++x ) {
		src_col = ((int) (x * ratio + 2.0 - 0.5)) - 2;
		/* +2, -2 is there because (int) rounds towards 0 and we need
		 * to round down */
		frac = (x * ratio - 0.5) - src_col;
		s = &src[ src_col * bytes ];
		for( b = 0 ; b < bytes ; b++ ) {
			dest[ b ] = cubic( frac, (int)s[ b - bytes ], (int)s[ b ],
							   (int)s[ b + bytes ], (int)s[ b + bytes * 2 ] );
		}

		dest += bytes;
	}

}


PRIVATE void
shrink_line( double               *dest,
             double               *src,
             int                   bytes,
             int                   old_width,
             int                   width )
{
	int          x;
	int          b;
	double      *srcp;
	double      *destp;
	double       accum[4];
	double       slice;
	const double avg_ratio = (double) width / old_width;
	const double inv_width = 1.0 / width;
	int          slicepos;      /* slice position relative to width */

#if 0
	Com_DPrintf("shrink_line bytes=%d old_width=%d width=%d interp=%d "
				"avg_ratio=%f\n",
				bytes, old_width, width, interp, avg_ratio);

#endif /* 0 */

#if HAVE_G_RETURN_IF_FAIL && defined(g_return_if_fail) && 0
	g_return_if_fail( bytes <= 4 );
#endif /* HAVE_G_RETURN_IF_FAIL && g_return_if_fail && 0 */

	/* This algorithm calculates the weighted average of pixel data that
	 * each output pixel must receive, taking into account that it always
	 * scales down, i.e. there is always more than one input pixel per each
	 * output pixel. */

	srcp = src;
	destp = dest;

	slicepos = 0;

	/* Initialize accum to the first pixel slice. As there is no partial
	 * pixel at start, that value is 0. The source data is interleaved, so
	 * we maintain BYTES accumulators at the same time to deal with that
	 * many channels simultaneously.  */
	for( b = 0 ; b < bytes ; ++b ) {
		accum[ b ] = 0.0;
	}

	for( x = 0 ; x < width ; x++ ) {
		/* Accumulate whole pixels.  */
		do {
			for( b = 0 ; b < bytes ; b++ ) {
				accum[ b ] += *srcp++;
			}

			slicepos += width;
        } while( slicepos < old_width );
		slicepos -= old_width;

		if( ! (slicepos < width)) {
			Com_Printf( "Assertion (slicepos < width) failed. Please report.\n" );
		}

		if( slicepos == 0 ) {
			/* Simplest case: we have reached a whole pixel boundary. Store
			 * the average value per channel and reset the accumulators for
			 * the next round.
             *
			 * The main reason to treat this case separately is to avoid an
			 * access to out-of-bounds memory for the first pixel.  */
			for (b = 0; b < bytes; b++) {
				*destp++ = accum[b] * avg_ratio;
				accum[b] = 0.0;
			}
		} else {
			for( b = 0; b < bytes; b++ ) {
				/* We have accumulated a whole pixel per channel where just a
				 * slice of it was needed. Subtract now the previous pixel's
				 * extra slice.  */
				slice = srcp[- bytes + b] * slicepos * inv_width;
				*destp++ = (accum[b] - slice) * avg_ratio;

				/* That slice is the initial value for the next round.  */
				accum[b] = slice;
            }
        }
    }

	/* Sanity check: srcp should point to the next-to-last position, and
	 * slicepos should be zero.  */
	if( ! (srcp - src == old_width * bytes && slicepos == 0) ) {
		Com_Printf ("Assertion (srcp - src == old_width * bytes && slicepos == 0)"
					" failed. Please report.");
	}
}

PRIVATE void pixel_region_get_row( W8 *src, int y, int width, W8 *tmp_src, int BytesPerPixel )
{
	int i;
    unsigned long k = 0;
    unsigned char *scanline = tmp_src;
	unsigned char *ptr = src;

	for( i = 0 ; i < (width * BytesPerPixel) ; ++i ) {
		scanline[ k++ ] = ptr[ y * width * BytesPerPixel + i ];
	}
}

PRIVATE void pixel_region_set_row(W8 *dest,
								  int BytesPerPixel,
								  int y,
								  int width,
								  W8 *data)
{
	int i;
	unsigned long k = 0;
    unsigned char *scanline = dest;
	unsigned char *ptr = data;

	for ((i = 0); (i < (width * BytesPerPixel)); ++i) {
		scanline[ y * width * BytesPerPixel + i ] = ptr[ k++ ];
	}
}

PRIVATE void
get_premultiplied_double_row(W8 *in,
							 int PRbytes,
							 int         x,
							 int         y,
							 int         w,
							 double     *row,
							 W8      *tmp_src,
							 int         n)
{
	int b;
	int bytes;

	bytes = PRbytes; /* initialize */

	/* dummy to use parameter 'n': */
	if (n == 0) {
		;
	}

	pixel_region_get_row(in, y, w, tmp_src, bytes);

	if (pixel_region_has_alpha(bytes)) {
		/* premultiply the alpha into the double array */
		double *irow  = row;
		int     alpha = bytes - 1;
		double  mod_alpha;

		for (x = 0; x < w; ++x) {
			mod_alpha = tmp_src[ alpha ] / 255.0;
			for (b = 0; b < alpha; ++b) {
				irow[ b ] = mod_alpha * tmp_src[ b ];
			}

			irow[ b ] = tmp_src[ alpha ];
			irow += bytes;
			tmp_src += bytes;
		}
    } else { /* no alpha */
		for (x = 0; x < w * bytes; ++x) {
			row[ x ] = tmp_src[ x ];
		}
	}

	/* set the off edge pixels to their nearest neighbor */
	for (b = 0; b < 2 * bytes; b++) {
		row[ b - 2 * bytes ] = row[ b % bytes ];
	}

	for (b = 0; b < bytes * 2; b++) {
		row[ b + w * bytes ] = row[ (w - 1) * bytes + b % bytes ];
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
PRIVATE INLINECALL void
rotate_pointers( W8 **p, W32 n )
{
	W32  i;
	W8  *tmp;

	tmp = p[ 0 ];
	for( i = 0 ; i < n-1 ; i++ ) {
		p[ i ] = p[ i + 1 ];
    }

	p[ i ] = tmp;
}

PRIVATE void
get_scaled_row( double              **src,
                int                   y,
                int                   new_width,
                double               *row,
                W8			        *src_tmp,
                W8			        *srcPR,
                int old_width,
                int old_height,
                int bytes )
{
	/* get the necesary lines from the source image, scale them,
	 * and put them into src[] */
	rotate_pointers( (unsigned char  **)src, 4 );

	if( y < 0 ) {
		y = 0;
	}

	if( y < old_height ) {
		get_premultiplied_double_row( srcPR, bytes, 0, y, old_width,
                                    row, src_tmp, 1 );
		if( new_width > old_width ) {
			expand_line( src[3], row, bytes, old_width, new_width );
		} else if( old_width > new_width ) {
			shrink_line( src[3], row, bytes, old_width, new_width );
		} else { /* no scaling needed */
			memcpy(src[3], row,
				   (sizeof(double) * (unsigned long)new_width * (unsigned long)bytes));
		}
	} else {
		memcpy(src[3], src[2],
			   (sizeof(double) * (unsigned long)new_width * (unsigned long)bytes));
	}
}


/*
non-interpolating scale_region.
 */
PRIVATE void
scale_region_no_resample(W8 *in, int inwidth, int inheight,
						W8 *out, int outwidth, int outheight, char bytes)
{
	int    *x_src_offsets;
	int    *y_src_offsets;
	W8	   *src;
	W8	   *dest;
	int    width, height, orig_width, orig_height;
	int    last_src_y;
	int    row_bytes;
	int    x, y, b;


	orig_width = inwidth;
	orig_height = inheight;

	width = outwidth;
	height = outheight;


	/*  the data pointers...  */
	x_src_offsets = (int *)MM_MALLOC((size_t)(sizeof(int) * (unsigned long)width * (unsigned long)bytes));
	y_src_offsets = (int *)MM_MALLOC((size_t)(sizeof(int) * (unsigned long)height));
	src  = (unsigned char *)MM_MALLOC((size_t)(orig_width * bytes));
	dest = (unsigned char *)MM_MALLOC((size_t)(width * bytes));

	/*  pre-calc the scale tables  */
	for ((b = 0); (b < bytes); b++) {
		for ((x = 0); (x < width); x++) {
			x_src_offsets[(b + (x * bytes))] =
				(b + bytes * (((x * orig_width) + (orig_width / 2)) / width));
		}
	}

	for ((y = 0); (y < height); y++) {
		y_src_offsets[y] = (((y * orig_height) + (orig_height / 2)) / height);
	}

	/*  do the scaling  */
	row_bytes = width * bytes;
	last_src_y = -1;
	for( y = 0; y < height; y++ ) {
		/* if the source of this line was the same as the source
		 * of the last line, there is no point in re-rescaling. */
		if( y_src_offsets[ y ] != last_src_y ) {
			pixel_region_get_row( in, y_src_offsets[ y ], orig_width, src, bytes );
#if 0
			pixel_region_get_row( srcPR, 0, y_src_offsets[y], orig_width, src, 1 );
#endif /* 0 */
			for( x = 0 ; x < row_bytes ; x++ ) {
				dest[ x ] = src[ x_src_offsets[ x ] ];
            }
			last_src_y = y_src_offsets[ y ];
        }

		pixel_region_set_row( out, bytes, y, width, dest );
    }

	MM_FREE( x_src_offsets );
	MM_FREE( y_src_offsets );
	MM_FREE( src );
	MM_FREE( dest );
}


/*
-----------------------------------------------------------------------------
 Function: TM_ResampleTexture	-Resize texture.

 Parameters:
			in -[in] Original texture data.
			inwidth -[in] Original width of texture in pixels.
			inheight -[in] Original height of texture in pixels.
			out -[in/out] Resized texture data.
			outwidth -[in] New width of texture in pixels.
			outheight -[in] New height of texture in pixels.
			bytes -[in] Number of bytes per pixel.
			interpolation -[in] see InterpolationType


 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void TM_ResampleTexture(W8 *in, int inwidth, int inheight, W8 *out,
							   int outwidth, int outheight, W8 bytes,
							   InterpolationType interpolation)
{
	double  *src[4];
	W8		*src_tmp;
	W8		*dest;
	double  *row, *accum;
	int     b;
	int     width, height;
	int     orig_width, orig_height;
	double  y_rat;
	int     i;
	int     old_y = -4;
	int     new_y;
	int     x, y;


	if (interpolation == INTERPOLATION_NONE) {
		scale_region_no_resample(in, inwidth, inheight,
								 out, outwidth, outheight, (char)bytes);
		return;
    }

	orig_width = inwidth;
	orig_height = inheight;

	width = outwidth;
	height = outheight;

#if 0 || DEBUG || __clang_analyzer__
  Com_DPrintf("scale_region: (%d x %d) -> (%d x %d)\n",
			  orig_width, orig_height, width, height);
#endif /* 0 || DEBUG || __clang_analyzer__ */

	/* find the ratios of old y to new y  */
	y_rat = (double) orig_height / (double) height;


	/* the data pointers...  */
	for ((i = 0); (i < 4); ++i ) {
		src[i] = (double *)MM_MALLOC((size_t)(sizeof(double) * (unsigned long)width * (unsigned long)bytes));
	}

	dest = (PW8)MM_MALLOC((size_t)(width * bytes));

	src_tmp = (PW8)MM_MALLOC((size_t)(orig_width * bytes));

	/* offset the row pointer by 2*bytes so the range of the array
	 * is [-2*bytes] to [(orig_width + 2)*bytes] */
	row = (double *)MM_MALLOC((size_t)(sizeof(double) * (unsigned long)(orig_width + (2 * 2)) *
									   (unsigned long)bytes));
	row += (bytes * 2);

	accum = (double *)MM_MALLOC((size_t)(sizeof(double) * (unsigned long)width * (unsigned long)bytes));


	/*  Scale the selected region  */

	for ((y = 0); (y < height); y++) {
		if (height < orig_height) {
			int          max;
			double       frac;
			const double inv_ratio = (1.0 / y_rat);

			if (y == 0) {
				/* load the first row if this is the first time through */
				get_scaled_row(&src[0], 0, width, row, src_tmp, in,
							   orig_width, orig_height, bytes);
			}

			new_y = (int)(y * y_rat);
			frac = (1.0 - ((y * y_rat) - new_y));
			for ((x = 0); (x < (width * bytes)); ++x ) {
				accum[x] = (src[3][x] * frac);
			}

			max = (int)((y + 1) * y_rat) - new_y - 1;

			get_scaled_row(&src[0], ++new_y, width, row, src_tmp, in,
						   orig_width, orig_height, bytes);

			while (max > 0) {
				for ((x = 0); (x < (width * bytes)); ++x ) {
					accum[x] += src[3][x];
				}

				get_scaled_row(&src[0], ++new_y, width, row, src_tmp, in,
							   orig_width, orig_height, bytes);
				max--;
            }

			frac = ((y + 1) * y_rat) - ((int)((y + 1) * y_rat));
			for ((x = 0); (x < (width * bytes)); ++x) {
				accum[x] += (frac * src[3][x]);
				accum[x] *= inv_ratio;
			}
		} else if (height > orig_height) {
			double p0, p1, p2, p3;
			double dy;

			new_y = (int)floor((y * y_rat) - 0.5);

			while (old_y <= new_y) {
				/* get the necesary lines from the source image, scale them,
				 * and put them into src[] */
				get_scaled_row(&src[0], (old_y + 2), width, row, src_tmp, in,
							   orig_width, orig_height, bytes);
				old_y++;
			}

			dy = (y * y_rat - 0.5) - new_y;

			p0 = cubic(dy, 1, 0, 0, 0);
			p1 = cubic(dy, 0, 1, 0, 0);
			p2 = cubic(dy, 0, 0, 1, 0);
			p3 = cubic(dy, 0, 0, 0, 1);

			for ((x = 0); (x < (width * bytes)); ++x) {
				accum[x] = ((p0 * src[0][x]) + (p1 * src[1][x]) +
							(p2 * src[2][x]) + (p3 * src[3][x]));
			}

        } else { /* height == orig_height */
			get_scaled_row(&src[0], y, width, row, src_tmp, in,
						   orig_width, orig_height, bytes);
			memcpy(accum, src[3], (sizeof(double) * (unsigned long)width * (unsigned long)bytes));
        }

		if (pixel_region_has_alpha(bytes)) {
			/* unmultiply the alpha */
			double  inv_alpha;
			double *p = accum;
			int     alpha = (bytes - 1);
			int     result;
			W8		*d = dest;

			for ((x = 0); (x < width); ++x) {
				if (p[alpha] > 0.001) {
					inv_alpha = (255.0 / p[alpha]);
					for ((b = 0); (b < alpha); b++) {
						result = ((int)RINT(inv_alpha * p[b]));
						if (result < 0) {
							d[b] = 0;
						} else if (result > 255) {
							d[b] = 255;
						} else {
							d[b] = (W8)result;
						}
                    }
					result = ((int)RINT(p[alpha]));
					if (result > 255) {
						d[alpha] = 255;
					} else {
						d[alpha] = (W8)result;
					}
                } else { /* alpha <= 0 */
					for ((b = 0); (b <= alpha); ++b) {
						d[b] = 0;
					}
				}

				d += bytes;
				p += bytes;
            }
        } else {
			int w;
			w = (width * bytes);

			for ((x = 0); (x < w); ++x ) {
				if (accum[x] < 0.0) {
					dest[x] = 0;
				} else if (accum[x] > 255.0) {
					dest[x] = 255;
				} else {
					dest[x] = (W8)((int)RINT(accum[x]));
				}
            }
        }
		pixel_region_set_row(out, bytes, y, width, dest);
    }

	/*  free up temporary arrays  */
	MM_FREE(accum);

	for ((i = 0); (i < 4); ++i) {
		MM_FREE(src[i]);
	}

	MM_FREE(src_tmp);
	MM_FREE(dest);

	row -= (2 * bytes);
	MM_FREE(row);
}


/*
-----------------------------------------------------------------------------
 Function: TM_MipMap	-Generate MipMap.

 Parameters:
				in -[in/out] Texture data.
				width -[in] Width of texture in pixels.
				height -[in] Height of texture in pixels.

 Returns: Nothing.

 Notes: Operates in place, quartering the size of the texture.
-----------------------------------------------------------------------------
*/
PUBLIC _boolean TM_MipMap( PW8 in, W16 *width, W16 *height, W16 bytes )
{
	W16 new_width, new_height;

	if ((*width == 1) && (*height == 1)) {
		return false;
	}


	if (*width < 2) {
		new_width = 1;
	} else {
		new_width = *width >> 1;
	}

	if (*height < 2) {
		new_height = 1;
	} else {
		new_height = (*height >> 1);
	}

	TM_ResampleTexture(in, *width, *height, in, new_width, new_height,
					   (W8)bytes, INTERPOLATION_CUBIC);

	*width = new_width;
	*height = new_height;

	return true;
}


/*
-----------------------------------------------------------------------------
 Function: TM_Init	-Initialize Texture Manager.

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Generates default texture.
-----------------------------------------------------------------------------
*/
PUBLIC void TM_Init(void)
{
	W8 *ptr;
	W8 *data;
	int x, y;

    memset(_texWalls, 0, sizeof(_texWalls));
    memset(_texSprites, 0, sizeof(_texSprites));

	texture_registration_sequence = 1;

/* create a checkerboard texture */
	data = MM_MALLOC((size_t)(16 * 16 * 4)); /* (16 * 16 * 4) = 1024 */
	for ((y = 0); (y < 16); ++y) {
		for ((x = 0); (x < 16); ++x) {
			ptr = &data[((y * 16 + x) * 4)];
			if ((y < 8) ^ (x < 8)) {
				ptr[0] = ptr[1] = ptr[2] = 0x00;
				ptr[3] = 0xFF;
			} else {
				ptr[0] = ptr[1] = ptr[2] = 0xFF;
				ptr[3] = 0xFF;
			}
		}
	}

	r_notexture = TM_LoadTexture("***r_notexture***", data, 16, 16, TT_Pic, 4);

	MM_FREE(data);

	Cmd_AddCommand("listTextures", TM_TextureList_f);
}


/*
-----------------------------------------------------------------------------
 Function: TM_Shutdown	-Shutdown Texture Manager.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void TM_Shutdown(void)
{
	W32		i;
	texture_t	*tex;

	for ((i = 0), (tex = ttextures); ((int)i < numttextures); ++i, ++tex) {
		if (! tex->registration_sequence) {
			continue;		/* free image_t slot */
		}

		/* free texture */
		R_DeleteTexture(tex->texnum);
		memset(tex, 0, sizeof(*tex));
	}


    for ((i = 0); (i < 768); i++) {
        tex =  &_texSprites[i];

		if (! tex->registration_sequence) {
			continue;		/* free image_t slot */
		}

		/* free texture */
		R_DeleteTexture(tex->texnum);
		memset(tex, 0, sizeof(*tex));
	}

    for ((i = 0); (i < 256); i++) {
        tex = &_texWalls[i];

		if (! tex->registration_sequence) {
			continue; /* free image_t slot */
		}

		/* free texture */
		R_DeleteTexture(tex->texnum);
		memset(tex, 0, sizeof(*tex));
	}

	Cmd_RemoveCommand("listTextures");
}

/* EOF */
