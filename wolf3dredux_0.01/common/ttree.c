/*
 *	Copyright (C) 2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
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
 *	ttree.h: Ternary Search Tree.
 *
 *	Author:	Michael Liebscher	<johnnycanuck@users.sourceforge.net>
 *
 *	Acknowledgement:
 *	This code was derived from code that was originally
 *	written by Peter A. Friend.
 *
 */

#include <stdlib.h>
#include <string.h>


#include "ttree.h"



/*
-----------------------------------------------------------------------------
 Function: ternary_tree_grow_node_free_list -Grow the free node list.

 Parameters: tst -[in] Pointer to ttree_t structure.

 Returns: On success true, otherwise false.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE _boolean ternary_tree_grow_node_free_list( ttree_t *tst )
{
	node_t *current_node;
	node_lines_t *new_line;
	W32 i;

	new_line = (node_lines_t *) MM_MALLOC( sizeof( node_lines_t ) );
	if( new_line == NULL ) {
		return false;
	}

	new_line->node_line = (node_t *)
		MM_MALLOC( tst->node_line_width * sizeof( node_t ) );
	if( new_line->node_line == NULL ) {
		MM_FREE( new_line );

		return false;
	}

	memset( new_line->node_line, 0, tst->node_line_width * sizeof( node_t ) );


	new_line->next = tst->node_lines;
	tst->node_lines = new_line;


	current_node = tst->node_lines->node_line;
	tst->free_list = current_node;
	for( i = 1 ; i < tst->node_line_width ; ++i ) {
		current_node->eq = &(tst->node_lines->node_line[ i ]);
		current_node = current_node->eq;
	}
	current_node->eq = NULL;

	return true;
}


/*
-----------------------------------------------------------------------------
 Function: ternary_tree_init -Initialize a ternary tree.

 Parameters: width -[in] How many nodes to grow the free node list by.

 Returns: On success a valid pointer to ttree_t structure, otherwise NULL.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC ttree_t *ternary_tree_init( W32 width )
{
	ttree_t *tst;
	node_t *current_node;
	W32 i;

	if( width == 0 ) {
		width = 1;
	}

	tst = MM_MALLOC( sizeof( ttree_t ) );
	if( tst == NULL ) {
		return NULL;
	}

	memset( tst, 0, sizeof( ttree_t ) );


	tst->node_lines = (node_lines_t *) MM_MALLOC( sizeof( node_lines_t ) );
	if( tst->node_lines == NULL) {
		MM_FREE( tst );

		return NULL;
	}

	memset( tst->node_lines, 0, sizeof( node_lines_t ) );


	tst->node_line_width = width;
	tst->node_lines->next = NULL;

	tst->node_lines->node_line = (node_t *) MM_MALLOC( width * sizeof( node_t ) );
	if( tst->node_lines->node_line == NULL ) {
		MM_FREE( tst->node_lines );
		MM_FREE( tst );

		return NULL;
	}

	memset( tst->node_lines->node_line, 0, width * sizeof( node_t ) );

	current_node = tst->node_lines->node_line;
	tst->free_list = current_node;
	for( i = 1 ; i < width ; ++i ) {
		current_node->eq = &(tst->node_lines->node_line[ i ]);
		current_node = current_node->eq;
	}

	current_node->eq = NULL;

	return tst;
}

/*
-----------------------------------------------------------------------------
 Function: ternary_tree_add -Add new key/data to tree.

 Parameters:
			key -[in] NUL-terminated string.
			new_data -[in] Data to associate with key.
			tst -[in] Pointer to valid ttree_t structure.
			bReplace -[in] If a collision occurs replace old data?
			old_data -[out] Pointer to old data.

 Returns: On success true, otherwise false.

 Notes: Call ternary_tree_cleanup() to destroy the allocated tree.
-----------------------------------------------------------------------------
*/
PUBLIC _boolean ternary_tree_add( unsigned char *key, void *new_data, ttree_t *tst, _boolean bReplace, void **old_data )
{
	node_t *current_node;
	node_t *new_node_tree_begin = NULL;
	int key_index;
	int perform_loop = 1;


	if( ! key || ! *key ) {
		return false;
	}


	if( tst->head[ (int)key[ 0 ] ] == NULL ) {
		if( tst->free_list == NULL ) {
			if( ! ternary_tree_grow_node_free_list( tst ) ) {
				return false;
			}
		}

		tst->head[ (int)key[ 0 ] ] = tst->free_list;

		tst->free_list = tst->free_list->eq;
		current_node = tst->head[ (int)key[ 0 ] ];
		current_node->value = key[ 1 ];
		if( key[ 1 ] == 0 ) {
			current_node->eq = new_data;

			return true;
		} else {
			perform_loop = 0;
		}
	}

	current_node = tst->head[ (int)key[ 0 ] ];
	key_index = 1;

	while( perform_loop == 1 ) {
		if( key[ key_index ] == current_node->value ) {
			if( key[ key_index ] == 0 ) {
				if( bReplace ) {
					if( old_data != NULL ) {
						*old_data = current_node->eq;
					}

					current_node->eq = new_data;

					return true;
				} else {
					if( old_data != NULL ) {
						*old_data = current_node->eq;
					}

					return false;
				}
			} else {
				if( current_node->eq == NULL ) {
					if( tst->free_list == NULL ) {
						if( ! ternary_tree_grow_node_free_list( tst ) ) {
							return false;
						}
					}
					current_node->eq = tst->free_list;

					tst->free_list = tst->free_list->eq;
					new_node_tree_begin = current_node;
					current_node = current_node->eq;
					current_node->value = key[key_index];

					break;
				} else {
					current_node = current_node->eq;
					key_index++;

					continue;
				}
			}
		}

		if (((current_node->value == 0) && (key[key_index] < 64)) ||
			((current_node->value != 0) && (key[key_index] <
											current_node->value)) ) {
			if( current_node->lo == NULL ) {
				if( tst->free_list == NULL ) {
					if( ! ternary_tree_grow_node_free_list( tst ) ) {
						return false;
					}
				}
				current_node->lo = tst->free_list;

				tst->free_list = tst->free_list->eq;
				new_node_tree_begin = current_node;
				current_node = current_node->lo;
				current_node->value = key[key_index];
				if( key[ key_index ] == 0 ) {
					current_node->eq = new_data;

					return true;
				} else {
					break;
				}
			} else {
				current_node = current_node->lo;
				continue;
			}
		} else {
			if( current_node->hi == NULL ) {
				if( tst->free_list == NULL ) {
					if( ternary_tree_grow_node_free_list( tst ) != 1 ) {
						return false;
					}
				}
				current_node->hi = tst->free_list;

				tst->free_list = tst->free_list->eq;
				new_node_tree_begin = current_node;
				current_node = current_node->hi;
				current_node->value = key[key_index];
				break;
			} else {
				current_node = current_node->hi;

				continue;
			}
		}
	}

	do {
		key_index++;

		if( tst->free_list == NULL ) {
			if( ternary_tree_grow_node_free_list( tst ) != 1 ) {
				current_node = new_node_tree_begin->eq;

				while( current_node->eq != NULL ) {
					current_node = current_node->eq;
				}

				current_node->eq = tst->free_list;
				tst->free_list = new_node_tree_begin->eq;
				new_node_tree_begin->eq = NULL;

				return false;
			}
		}


		if( tst->free_list == NULL ) {
			if( ternary_tree_grow_node_free_list( tst ) != 1 ) {
				return false;
			}
		}
		current_node->eq = tst->free_list;

		tst->free_list = tst->free_list->eq;
		current_node = current_node->eq;
		current_node->value = key[ key_index ];

	} while( key[ key_index ] != 0 );

	current_node->eq = new_data;

	return true;
}

/*
-----------------------------------------------------------------------------
 Function: ternary_tree_remove -Removes the key from tree and return pointer
								to the data.

 Parameters:
			key -[in] NUL-terminated string.
			tst -[in] Pointer to valid ttree_t structure.

 Returns: On success valid pointer to key data, otherwise NULL.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void *ternary_tree_remove( unsigned char *key, ttree_t *tst )
{
	node_t *current_node;
	node_t *current_node_parent;
	node_t *last_branch;
	node_t *last_branch_parent;
	node_t *next_node;
	node_t *last_branch_replacement;
	node_t *last_branch_dangling_child;
	int key_index;


	if( key[ 0 ] == 0 ) {
		return NULL;
	}

	if( tst->head[ (int)key[ 0 ] ] == NULL ) {
		return NULL;
	}

	last_branch = NULL;
	last_branch_parent = NULL;
	current_node = tst->head[ (int)key[ 0 ] ];
	current_node_parent = NULL;
	key_index = 1;

	while( current_node != NULL ) {
		if( key[ key_index ] == current_node->value ) {
			if( (current_node->lo != NULL) || (current_node->hi != NULL) ) {
				last_branch = current_node;
				last_branch_parent = current_node_parent;
			}

			if( key[ key_index ] == 0 ) {
				break;
			} else {
				current_node_parent = current_node;
				current_node = current_node->eq;
				key_index++;

				continue;
			}
		} else if (((current_node->value == 0) && (key[key_index] < 64)) ||
				   ((current_node->value != 0) && (key[key_index] <
												   current_node->value)) ) {
			last_branch_parent = current_node;
			current_node_parent = current_node;
			current_node = current_node->lo;
			last_branch = current_node;

			continue;
		} else {
			last_branch_parent = current_node;
			current_node_parent = current_node;
			current_node = current_node->hi;
			last_branch = current_node;

			continue;
		}
	}


	if( current_node == NULL ) {
		return NULL;
	}

	if( last_branch == NULL ) {
		next_node = tst->head[ (int)key[ 0 ] ];
		tst->head[ (int)key[ 0 ] ] = NULL;
	} else if( (last_branch->lo == NULL) && (last_branch->hi == NULL) ) {
		if( last_branch_parent->lo == last_branch ) {
			last_branch_parent->lo = NULL;
		} else {
			last_branch_parent->hi = NULL;
		}

		next_node = last_branch;
	} else {
		if( (last_branch->lo != NULL) && (last_branch->hi != NULL) ) {
			last_branch_replacement = last_branch->hi;
			last_branch_dangling_child = last_branch->lo;
		} else if( last_branch->hi != NULL ) {
			last_branch_replacement = last_branch->hi;
			last_branch_dangling_child = NULL;
		} else {
			last_branch_replacement = last_branch->lo;
			last_branch_dangling_child = NULL;
		}


		if( last_branch_parent == NULL ) {
			tst->head[ (int)key[ 0 ] ] = last_branch_replacement;
		} else {
			if( last_branch_parent->lo == last_branch ) {
				last_branch_parent->lo = last_branch_replacement;
			} else if( last_branch_parent->hi == last_branch ) {
				last_branch_parent->hi = last_branch_replacement;
			} else {
				last_branch_parent->eq = last_branch_replacement;
			}
		}

		if( last_branch_dangling_child != NULL ) {
			current_node = last_branch_replacement;

			while( current_node->lo != NULL ) {
				current_node = current_node->lo;
			}

			current_node->lo = last_branch_dangling_child;
		}

		next_node = last_branch;
	}

	do {
		current_node = next_node;
		next_node = current_node->eq;

		current_node->lo = NULL;
		current_node->hi = NULL;
		current_node->eq = tst->free_list;
		tst->free_list = current_node;

	} while( current_node->value != 0 );

	return next_node;
}


/*
-----------------------------------------------------------------------------
 Function: ternary_tree_find -Lookup key in tree and return pointer to
								data.

 Parameters:
			key -[in] NUL-terminated string.
			tst -[in] Pointer to valid ttree_t structure.

 Returns: On success valid pointer to key data, otherwise NULL.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void *ternary_tree_find( unsigned char *key, ttree_t *tst )
{
	node_t *current_node;
	int key_index;


	if( key[ 0 ] == 0 || tst == NULL ) {
		return NULL;
	}


	if( tst->head[ (int)key[ 0 ] ] == NULL ) {
		return NULL;
	}


	current_node = tst->head[ (int)key[ 0 ] ];
	key_index = 1;

	while( current_node != NULL ) {
		if( key[ key_index ] == current_node->value ) {
			if( current_node->value == 0 ) {
				return current_node->eq;
			} else {
				current_node = current_node->eq;
				key_index++;

				continue;
			}
		} else if (((current_node->value == 0) && (key[key_index] < 64)) ||
				   ((current_node->value != 0) && (key[key_index] <
												   current_node->value)) ) {
			current_node = current_node->lo;

			continue;
		} else {
			current_node = current_node->hi;

			continue;
		}
	}

	return NULL;
}

/*
-----------------------------------------------------------------------------
 Function: ternary_tree_cleanup -Destroy tree.

 Parameters: tst -[in] Pointer to valid ttree_t structure.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PUBLIC void ternary_tree_cleanup( ttree_t *tst )
{
	node_lines_t *current_line;
	node_lines_t *next_line;


	if( tst == NULL ) {
		return;
	}

	next_line = tst->node_lines;

	do {
		current_line = next_line;
		next_line = current_line->next;

		MM_FREE( current_line->node_line );
		MM_FREE( current_line );

	} while( next_line != NULL );

	MM_FREE( tst );
}

/* EOF */
