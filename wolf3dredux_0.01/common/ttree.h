/*
 *	Copyright (C) 2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 * 	as published by the Free Software Foundation; either version 2
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
 *	ttree.h: Ternary Search Tree.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from code that was originally
 *	written by Peter A. Friend.
 *
 */


#ifndef __TTREE_H__
#define __TTREE_H__


#include "common.h"


typedef struct node_s
{
	unsigned char value;
    struct node_s *lo, *eq, *hi;

} node_t;

typedef struct node_lines_s
{
   node_t *node_line;

   struct node_lines_s *next;

} node_lines_t;

typedef struct
{
   W32 node_line_width;			/* number of nodes to allocated at a time */
   node_lines_t *node_lines;
   node_t *free_list;
   node_t *head[ 127 ];

} ttree_t;

extern ttree_t *ternary_tree_init( W32 width );
extern _boolean ternary_tree_add( unsigned char *key, void *new_data, ttree_t *tst, _boolean bReplace, void **old_data );
extern void *ternary_tree_remove( unsigned char *key, ttree_t *tst );
extern void *ternary_tree_find( unsigned char *key, ttree_t *tst );
extern void ternary_tree_cleanup( ttree_t *tst );

#endif /* __TTREE_H__ */

/* EOF */
