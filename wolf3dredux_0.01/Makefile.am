# Since we used to leave AM_INIT_AUTOMAKE out of configure.ac, this used to
# only exists to make sure that m4 macros are handled properly.
# The old makefile is still handwritten (as makefile.orig).

ACLOCAL_AMFLAGS = -I m4 --install

#
# Project: Wolfenstein 3-D Redux
#
# Program: wolf3dredux
#

#TODO: have autoconf check that all flags are supported:
AM_WARNFLAGS = -Wall -Wmissing-field-initializers -Wmissing-prototypes -Wimplicit -Wreturn-type -Wmissing-braces -Wparentheses -Wswitch -Wunknown-pragmas -Wsign-compare -Wnewline-eof -Wno-unused -Wno-trigraphs
AM_CFLAGS = $(AM_WARNFLAGS) -ffast-math -funroll-loops
AM_CPPFLAGS = -I. -I/usr/X11R6/include

if WOLF_OLD_BUILD
COMPILER = @CC@
NON_AM_CFLAGS = -c
OLD_LDFLAGS_UNUSED =  -s
XLDFLAGS = -L/usr/X11R6/lib @LIBS@
VORBISFLAGS = @VORBIS_LIBS@
ODIR = Release
EXEBASE = wolf3dredux
EXE = $(ODIR)/$(EXEBASE)
endif

## should contain a "/" at the end:
LIBOBJDIR = @LIBOBJDIR@

SUBDIRS = $(LIBOBJDIR) .

bin_PROGRAMS = wolf3dredux

wolf3dredux_SOURCES = client/client_input.c \
                      client/client_main.c \
                      client/client_screen.c \
                      client/console.c \
                      client/keys.c \
                      client/menu_conf.c \
                      common/arch.c \
                      common/cmd.c \
                      common/common.c \
                      common/cvar.c \
                      common/share.c \
                      common/ttree.c \
                      device/cpu/cpuid.c \
                      device/filesys/fileio.c \
                      device/filesys/files.c \
                      device/filesys/filestring.c \
                      device/filesys/zippak.c \
                      device/filesys/unix/unix_file.c \
                      device/filesys/unix/glob.c \
                      device/input/in_view.c \
                      device/input/unix/unix_keys.c \
                      device/input/unix/unix_mouse.c \
                      device/input/unix/unix_joystick.c \
                      device/system/system.c \
                      device/system/unix/unix_system.c \
                      device/timer/unix/unix_timer.c \
                      font/font_manager.c \
                      loaders/tga.c \
                      loaders/wav.c \
                      math/angle.c \
                      math/math.c \
                      math/matrix.c \
                      math/random_number.c \
                      math/vector.c \
                      memory/memory.c \
                      memory/zmem.c \
                      script/script.c \
                      sound/sound.c \
                      sound/sound_sfx_id.c \
                      sound/sound_stream.c \
                      sound/openal/openal_binding.c \
                      sound/openal/openal_main.c \
                      string/com_string.c \
                      unix/unix_main.c \
                      unix/unix_vid_sys.c \
                      video/texture_manager.c \
                      video/opengl/opengl_binding.c \
                      video/opengl/opengl_draw.c \
                      video/opengl/opengl_extension.c \
                      video/opengl/opengl_main.c \
                      video/opengl/opengl_texture.c \
                      video/opengl/unix/openglw_imp.c \
                      zlib/adler32.c \
                      zlib/crc32.c \
                      zlib/inffast.c \
                      zlib/inflate.c \
                      zlib/inftrees.c \
                      zlib/zutil.c \
                      game/wolf3d/wolf_main.c \
                      game/wolf3d/client/wolf_client_main.c \
                      game/wolf3d/client/wolf_sv_ccmds.c \
                      game/wolf3d/common/wolf_common.c \
                      game/wolf3d/entities/wolf_actor_ai.c \
                      game/wolf3d/entities/wolf_actors.c \
                      game/wolf3d/entities/wolf_ai_com.c \
                      game/wolf3d/entities/wolf_bj.c \
                      game/wolf3d/entities/wolf_player.c \
                      game/wolf3d/entities/wolf_powerups.c \
                      game/wolf3d/entities/wolf_sprites.c \
                      game/wolf3d/entities/wolf_weapon.c \
                      game/wolf3d/level/wolf_areas.c \
                      game/wolf3d/level/wolf_doors.c \
                      game/wolf3d/level/wolf_level.c \
                      game/wolf3d/level/wolf_pushwalls.c \
                      game/wolf3d/math/wolf_math.c \
                      game/wolf3d/menu/wolf_mcredits.c \
                      game/wolf3d/menu/wolf_menu.c \
                      game/wolf3d/menu/wolf_mintermission.c \
                      game/wolf3d/menu/wolf_mintro.c \
                      game/wolf3d/menu/wolf_mnewgame.c \
                      game/wolf3d/menu/wolf_moptions.c \
                      game/wolf3d/menu/wolf_mquit.c \
                      game/wolf3d/menu/wolf_msound.c \
                      game/wolf3d/menu/wolf_mvideo.c \
                      game/wolf3d/script/episode.c \
                      game/wolf3d/video/wolf_raycast.c \
                      game/wolf3d/video/wolf_renderer.c \
                      game/wolf3d/video/opengl/wolf_opengl.c

wolf3dredux_LDADD = $(LIBOBJDIR)libgnu.a

noinst_PROGRAMS = gamepal signon

gamepal_SOURCES = tools/gamepal/main.c
signon_SOURCES = tools/signon/wolf_pal.c \
                 tools/signon/main.c \
                 tools/signon/WriteTGA_minimal.c

if WOLF_OLD_BUILD
DO_CC = $(COMPILER) $(CFLAGS) $(AM_CFLAGS) $(AM_CPPFLAGS) ${CPPFLAGS} -o $@ -c $<

PROGRAM_OBJS =  $(ODIR)/client_input.o \
		$(ODIR)/client_main.o \
		$(ODIR)/client_screen.o \
		$(ODIR)/console.o \
		$(ODIR)/keys.o \
		$(ODIR)/menu_conf.o \
		$(ODIR)/arch.o \
		$(ODIR)/cmd.o \
		$(ODIR)/common.o \
		$(ODIR)/cvar.o \
		$(ODIR)/share.o \
		$(ODIR)/ttree.o \
		$(ODIR)/cpuid.o \
		$(ODIR)/fileio.o \
		$(ODIR)/files.o \
		$(ODIR)/filestring.o \
		$(ODIR)/zippak.o \
		$(ODIR)/unix_file.o \
		$(ODIR)/glob.o \
		$(ODIR)/in_view.o \
		$(ODIR)/unix_keys.o \
		$(ODIR)/unix_mouse.o \
		$(ODIR)/unix_joystick.o \
		$(ODIR)/system.o \
		$(ODIR)/unix_system.o \
		$(ODIR)/unix_timer.o \
		$(ODIR)/font_manager.o \
		$(ODIR)/tga.o \
		$(ODIR)/wav.o \
		$(ODIR)/angle.o \
		$(ODIR)/math.o \
		$(ODIR)/matrix.o \
		$(ODIR)/random_number.o \
		$(ODIR)/vector.o \
		$(ODIR)/memory.o \
		$(ODIR)/zmem.o \
		$(ODIR)/script.o \
		$(ODIR)/sound.o \
		$(ODIR)/sound_sfx_id.o \
		$(ODIR)/sound_stream.o \
		$(ODIR)/openal_binding.o \
		$(ODIR)/openal_main.o \
		$(ODIR)/com_string.o \
		$(ODIR)/unix_main.o \
		$(ODIR)/unix_vid_sys.o \
		$(ODIR)/texture_manager.o \
		$(ODIR)/opengl_binding.o \
		$(ODIR)/opengl_draw.o \
		$(ODIR)/opengl_extension.o \
		$(ODIR)/opengl_main.o \
		$(ODIR)/opengl_texture.o \
		$(ODIR)/openglw_imp.o \
		$(ODIR)/adler32.o \
		$(ODIR)/crc32.o \
		$(ODIR)/inffast.o \
		$(ODIR)/inflate.o \
		$(ODIR)/inftrees.o \
		$(ODIR)/zutil.o \
		$(ODIR)/wolf_powerups.o \
		$(ODIR)/wolf_sprites.o \
		$(ODIR)/wolf_bj.o \
		$(ODIR)/wolf_main.o \
		$(ODIR)/wolf_client_main.o \
		$(ODIR)/wolf_sv_ccmds.o \
		$(ODIR)/wolf_common.o \
		$(ODIR)/wolf_actor_ai.o \
		$(ODIR)/wolf_actors.o \
		$(ODIR)/wolf_ai_com.o \
		$(ODIR)/wolf_player.o \
		$(ODIR)/wolf_weapon.o \
		$(ODIR)/wolf_areas.o \
		$(ODIR)/wolf_doors.o \
		$(ODIR)/wolf_level.o \
		$(ODIR)/wolf_pushwalls.o \
		$(ODIR)/wolf_math.o \
		$(ODIR)/wolf_mcredits.o \
		$(ODIR)/wolf_menu.o \
		$(ODIR)/wolf_mintermission.o \
		$(ODIR)/wolf_mintro.o \
		$(ODIR)/wolf_mnewgame.o \
		$(ODIR)/wolf_moptions.o \
		$(ODIR)/wolf_mquit.o \
		$(ODIR)/wolf_msound.o \
		$(ODIR)/wolf_mvideo.o \
		$(ODIR)/episode.o \
		$(ODIR)/wolf_raycast.o \
		$(ODIR)/wolf_renderer.o \
		$(ODIR)/wolf_opengl.o

all: config.h all-recursive $(EXE)

# Leave out the ALLOCA obj, because its header requires special
# modification that comes by actually using gnulib properly
$(EXE): $(PROGRAM_OBJS) $(LIBOBJS)
	${COMPILER} -o $(EXE) $(PROGRAM_OBJS) $(LIBOBJS) ${LDFLAGS} $(XLDFLAGS) $(VORBISFLAGS) -lm 


#==========================================================================
#	Build
#==========================================================================

 
#
#	client/ 
#
$(ODIR)/client_input.o : client/client_input.c 
	$(DO_CC)

$(ODIR)/client_main.o : client/client_main.c 
	$(DO_CC)

$(ODIR)/client_screen.o : client/client_screen.c 
	$(DO_CC)

$(ODIR)/console.o : client/console.c 
	$(DO_CC)

$(ODIR)/keys.o : client/keys.c 
	$(DO_CC)

$(ODIR)/menu_conf.o : client/menu_conf.c 
	$(DO_CC)

#
#	common/ 
#
$(ODIR)/arch.o : common/arch.c 
	$(DO_CC)

$(ODIR)/cmd.o : common/cmd.c 
	$(DO_CC)

$(ODIR)/common.o : common/common.c 
	$(DO_CC)

$(ODIR)/cvar.o : common/cvar.c 
	$(DO_CC)

$(ODIR)/share.o : common/share.c 
	$(DO_CC)

$(ODIR)/ttree.o : common/ttree.c 
	$(DO_CC)

#
#	device/CPU/ 
#
$(ODIR)/cpuid.o : device/cpu/cpuid.c 
	$(DO_CC)

#
#	device/filesys/ 
#
$(ODIR)/fileio.o : device/filesys/fileio.c 
	$(DO_CC)

$(ODIR)/files.o : device/filesys/files.c 
	$(DO_CC)

$(ODIR)/filestring.o : device/filesys/filestring.c 
	$(DO_CC)

$(ODIR)/zippak.o : device/filesys/zippak.c 
	$(DO_CC)

$(ODIR)/unix_file.o : device/filesys/unix/unix_file.c 
	$(DO_CC)

$(ODIR)/glob.o : device/filesys/unix/glob.c
	$(DO_CC)

#
#	device/input/
#
$(ODIR)/in_view.o : device/input/in_view.c 
	$(DO_CC)

$(ODIR)/unix_keys.o : device/input/unix/unix_keys.c 
	$(DO_CC)

$(ODIR)/unix_mouse.o : device/input/unix/unix_mouse.c 
	$(DO_CC)
	
$(ODIR)/unix_joystick.o : device/input/unix/unix_joystick.c 
	$(DO_CC)

#
#	device/system/
#
$(ODIR)/system.o : device/system/system.c 
	$(DO_CC)

$(ODIR)/unix_system.o : device/system/unix/unix_system.c 
	$(DO_CC)

#
#	device/timer/
#
$(ODIR)/unix_timer.o : device/timer/unix/unix_timer.c 
	$(DO_CC)

#
#	font/
#
$(ODIR)/font_manager.o : font/font_manager.c 
	$(DO_CC)

#
#	gnulib_replacement_funcs/
#
$(ODIR)/error.o : gnulib_replacement_funcs/error.c 
	$(DO_CC)

$(ODIR)/progname.o : gnulib_replacement_funcs/progname.c 
	$(DO_CC)

error.o: $(ODIR)/error.o $(ODIR)/progname.o
	test -e $(ODIR)/error.o && cp -v $(ODIR)/error.o .

progname.o: $(ODIR)/progname.o
	test -e $(ODIR)/progname.o && cp -v $(ODIR)/progname.o .

$(ODIR)/lstat.o : gnulib_replacement_funcs/lstat.c
	$(DO_CC)

lstat.o: $(ODIR)/lstat.o
	test -e $(ODIR)/lstat.o && cp -v $(ODIR)/lstat.o .

$(ODIR)/strnlen.o : gnulib_replacement_funcs/strnlen.c 
	$(DO_CC)

$(ODIR)/strnlen1.o : gnulib_replacement_funcs/strnlen1.c 
	$(DO_CC)

strnlen.o: $(ODIR)/strnlen.o $(ODIR)/strnlen1.o
	test -e $(ODIR)/strnlen.o && cp -v $(ODIR)/strnlen.o .

strnlen1.o: $(ODIR)/strnlen1.o
	test -e $(ODIR)/strnlen1.o && cp -v $(ODIR)/strnlen1.o .

#
#	loaders/
#
$(ODIR)/tga.o : loaders/tga.c 
	$(DO_CC)

$(ODIR)/wav.o : loaders/wav.c 
	$(DO_CC)

#
#	math/
#
$(ODIR)/angle.o : math/angle.c 
	$(DO_CC)

$(ODIR)/math.o : math/math.c 
	$(DO_CC)

$(ODIR)/matrix.o : math/matrix.c 
	$(DO_CC)

$(ODIR)/random_number.o : math/random_number.c 
	$(DO_CC)

$(ODIR)/vector.o : math/vector.c 
	$(DO_CC)


#
#	memory/
#
$(ODIR)/memory.o : memory/memory.c 
	$(DO_CC)

$(ODIR)/zmem.o : memory/zmem.c 
	$(DO_CC)

#
#	script/
#
$(ODIR)/script.o : script/script.c 
	$(DO_CC)

#
#	sound/
#
$(ODIR)/sound.o : sound/sound.c 
	$(DO_CC)

$(ODIR)/sound_sfx_id.o : sound/sound_sfx_id.c 
	$(DO_CC)

$(ODIR)/sound_stream.o : sound/sound_stream.c 
	$(DO_CC)

$(ODIR)/openal_binding.o : sound/openal/openal_binding.c 
	$(DO_CC)

$(ODIR)/openal_main.o : sound/openal/openal_main.c 
	$(DO_CC)

#
#	string/
#
$(ODIR)/com_string.o : string/com_string.c 
	$(DO_CC)


#
#	unix/
#
$(ODIR)/unix_main.o : unix/unix_main.c 
	$(DO_CC)

$(ODIR)/unix_vid_sys.o : unix/unix_vid_sys.c 
	$(DO_CC)

	
#
#	video/
#
$(ODIR)/texture_manager.o : video/texture_manager.c 
	$(DO_CC)


#
#	video/opengl/
#
$(ODIR)/opengl_binding.o : video/opengl/opengl_binding.c
	$(DO_CC)

$(ODIR)/opengl_draw.o : video/opengl/opengl_draw.c 
	$(DO_CC)

$(ODIR)/opengl_extension.o : video/opengl/opengl_extension.c 
	$(DO_CC)

$(ODIR)/opengl_main.o : video/opengl/opengl_main.c 
	$(DO_CC)

$(ODIR)/opengl_texture.o : video/opengl/opengl_texture.c 
	$(DO_CC)

# this file seems not to exist yet?
$(ODIR)/opengl_interface.o : video/opengl/unix/opengl_interface.c 
	$(DO_CC)


#
#	video/opengl/unix/
#
$(ODIR)/openglw_imp.o : video/opengl/unix/openglw_imp.c 
	$(DO_CC)

#
#	zlib/
#
$(ODIR)/adler32.o : zlib/adler32.c 
	$(DO_CC)

$(ODIR)/crc32.o : zlib/crc32.c 
	$(DO_CC)

$(ODIR)/inffast.o : zlib/inffast.c 
	$(DO_CC)

$(ODIR)/inflate.o : zlib/inflate.c 
	$(DO_CC)

$(ODIR)/inftrees.o : zlib/inftrees.c 
	$(DO_CC)

$(ODIR)/zutil.o : zlib/zutil.c 
	$(DO_CC)

##
##	game/wolf3d/
##
$(ODIR)/wolf_main.o : game/wolf3d/wolf_main.c 
	$(DO_CC)

#
#	game/wolf3d/client/
#
$(ODIR)/wolf_client_main.o : game/wolf3d/client/wolf_client_main.c 
	$(DO_CC)

$(ODIR)/wolf_sv_ccmds.o : game/wolf3d/client/wolf_sv_ccmds.c 
	$(DO_CC)

#
#	game/wolf3d/common/
#
$(ODIR)/wolf_common.o : game/wolf3d/common/wolf_common.c 
	$(DO_CC)

#
#	game/wolf3d/entities/
#
$(ODIR)/wolf_actor_ai.o : game/wolf3d/entities/wolf_actor_ai.c 
	$(DO_CC)

$(ODIR)/wolf_actors.o : game/wolf3d/entities/wolf_actors.c 
	$(DO_CC)

$(ODIR)/wolf_ai_com.o : game/wolf3d/entities/wolf_ai_com.c 
	$(DO_CC)

$(ODIR)/wolf_bj.o : game/wolf3d/entities/wolf_bj.c 
	$(DO_CC)

$(ODIR)/wolf_player.o : game/wolf3d/entities/wolf_player.c 
	$(DO_CC)

$(ODIR)/wolf_powerups.o : game/wolf3d/entities/wolf_powerups.c 
	$(DO_CC)

$(ODIR)/wolf_sprites.o : game/wolf3d/entities/wolf_sprites.c 
	$(DO_CC)

$(ODIR)/wolf_weapon.o : game/wolf3d/entities/wolf_weapon.c 
	$(DO_CC)



#
#	game/wolf3d/level/
#
$(ODIR)/wolf_areas.o : game/wolf3d/level/wolf_areas.c 
	$(DO_CC)

$(ODIR)/wolf_doors.o : game/wolf3d/level/wolf_doors.c 
	$(DO_CC)

$(ODIR)/wolf_level.o : game/wolf3d/level/wolf_level.c 
	$(DO_CC)

$(ODIR)/wolf_pushwalls.o : game/wolf3d/level/wolf_pushwalls.c 
	$(DO_CC)

#
#	game/wolf3d/math/
#
$(ODIR)/wolf_math.o : game/wolf3d/math/wolf_math.c 
	$(DO_CC)

#
#	game/wolf3d/menu/
#
$(ODIR)/wolf_mcredits.o : game/wolf3d/menu/wolf_mcredits.c 
	$(DO_CC)

$(ODIR)/wolf_menu.o : game/wolf3d/menu/wolf_menu.c 
	$(DO_CC)

$(ODIR)/wolf_mintermission.o : game/wolf3d/menu/wolf_mintermission.c 
	$(DO_CC)

$(ODIR)/wolf_mintro.o : game/wolf3d/menu/wolf_mintro.c 
	$(DO_CC)

$(ODIR)/wolf_mnewgame.o : game/wolf3d/menu/wolf_mnewgame.c 
	$(DO_CC)

$(ODIR)/wolf_moptions.o : game/wolf3d/menu/wolf_moptions.c 
	$(DO_CC)

$(ODIR)/wolf_mquit.o : game/wolf3d/menu/wolf_mquit.c 
	$(DO_CC)

$(ODIR)/wolf_msound.o : game/wolf3d/menu/wolf_msound.c 
	$(DO_CC)

$(ODIR)/wolf_mvideo.o : game/wolf3d/menu/wolf_mvideo.c 
	$(DO_CC)

#
#	game/wolf3d/script/
#
$(ODIR)/episode.o : game/wolf3d/script/episode.c
	$(DO_CC)

#
#	game/wolf3d/video/
#
$(ODIR)/wolf_raycast.o : game/wolf3d/video/wolf_raycast.c 
	$(DO_CC)

$(ODIR)/wolf_renderer.o : game/wolf3d/video/wolf_renderer.c 
	$(DO_CC)

#
#	game/wolf3d/video/opengl/
#
$(ODIR)/wolf_opengl.o : game/wolf3d/video/opengl/wolf_opengl.c 
	$(DO_CC)

#==========================================================================
#	Misc
#==========================================================================
clean-local:
	rm -f *.o $(ODIR)/*.o $(EXE)
endif

DISTCLEANFILES = *~

distclean-local:
	rm -rf autom4te.cache || rmdir autom4te.cache
