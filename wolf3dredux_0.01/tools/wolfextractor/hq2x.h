/*
 *  hq2x filter
 *  ----------------------------------------------------------
 *  Copyright (C) 2003 MaxSt ( maxst@hiend3d.com )
 */
/*
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *
 * 	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __HQ2X_H__
#define __HQ2X_H__

extern void Interp1(unsigned char *pc, int c1, int c2);
extern void Interp2(unsigned char *pc, int c1, int c2, int c3);
extern void Interp5(unsigned char *pc, int c1, int c2);
extern void Interp6(unsigned char *pc, int c1, int c2, int c3);
extern void Interp7(unsigned char *pc, int c1, int c2, int c3);
extern void Interp9(unsigned char *pc, int c1, int c2, int c3);
extern void Interp10(unsigned char *pc, int c1, int c2, int c3);

/* I want to avoid extern-ing this, as the name is so common that it has me
 * worried that it might conflict with other functions... */
int Diff( unsigned int w1, unsigned int w2 );

extern void InitLUTs( void );

extern void hq2x_32(unsigned char *pIn, unsigned char *pOut,
					int Xres, int Yres, int BpL );


#endif /* __HQ2X_H__ */

/* EOF */
