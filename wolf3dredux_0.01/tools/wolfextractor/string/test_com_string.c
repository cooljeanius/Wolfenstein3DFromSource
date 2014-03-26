/*
 *  test_com_string.c
 *  Wolfenstein3DFromSource
 *
 *  Created by Eric Gallager on 3/3/14.
 *  Copyright 2014 George Washington University. All rights reserved.
 *
 * There is no Makefile in this directory for this file, as I do not want to
 * confuse the rest of the build system just yet... instead, compile manually:
 *
 *	gcc -Wall -ggdb -o com_string.o -c com_string.c
 *	gcc -Wall -ggdb -o test_com_string.o -c test_com_string.c
 *	gcc -o test_com_string *.o
 */

#include <stdio.h>
#include "com_string.h"

#ifdef main
# if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#  warning "Do not try to link this file with the rest of the project."
# endif /* __GNUC__ && !__STRICT_ANSI__ */
#endif /* main */

int main (int argc, char *argv[])
{
	char *string_to_print;

	string_to_print = cs_CopyString(argv[0]);
	printf("string_to_print is %s \n", string_to_print);

	return 0;
}

/* EOF */
