# Since we used to leave AM_INIT_AUTOMAKE out of configure.ac, this used to
# only exists to make sure that m4 macros are handled properly.
# The old makefile is still handwritten (as makefile_orig).

ACLOCAL_AMFLAGS = -I m4 --install

#
# Project: Wolfenstein 3-D Redux
#
# Program: wolf3dredux
#

#DONE: autoconf now checks that all warning flags are supported:
WARNFLAGS_COMMON = @WARN_CFLAGS@
AM_WARNFLAGS = $(WARNFLAGS_COMMON) @EXTRA_WARN_CFLAGS@
CFLAGS_COMMON = -ffast-math -funroll-loops
AM_CFLAGS = $(AM_WARNFLAGS) $(CFLAGS_COMMON)
AM_CPPFLAGS = -I. -I/usr/X11R6/include
AM_CXXFLAGS = -fno-rtti $(WARN_CXXFLAGS)
AM_LDFLAGS = -L. -L$(LIBOBJDIR)

GLXINFO = @GLXINFO@
SCAN_BUILD = @SCAN_BUILD@
LLVM_SYMBOLIZER = @LLVM_SYMBOLIZER@
ASAN_SYMBOLIZER_PATH = $(LLVM_SYMBOLIZER)

if CLANG_RULES
## DONE: the configure script should now detect a valid version of clang:
COMPILER = @CLANG_ANALYZER@
## TODO: make sure that all of these flags work and are used properly when
## building normally, instead of just when building the 'analyze' target:
DIAGFLAGS = -fcolor-diagnostics -fdiagnostics-print-source-range-info -fdiagnostics-show-note-include-stack -fdiagnostics-show-option
WARNFLAGS = $(WARNFLAGS_COMMON) -Wunused-label -Wshorten-64-to-32
# (cannot sanitize both thread and address at the same time)
SANFLAGS = # -fsanitize=bounds -fsanitize=undefined
OPTFLAGS = -g -gdwarf-2 -ggdb -O1 -msse3 -mssse3 -msse4.1 -msse4.2 
CLANG_CFLAGS = $(OPTFLAGS) -std=gnu99 $(WARNFLAGS) -fasm-blocks -fexceptions -fno-limit-debug-info -fno-omit-frame-pointer -fpascal-strings $(CFLAGS_COMMON) -mfix-and-continue $(DIAGFLAGS) $(SANFLAGS)
AM_CFLAGS += $(CLANG_CFLAGS)
AM_CLANG_CPPFLAGS = -I/opt/local/include
AM_CPPFLAGS += $(CLANG_CPPFLAGS)
# need to link with sanitizer flags, too, to get necessary symbols:
AM_CLANG_LINKFLAGS = -L/opt/local/lib $(SANFLAGS) -v # -exported_symbols_list ../Wolfenstein3DFromSource.exp
AM_LDFLAGS += $(AM_CLANG_LINKFLAGS)

## put this inside the clang rules conditional because I have only tested
## these flags with clang:
if MS_COMPAT_CHECK
# turn off warnings that actually are just coming from the Wine headers:
CLANG_MS_COMPAT_WARNFLAGS = $(WARNFLAGS_COMMON) -Wno-unknown-pragmas -Wno-ignored-attributes -Wno-implicit-function-declaration -Wno-int-conversion -Wno-uninitialized
_X_C_FLAG = -x c
CLANG_MS_COMPAT_CFLAGS = -fasm-blocks -fborland-extensions -fexceptions -ffast-math -fms-compatibility -fms-extensions -fmsc-version=601 -fno-builtin -fno-common -fvisibility-ms-compat -funroll-loops -mfix-and-continue -mms-bitfields -msse3 -mssse3 -msse4.1 -msse4.2 -m32 -g -O0 -arch i386 -std=c99 $(CLANG_MS_COMPAT_WARNFLAGS)
AM_CFLAGS += $(CLANG_MS_COMPAT_CFLAGS)
AM_CLANG_MS_COMPAT_CPPFLAGS = -D_MSC_VER=601 -D_WIN32 -I/opt/local/include/wine -I/opt/local/include/wine -I/opt/local/include/wine/msvcrt -I/opt/local/include/wine/windows $(AM_CLANG_CPPFLAGS)
AM_CPPFLAGS += $(AM_CLANG_MS_COMPAT_CPPFLAGS)
AM_CLANG_MS_COMPAT_LINKFLAGS = -m32 -arch i386
AM_LDFLAGS += $(AM_CLANG_MS_COMPAT_LINKFLAGS)
endif
## (end MS_COMPAT_CHECK)

SCAN_BUILD_CHECKERS = -enable-checker alpha.core.CastSize -enable-checker alpha.core.CastToStruct -enable-checker alpha.core.FixedAddr -enable-checker alpha.core.PointerArithm -enable-checker alpha.core.PointerSub -enable-checker alpha.core.SizeofPtr -enable-checker alpha.security.ArrayBound -enable-checker alpha.security.ArrayBoundV2 -enable-checker alpha.security.MallocOverflow -enable-checker alpha.security.ReturnPtrRange -enable-checker alpha.unix.SimpleStream -enable-checker alpha.unix.Stream -enable-checker alpha.unix.cstring.BufferOverlap -enable-checker alpha.unix.cstring.NotNullTerminated -enable-checker alpha.unix.cstring.OutOfBounds -enable-checker security.insecureAPI.rand -enable-checker security.insecureAPI.strcpy -enable-checker security.insecureAPI.vfork -enable-checker llvm.Conventions -enable-checker alpha.deadcode.UnreachableCode -enable-checker security.FloatLoopCounter
else
## not CLANG_RULES
if MS_COMPAT_CHECK
# turn off warnings that actually are just coming from the Wine headers:
MS_COMPAT_WARNFLAGS = $(WARNFLAGS_COMMON) -Wno-unknown-pragmas -Wno-implicit-function-declaration -Wno-uninitialized
MS_COMPAT_CFLAGS = -fasm-blocks -fexceptions -ffast-math -fms-extensions -fno-builtin -fno-common -funroll-loops -mfix-and-continue -mms-bitfields -msse3 -mssse3 -msse4.1 -msse4.2 -m32 -g -O0 -arch i386 -std=c99 $(MS_COMPAT_WARNFLAGS) 
AM_CFLAGS += $(MS_COMPAT_CFLAGS)
MS_COMPAT_CXXFLAGS = -fvisibility-ms-compat
AM_CXXFLAGS += $(MS_COMPAT_CXXFLAGS)
AM_MS_COMPAT_CPPFLAGS = -D_MSC_VER=601 -D_WIN32 -I/opt/local/include/wine -I/opt/local/include/wine -I/opt/local/include/wine/msvcrt -I/opt/local/include/wine/windows -I/opt/local/include
AM_CPPFLAGS += $(AM_MS_COMPAT_CPPFLAGS)
AM_MS_COMPAT_LINKFLAGS = -m32 -arch i386
AM_LDFLAGS += $(AM_MS_COMPAT_LINKFLAGS)
endif
## (end MS_COMPAT_CHECK)
endif
## (end CLANG_RULES)

if WOLF_OLD_BUILD
COMPILER = @CC@
NON_AM_CFLAGS = -c
OLD_LDFLAGS_UNUSED =  -s
XLDFLAGS = -L/usr/X11R6/lib @LIBS@
if BUILD_OBJC_SOURCES
# Building Objective C sources requires linking agains frameworks:
XLDFLAGS += -framework Cocoa
endif
## (end BUILD_OBJC_SOURCES)
VORBISFLAGS = @VORBIS_LIBS@
ODIR = Release
EXEBASE = wolf3dredux
EXE = $(ODIR)/$(EXEBASE)
endif
## (end WOLF_OLD_BUILD)

## should contain a "/" at the end:
LIBOBJDIR = @LIBOBJDIR@
## there is also a version without the slash, in case the slash is a
## problem:
LIBOBJDIRNOSLASH = @LIBOBJDIRNOSLASH@

SUBDIRS = $(LIBOBJDIRNOSLASH) . tools/wolfextractor

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
                      device/input/in_view.c \
                      device/system/system.c \
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
                      video/texture_manager.c \
                      video/opengl/opengl_binding.c \
                      video/opengl/opengl_draw.c \
                      video/opengl/opengl_extension.c \
                      video/opengl/opengl_main.c \
                      video/opengl/opengl_texture.c \
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
## TODO: enumerate headers under "noinst_HEADERS"

if MS_COMPAT_CHECK
wolf3dredux_SOURCES += device/filesys/win32/win_file.c \
                       device/input/win32/win_joystick.c \
                       device/input/win32/win_keys.c \
                       device/input/win32/win_mouse.c \
                       device/system/win32/win_system.c \
                       device/timer/win32/win_timer.c \
                       video/opengl/win32/openglw_imp.c \
                       win32/vid_sys.c \
                       win32/win_main.c
else
## (not MS_COMPAT_CHECK -- i.e. standard unix build:)
wolf3dredux_SOURCES += device/filesys/unix/unix_file.c \
                       device/filesys/unix/glob.c \
                       device/input/unix/unix_keys.c \
                       device/input/unix/unix_mouse.c \
                       device/input/unix/unix_joystick.c \
                       device/system/unix/unix_system.c \
                       device/timer/unix/unix_timer.c \
                       unix/unix_main.c \
                       unix/unix_vid_sys.c \
                       video/opengl/unix/openglw_imp.c

## only makes sense to do these when NOT doing MS_COMPAT_CHECK:
if BUILD_X11_SOURCES
wolf3dredux_SOURCES += unix/clipboard/X11/X11Clipboard.c
endif
## (end BUILD_X11_SOURCES)

## only makes sense to do these when NOT doing MS_COMPAT_CHECK:
if BUILD_OBJC_SOURCES
wolf3dredux_SOURCES += unix/clipboard/OSX/OSXClipboard.m \
                       unix/clipboard/OSX/ASCIIString.m \
                       unix/clipboard/OSX/HexAndASCIIString.m \
                       unix/clipboard/OSX/HexString.m \
                       unix/clipboard/OSX/LazyDataTextStorage.m \
                       unix/clipboard/OSX/PasteboardTypeTransformer.m
AM_OBJCFLAGS = -fobjc-exceptions -fno-strict-aliasing $(GNU_OBJC_WARN_CFLAGS)
# Building Objective C sources requires linking agains frameworks:
AM_OBJCLDFLAGS = -framework Cocoa
endif
## (end BUILD_OBJC_SOURCES)

endif
## (end MS_COMPAT_CHECK)

# override these two to avoid the "unable to infer tagged configuration"
# libtool error that comes from mixing C and Objective C:
LTOBJCCOMPILE = $(LIBTOOL) $(AM_V_lt) --tag=CC $(AM_LIBTOOLFLAGS) \
	$(LIBTOOLFLAGS) --mode=compile $(OBJC) $(DEFS) \
	$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_OBJCFLAGS) $(OBJCFLAGS)
OBJCLINK = $(LIBTOOL) $(AM_V_lt) --tag=CC $(AM_LIBTOOLFLAGS) \
	$(LIBTOOLFLAGS) --mode=link $(OBJCLD) $(AM_OBJCFLAGS)  \
	$(OBJCFLAGS) $(AM_LDFLAGS) $(LDFLAGS) $(AM_OBJCLDFLAGS) \
	$(OBJCLDFLAGS) -o $@
# (these need to be overriden unconditionally, because for some reason
# automake uses the Objective C linker even if the Objective C sources
# have not actually been added in that condition...)

wolf3dredux_LDADD = $(LIBOBJDIR)libgnu.a
EXTRA_wolf3dredux_DEPENDENCIES = $(LIBOBJDIR)libgnu.a

# note the space in the rule:
$(LIBOBJDIR)libgnu.a: $(LIBOBJDIR)Makefile
	$(MAKE) -C $(LIBOBJDIRNOSLASH) libgnu.a

noinst_PROGRAMS = gamepal signon

gamepal_SOURCES = tools/gamepal/main.c
signon_SOURCES = tools/signon/wolf_pal.c \
                 tools/signon/main.c \
                 tools/signon/WriteTGA_minimal.c

XSyms.txt: wolf3dredux
	for Xlib in `$(OTOOL) -L wolf3dredux | $(GREP) -i libX | cut -d\( -f1 | tr -d [:blank:] | xargs basename | cut -d. -f1`; do $(NM) -m wolf3dredux | $(GREP) $${Xlib}; done | tee XSyms.txt

if WOLF_OLD_BUILD
DO_CC = $(COMPILER) $(CFLAGS) $(AM_CFLAGS) $(AM_CPPFLAGS) ${CPPFLAGS} -o $@ -c $<
XOBJCFLAGS = -x objective-c -fobjc-exceptions
DO_OBJCC = $(COMPILER) $(CFLAGS) $(AM_CFLAGS) $(OBJCFLAGS) $(XOBJCFLAGS) $(AM_OBJCFLAGS) $(OBJCPPFLAGS) $(AM_OBJCPPFLAGS) $(AM_CPPFLAGS) ${CPPFLAGS} -o $@ -c $<

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
## TODO: make old rules respect MS_COMPAT_CHECK as well

if BUILD_X11_SOURCES
PROGRAM_OBJS += $(ODIR)/X11Clipboard.o
endif
## (end BUILD_X11_SOURCES)

if BUILD_OBJC_SOURCES
PROGRAM_OBJS += $(ODIR)/OSXClipboard.o \
                $(ODIR)/ASCIIString.o \
                $(ODIR)/HexAndASCIIString.o \
                $(ODIR)/HexString.o \
                $(ODIR)/LazyDataTextStorage.o \
                $(ODIR)/PasteboardTypeTransformer.o
endif
## (end BUILD_OBJC_SOURCES)

all: config.h all-recursive $(EXE)

# Leave out the ALLOCA obj, because its header requires special
# modification that comes by actually using gnulib properly
$(EXE): $(PROGRAM_OBJS) $(LIBOBJS)
	${COMPILER} -o $(EXE) $(PROGRAM_OBJS) $(LIBOBJS) $(AM_LDFLAGS) ${LDFLAGS} $(XLDFLAGS) $(VORBISFLAGS) -lm 


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
if BUILD_OBJC_SOURCES
	$(DO_OBJCC)
else
## not BUILD_OBJC_SOURCES:
	$(DO_CC)
endif
## (end BUILD_OBJC_SOURCES)

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

# this file seems not to exist yet? TODO: add/create it...
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

#
#	unix/clipboard/
#
if BUILD_X11_SOURCES
$(ODIR)/X11Clipboard.o : unix/clipboard/X11/X11Clipboard.c
	$(DO_CC)
endif
## (end BUILD_X11_SOURCES)

if BUILD_OBJC_SOURCES
$(ODIR)/OSXClipboard.o : unix/clipboard/OSX/OSXClipboard.m
	$(DO_OBJCC)

$(ODIR)/ASCIIString.o : unix/clipboard/OSX/ASCIIString.m
	$(DO_OBJCC)

$(ODIR)/HexAndASCIIString.o : unix/clipboard/OSX/HexAndASCIIString.m
	$(DO_OBJCC)

$(ODIR)/HexString.o : unix/clipboard/OSX/HexString.m
	$(DO_OBJCC)

$(ODIR)/LazyDataTextStorage.o : unix/clipboard/OSX/LazyDataTextStorage.m
	$(DO_OBJCC)

$(ODIR)/PasteboardTypeTransformer.o : unix/clipboard/OSX/PasteboardTypeTransformer.m
	$(DO_OBJCC)

endif
## (end BUILD_OBJC_SOURCES)

#==========================================================================
#	Misc
#==========================================================================
clean-local:
	rm -f *.o $(ODIR)/*.o $(EXE)
	rm -rf *.dSYM || rmdir *.dSYM
endif
## (end WOLF_OLD_BUILD)

if CLANG_RULES
analyze: clean ../Wolfenstein3DFromSource.exp
	$(SCAN_BUILD) --use-analyzer=$(CLANG_ANALYZER) $(SCAN_BUILD_CHECKERS) -v -plist-html -o scan_build_output $(MAKE) AM_CFLAGS="" CFLAGS="$(CLANG_CFLAGS)" AM_CPPFLAGS="-I." CPPFLAGS="$(AM_CLANG_CPPFLAGS)" AM_LDFLAGS="-L." LDFLAGS="$(AM_CLANG_LINKFLAGS)" $(LIBOBJDIR)libgnu.a wolf3dredux
	test -e tools/wolfextractor/Makefile && $(MAKE) -C tools/wolfextractor analyze

if MS_COMPAT_CHECK
## this target is not really as necessary any more now that mswin files
## are built as part of the normal build process instead of in a separate
## static library...
ms-compat: clean
	 $(MAKE) AM_CFLAGS="" CFLAGS="$(CLANG_MS_COMPAT_CFLAGS)" AM_CPPFLAGS="-I." CPPFLAGS="$(AM_CLANG_MS_COMPAT_CPPFLAGS)" AM_LDFLAGS="-L." LDFLAGS=""
else
## (not MS_COMPAT_CHECK)
ms-compat:
	@echo "the ms-compat target is not supported with your current configuration."
endif
## (end MS_COMPAT_CHECK)

else
## (not CLANG_RULES)
analyze:
	@echo "you must configure with CC set to a compiler that is clang to be able to use this target."
endif
## (end CLANG_RULES)

DISTCLEANFILES = *~

distclean-local:
	rm -rf autom4te.cache || rmdir autom4te.cache
