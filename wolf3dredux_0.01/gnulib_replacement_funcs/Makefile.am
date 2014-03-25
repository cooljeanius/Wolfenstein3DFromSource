# needs to end in a "/":
LIBOBJDIR = ./

noinst_LIBRARIES = libgnu.a

## clear to prevent implicit rules from trying to find a 'libgnu.c':
libgnu_a_SOURCES =
## (TODO: actually add a 'libgnu.c' file)

libgnu_a_LIBADD = $(LIBOBJS)

## Leave out the ALLOCA obj, because its header requires special
## modification that comes by actually using gnulib properly
