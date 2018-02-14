# The old, handwritten makefile for this subdir still exists as here as
# makefile.orig

#
# Project: Wolfenstein 3-D Redux
#
# Program: wolfextractor
#

if CLANG_RULES
## TODO: instead of hardcoding this, detect a valid version of clang:
COMPILER = /opt/local/libexec/llvm-3.3/bin/clang
## TODO: make sure that all of these flags work and are used properly when
## building normally, instead of just when building the 'analyze' target:
DIAGFLAGS = -fcolor-diagnostics -fdiagnostics-print-source-range-info -fdiagnostics-show-note-include-stack -fdiagnostics-show-option
WARNFLAGS = -Wall -Wno-trigraphs -Wmissing-field-initializers -Wmissing-prototypes -Wreturn-type -Wmissing-braces -Wparentheses -Wswitch -Wunused-function -Wunused-label -Wunused-parameter -Wunused-variable -Wshorten-64-to-32 -Wimplicit
# (cannot sanitize both thread and address at the same time)
SANFLAGS = # -fsanitize=bounds -fsanitize=undefined
OPTFLAGS = -g -gdwarf-2 -ggdb -O1 -msse3 -mssse3 -msse4.2 -msse4.2 
AM_CFLAGS = $(OPTFLAGS) -std=gnu99 $(WARNFLAGS) -fasm-blocks -fexceptions -fno-common -fno-limit-debug-info -fno-omit-frame-pointer -fpascal-strings -funroll-loops -ffast-math -mfix-and-continue $(DIAGFLAGS) $(SANFLAGS)
AM_CPPFLAGS = -I/opt/local/include
# need to link with sanitizer flags, too, to get necessary symbols:
AM_LDFLAGS = -L/opt/local/lib $(SANFLAGS)
endif
## (end CLANG_RULES)

if WOLF_OLD_BUILD
COMPILER = @CC@
NON_AM_CFLAGS = -Wall -c -O2
OLD_LDFLAGS_UNUSED = -s
AM_LIBS = -lvorbis -lvorbisenc -logg -lm
EXODIR = Releasedir
EXEBASE = wolfextractor
EXE = $(EXODIR)/$(EXEBASE)
endif
## (end WOLF_OLD_BUILD)

SCAN_BUILD = @SCAN_BUILD@
DSYMUTIL = @DSYMUTIL@

if WOLF_OLD_BUILD
DO_CC = $(COMPILER) $(CFLAGS) $(AM_CFLAGS) $(AM_CPPFLAGS) ${CPPFLAGS} -o $@ -c $<       
endif
## (end WOLF_OLD_BUILD)

if CLANG_RULES
SCAN_BUILD_CHECKERS = -enable-checker alpha.core.CastSize -enable-checker alpha.core.CastToStruct -enable-checker alpha.core.FixedAddr -enable-checker alpha.core.PointerArithm -enable-checker alpha.core.PointerSub -enable-checker alpha.core.SizeofPtr -enable-checker alpha.security.ArrayBound -enable-checker alpha.security.ArrayBoundV2 -enable-checker alpha.security.MallocOverflow -enable-checker alpha.security.ReturnPtrRange -enable-checker alpha.unix.SimpleStream -enable-checker alpha.unix.Stream -enable-checker alpha.unix.cstring.BufferOverlap -enable-checker alpha.unix.cstring.NotNullTerminated -enable-checker alpha.unix.cstring.OutOfBounds -enable-checker security.insecureAPI.rand -enable-checker security.insecureAPI.strcpy -enable-checker security.insecureAPI.vfork -enable-checker llvm.Conventions -enable-checker alpha.deadcode.UnreachableCode -enable-checker security.FloatLoopCounter
endif
## (end CLANG_RULES)

if WOLF_OLD_BUILD
all: ../../config.h $(EXE)
endif
## (end WOLF_OLD_BUILD)

if CLANG_RULES
analyze: clean Makefile
	$(SCAN_BUILD) --use-analyzer=$(CLANG_ANALYZER) $(SCAN_BUILD_CHECKERS) -v -plist-html -o scan_build_output $(MAKE)
endif
## (end WOLF_OLD_BUILD)

if WOLF_OLD_BUILD
clean-local:
	rm -f $(EXODIR)/*.o $(EXE)
	rm -f string/*.o
	rm -f string/test_com_string
	rm -f Debug/*/*.o
	rm -f Debug/tools/wolfextractor/*/*.o
	rm -f Debug/tools/wolfextractor/*/*/*.o
endif
## (end WOLF_OLD_BUILD)

noinst_PROGRAMS = wolfextractor

wolfextractor_SOURCES = adlib/adlib.c \
		        adlib/fmopl.c \
		        ../../common/arch.c \
		        filesys/file.c \
			string/com_string.c \
		        loaders/tga.c \
		        loaders/wav.c \
		        memory/memory.c \
		        mac/mac.c \
		        wolf/wl6_name.c \
		        wolf/wolf_aud.c \
		        wolf/wolf_gfx.c \
		        wolf/wolf_map.c \
		        wolf/wolf_pal.c \
		        wolf/wolf_pm.c \
		        zip/zipfile.c \
		        ../../zlib/adler32.c \
		        ../../zlib/compress.c \
		        ../../zlib/crc32.c \
		        ../../zlib/deflate.c \
		        ../../zlib/trees.c \
		        ../../zlib/zutil.c \
		        hq2x.c \
		        pak.c \
		        vorbisenc_inter.c \
		        main.c
## TODO: enumerate headers under "noinst_HEADERS"

if MS_COMPAT_CHECK
wolfextractor_SOURCES += filesys/win32/win_file.c
else
## (not MS_COMPAT_CHECK -- i.e. standard unix build:)
wolfextractor_SOURCES += filesys/unix/unix_file.c \
		         filesys/unix/glob.c
endif
## (end MS_COMPAT_CHECK)


check_PROGRAMS = test_com_string

test_com_string_SOURCES = string/test_com_string.c \
                          string/com_string.c

test_com_string_CFLAGS = -Wall -ggdb

if WOLF_OLD_BUILD
PROGRAM_FILES = $(EXODIR)/adlib.o \
		$(EXODIR)/fmopl.o \
		$(EXODIR)/arch.o \
		$(EXODIR)/file.o \
		$(EXODIR)/unix_file.o \
		$(EXODIR)/glob.o \
		$(EXODIR)/com_string.o \
		$(EXODIR)/tga.o \
		$(EXODIR)/wav.o \
		$(EXODIR)/memory.o \
		$(EXODIR)/mac.o \
		$(EXODIR)/wl6_name.o \
		$(EXODIR)/wolf_aud.o \
		$(EXODIR)/wolf_gfx.o \
		$(EXODIR)/wolf_map.o \
		$(EXODIR)/wolf_pal.o \
		$(EXODIR)/wolf_pm.o \
		$(EXODIR)/zipfile.o \
		$(EXODIR)/adler32.o \
		$(EXODIR)/compress.o \
		$(EXODIR)/crc32.o \
		$(EXODIR)/deflate.o \
		$(EXODIR)/trees.o \
		$(EXODIR)/zutil.o \
		$(EXODIR)/hq2x.o \
		$(EXODIR)/pak.o \
		$(EXODIR)/vorbisenc_inter.o \
		$(EXODIR)/main.o
## TODO: make old rules respect MS_COMPAT_CHECK as well


$(EXE): $(PROGRAM_FILES) 
	${COMPILER} -o $(EXE) $(PROGRAM_FILES) $(AM_LDFLAGS) ${LDFLAGS} $(AM_LIBS) $(LIBS)
	$(DSYMUTIL) $(EXE)



#==========================================================================
#	Build
#==========================================================================
   
										

#
#	adlib/
#
$(EXODIR)/adlib.o : adlib/adlib.c 
	$(DO_CC)

$(EXODIR)/fmopl.o : adlib/fmopl.c 
	$(DO_CC)

#
#	common/
#
$(EXODIR)/arch.o : ../../common/arch.c 
	$(DO_CC)

#
#	filesys/
#
$(EXODIR)/file.o : filesys/file.c 
	$(DO_CC)

#
#	filesys/unix/
#
$(EXODIR)/unix_file.o : filesys/unix/unix_file.c 
	$(DO_CC)

$(EXODIR)/glob.o : filesys/unix/glob.c 
	$(DO_CC)

#
#	loaders/
#
$(EXODIR)/tga.o : loaders/tga.c 
	$(DO_CC)

$(EXODIR)/wav.o : loaders/wav.c 
	$(DO_CC)

#
#	mac/
#
$(EXODIR)/mac.o : mac/mac.c 
	$(DO_CC)

#
#	memory/
#
$(EXODIR)/memory.o : memory/memory.c 
	$(DO_CC)

#
#	string/
#
$(EXODIR)/com_string.o : string/com_string.c 
	$(DO_CC)

#
#	wolf/
#
$(EXODIR)/wl6_name.o : wolf/wl6_name.c 
	$(DO_CC)

$(EXODIR)/wolf_aud.o : wolf/wolf_aud.c 
	$(DO_CC)

$(EXODIR)/wolf_gfx.o : wolf/wolf_gfx.c 
	$(DO_CC)

$(EXODIR)/wolf_map.o : wolf/wolf_map.c 
	$(DO_CC)

$(EXODIR)/wolf_pal.o : wolf/wolf_pal.c 
	$(DO_CC)

$(EXODIR)/wolf_pm.o : wolf/wolf_pm.c 
	$(DO_CC)

#
#	zip/
#
$(EXODIR)/zipfile.o : zip/zipfile.c 
	$(DO_CC)

#
#	zlib/
#
$(EXODIR)/adler32.o : ../../zlib/adler32.c 
	$(DO_CC)

$(EXODIR)/compress.o : ../../zlib/compress.c 
	$(DO_CC)

$(EXODIR)/crc32.o : ../../zlib/crc32.c 
	$(DO_CC)

$(EXODIR)/deflate.o : ../../zlib/deflate.c 
	$(DO_CC)

$(EXODIR)/trees.o : ../../zlib/trees.c 
	$(DO_CC)

$(EXODIR)/zutil.o : ../../zlib/zutil.c 
	$(DO_CC)

#
#
#
$(EXODIR)/hq2x.o : hq2x.c 
	$(DO_CC)

$(EXODIR)/pak.o : pak.c 
	$(DO_CC)

$(EXODIR)/vorbisenc_inter.o : vorbisenc_inter.c
	$(DO_CC)

$(EXODIR)/main.o : main.c 
	$(DO_CC)

endif
## (end WOLF_OLD_BUILD)


