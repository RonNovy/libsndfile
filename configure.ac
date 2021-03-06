# Copyright (C) 1999-2014  Erik de Castro Lopo <erikd@mega-nerd.com>.

dnl Require autoconf version
AC_PREREQ(2.57)

AC_INIT([libsndfile],[1.0.26pre5],[sndfile@mega-nerd.com],
		[libsndfile],[http://www.mega-nerd.com/libsndfile/])

# Put config stuff in Cfg.
AC_CONFIG_AUX_DIR(Cfg)
AC_CONFIG_MACRO_DIR([M4])

AC_CONFIG_SRCDIR([src/sndfile.c])
AC_CANONICAL_TARGET([])

AC_CONFIG_MACRO_DIR([M4])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_LANG([C])

AC_PROG_CC_STDC
AC_USE_SYSTEM_EXTENSIONS
AM_PROG_CC_C_O
AC_PROG_CXX

MN_C_COMPILER_IS_CLANG
MN_GCC_REALLY_IS_GCC

AC_PROG_SED

# Do not check for F77.
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl

AM_PROG_LIBTOOL
LT_PROG_RC

AC_PROG_INSTALL
AC_PROG_LN_S

AC_CHECK_PROG(HAVE_AUTOGEN, autogen, yes, no)
AC_CHECK_PROG(HAVE_WINE, wine, yes, no)
AC_CHECK_PROG(HAVE_XCODE_SELECT, xcode-select, yes, no)

#------------------------------------------------------------------------------------
# Rules for library version information:
#
#  1. Start with version information of `0:0:0' for each libtool library.
#  2. Update the version information only immediately before a public release of
#     your software. More frequent updates are unnecessary, and only guarantee
#     that the current interface number gets larger faster.
#  3. If the library source code has changed at all since the last update, then
#     increment revision (`c:r:a' becomes `c:r+1:a').
#  4. If any interfaces have been added, removed, or changed since the last update,
#     increment current, and set revision to 0.
#  5. If any interfaces have been added since the last public release, then increment
#     age.
#  6. If any interfaces have been removed since the last public release, then set age
#     to 0.

CLEAN_VERSION=`echo $PACKAGE_VERSION | $SED "s/p.*//"`
VERSION_MINOR=`echo $CLEAN_VERSION | $SED "s/.*\.//"`

SHARED_VERSION_INFO="1:$VERSION_MINOR:0"

#------------------------------------------------------------------------------------

AC_HEADER_STDC

AC_CHECK_HEADERS(endian.h)
AC_CHECK_HEADERS(byteswap.h)
AC_CHECK_HEADERS(locale.h)
AC_CHECK_HEADERS(sys/time.h)

AC_HEADER_SYS_WAIT

AC_CHECK_DECLS(S_IRGRP)
if test x$ac_cv_have_decl_S_IRGRP = xyes ; then
	AC_DEFINE_UNQUOTED([HAVE_DECL_S_IRGRP],1,[Set to 1 if S_IRGRP is defined.])
else
	AC_DEFINE_UNQUOTED([HAVE_DECL_S_IRGRP],0)
	fi

AM_CONDITIONAL([LINUX_MINGW_CROSS_TEST],
	[test "$build_os:$target_os:$host_os:$HAVE_WINE" = "linux-gnu:mingw32msvc:mingw32msvc:yes"])

#====================================================================================
# Couple of initializations here. Fill in real values later.

SHLIB_VERSION_ARG=""

#====================================================================================
# Finished checking, handle options.

AC_ARG_ENABLE(experimental,
	AC_HELP_STRING([--enable-experimental], [enable experimental code]))

EXPERIMENTAL_CODE=0
if test x$enable_experimental = xyes ; then
	EXPERIMENTAL_CODE=1
	fi
AC_DEFINE_UNQUOTED([ENABLE_EXPERIMENTAL_CODE],${EXPERIMENTAL_CODE}, [Set to 1 to enable experimental code.])

AC_ARG_ENABLE(werror,
	AC_HELP_STRING([--enable-werror], [enable -Werror in all Makefiles]))

AC_ARG_ENABLE(stack-smash-protection,
       AC_HELP_STRING([--enable-stack-smash-protection], [Enable GNU GCC stack smash protection]))

AC_ARG_ENABLE(gcc-pipe,
	AC_HELP_STRING([--disable-gcc-pipe], [disable gcc -pipe option]))

AC_ARG_ENABLE(gcc-opt,
	AC_HELP_STRING([--disable-gcc-opt], [disable gcc optimisations]))

AC_ARG_ENABLE(cpu-clip,
	AC_HELP_STRING([--disable-cpu-clip], [disable tricky cpu specific clipper]))

AC_ARG_ENABLE(bow-docs,
	AC_HELP_STRING([--enable-bow-docs], [enable black-on-white html docs]))

AC_ARG_ENABLE(sqlite,
	AC_HELP_STRING([--disable-sqlite], [disable use of sqlite]))

AC_ARG_ENABLE(alsa,
	AC_HELP_STRING([--disable-alsa], [disable use of ALSA]))

AC_ARG_ENABLE(external-libs,
	AC_HELP_STRING([--disable-external-libs], [disable use of FLAC, Ogg and Vorbis [[default=no]]]))

AC_ARG_ENABLE(octave,
	AC_HELP_STRING([--enable-octave], [disable building of GNU Octave module]))

AC_ARG_ENABLE(test-coverage,
	AC_HELP_STRING([--enable-test-coverage], [enable test coverage]))
AM_CONDITIONAL([ENABLE_TEST_COVERAGE], [test "$enable_test_coverage" = yes])

AC_ARG_ENABLE(sanitizer,
	AC_HELP_STRING([--enable-sanitizer], [enable ggc/clang code sanitizier runtime checks]))

#====================================================================================
# Check types and their sizes.

AC_CHECK_SIZEOF(wchar_t,4)
AC_CHECK_SIZEOF(short,2)
AC_CHECK_SIZEOF(int,4)
AC_CHECK_SIZEOF(long,4)
AC_CHECK_SIZEOF(float,4)
AC_CHECK_SIZEOF(double,4)
AC_CHECK_SIZEOF(void*,8)
AC_CHECK_SIZEOF(size_t,4)
AC_CHECK_SIZEOF(int64_t,8)
AC_CHECK_SIZEOF(long long,8)

#====================================================================================
# Find an appropriate type for sf_count_t.
# On systems supporting files larger than 2 Gig, sf_count_t must be a 64 bit value.
# Unfortunately there is more than one way of ensuring this so need to do some
# pretty rigourous testing here.

# Check for common 64 bit file offset types.
AC_CHECK_SIZEOF(off_t,1)
AC_CHECK_SIZEOF(loff_t,1)
AC_CHECK_SIZEOF(off64_t,1)

if test "$enable_largefile:$ac_cv_sizeof_off_t" = "no:8" ; then
	echo
	echo "Error : Cannot disable large file support because sizeof (off_t) == 8."
	echo
	exit 1
	fi


case "$host_os" in
	mingw32*)
		TYPEOF_SF_COUNT_T="__int64"
		SF_COUNT_MAX="0x7FFFFFFFFFFFFFFFLL"
		SIZEOF_SF_COUNT_T=8
		AC_DEFINE([__USE_MINGW_ANSI_STDIO],1,[Set to 1 to use C99 printf/snprintf in MinGW.])
		;;
	*)
		SIZEOF_SF_COUNT_T=0
		if test "x$ac_cv_sizeof_off_t" = "x8" ; then
			# If sizeof (off_t) is 8, no further checking is needed.
			TYPEOF_SF_COUNT_T="int64_t"
			SF_COUNT_MAX="0x7FFFFFFFFFFFFFFFLL"
			SIZEOF_SF_COUNT_T=8
		elif test "x$ac_cv_sizeof_loff_t" = "x8" ; then
			TYPEOF_SF_COUNT_T="int64_t"
			SF_COUNT_MAX="0x7FFFFFFFFFFFFFFFLL"
			SIZEOF_SF_COUNT_T=8
		elif test "x$ac_cv_sizeof_off64_t" = "x8" ; then
			TYPEOF_SF_COUNT_T="int64_t"
			SF_COUNT_MAX="0x7FFFFFFFFFFFFFFFLL"
			SIZEOF_SF_COUNT_T=8
		else
			# Save the old sizeof (off_t) value  and then unset it to see if it
			# changes when Large File Support is enabled.
			pre_largefile_sizeof_off_t=$ac_cv_sizeof_off_t
			unset ac_cv_sizeof_off_t

			AC_SYS_LARGEFILE

			if test "x$ac_cv_sys_largefile_CFLAGS" = "xno" ; then
				ac_cv_sys_largefile_CFLAGS=""
				fi
			if test "x$ac_cv_sys_largefile_LDFLAGS" = "xno" ; then
				ac_cv_sys_largefile_LDFLAGS=""
				fi
			if test "x$ac_cv_sys_largefile_LIBS" = "xno" ; then
				ac_cv_sys_largefile_LIBS=""
				fi

			AC_CHECK_SIZEOF(off_t,1)

			if test "x$ac_cv_sizeof_off_t" = "x8" ; then
				TYPEOF_SF_COUNT_T="int64_t"
				SF_COUNT_MAX="0x7FFFFFFFFFFFFFFFLL"
			elif test "x$TYPEOF_SF_COUNT_T" = "xunknown" ; then
				echo
				echo "*** The configure process has determined that this system is capable"
				echo "*** of Large File Support but has not been able to find a type which"
				echo "*** is an unambiguous 64 bit file offset."
				echo "*** Please contact the author to help resolve this problem."
				echo
				AC_MSG_ERROR([[Bad file offset type.]])
				fi
			fi
		;;
	esac

if test $SIZEOF_SF_COUNT_T = 4 ; then
	SF_COUNT_MAX="0x7FFFFFFF"
	fi

AC_DEFINE_UNQUOTED([TYPEOF_SF_COUNT_T],${TYPEOF_SF_COUNT_T}, [Set to long if unknown.])
AC_SUBST(TYPEOF_SF_COUNT_T)

AC_DEFINE_UNQUOTED([SIZEOF_SF_COUNT_T],${SIZEOF_SF_COUNT_T}, [Set to sizeof (long) if unknown.])
AC_SUBST(SIZEOF_SF_COUNT_T)

AC_DEFINE_UNQUOTED([SF_COUNT_MAX],${SF_COUNT_MAX}, [Set to maximum allowed value of sf_count_t type.])
AC_SUBST(SF_COUNT_MAX)

AC_CHECK_TYPES(ssize_t)
AC_CHECK_SIZEOF(ssize_t,4)

#====================================================================================
# Determine endian-ness of target processor.

MN_C_FIND_ENDIAN

AC_DEFINE_UNQUOTED(CPU_IS_BIG_ENDIAN, ${ac_cv_c_big_endian},
	[Target processor is big endian.])
AC_DEFINE_UNQUOTED(CPU_IS_LITTLE_ENDIAN, ${ac_cv_c_little_endian},
	[Target processor is little endian.])
AC_DEFINE_UNQUOTED(WORDS_BIGENDIAN, ${ac_cv_c_big_endian},
	[Target processor is big endian.])

#====================================================================================
# Check for functions.

AC_CHECK_FUNCS(malloc calloc realloc free)
AC_CHECK_FUNCS(open read write lseek lseek64)
AC_CHECK_FUNCS(fstat fstat64 ftruncate fsync)
AC_CHECK_FUNCS(snprintf vsnprintf)
AC_CHECK_FUNCS(gmtime gmtime_r localtime localtime_r gettimeofday)
AC_CHECK_FUNCS(mmap getpagesize)
AC_CHECK_FUNCS(setlocale)
AC_CHECK_FUNCS(pipe waitpid)

AC_CHECK_LIB([m],floor)
AC_CHECK_FUNCS(floor ceil fmod lround)

MN_C99_FUNC_LRINT
MN_C99_FUNC_LRINTF

#====================================================================================
# Check for requirements for building plugins for other languages/enviroments.

dnl Octave maths environment http://www.octave.org/
if test x$cross_compiling = xno ; then
	if test x$enable_octave = xno ; then
		AM_CONDITIONAL(BUILD_OCTAVE_MOD, false)
	else
		AC_OCTAVE_BUILD
		fi
else
	AM_CONDITIONAL(BUILD_OCTAVE_MOD, false)
	fi

#====================================================================================
# Check for Ogg, Vorbis and FLAC.

HAVE_EXTERNAL_LIBS=0
EXTERNAL_CFLAGS=""
EXTERNAL_LIBS=""

# Check for pkg-config outside the if statement.
PKG_PROG_PKG_CONFIG
m4_ifdef([PKG_INSTALLDIR], [PKG_INSTALLDIR], AC_SUBST([pkgconfigdir], ${libdir}/pkgconfig))

if test -n "$PKG_CONFIG" ; then
	if test x$enable_external_libs = xno ; then
		AC_MSG_WARN([[*** External libs (FLAC, Ogg, Vorbis) disabled. ***]])
	else
		PKG_CHECK_MOD_VERSION(FLAC, flac >= 1.3.1, ac_cv_flac=yes, ac_cv_flac=no)

		# Make sure the FLAC_CFLAGS value is sane.
		FLAC_CFLAGS=`echo $FLAC_CFLAGS | $SED "s|include/FLAC|include|"`

		PKG_CHECK_MOD_VERSION(OGG, ogg >= 1.1.3, ac_cv_ogg=yes, ac_cv_ogg=no)

		if test x$enable_experimental = xyes ; then
			PKG_CHECK_MOD_VERSION(SPEEX, speex >= 1.2, ac_cv_speex=yes, ac_cv_speex=no)
		else
			SPEEX_CFLAGS=""
			SPEEX_LIBS=""
			fi

		# Vorbis versions earlier than 1.2.3 have bugs that cause the libsndfile
		# test suite to fail on MIPS, PowerPC and others.
		# See: http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=549899
		PKG_CHECK_MOD_VERSION(VORBIS, vorbis >= 1.2.3, ac_cv_vorbis=yes, ac_cv_vorbis=no)
		PKG_CHECK_MOD_VERSION(VORBISENC, vorbisenc >= 1.2.3, ac_cv_vorbisenc=yes, ac_cv_vorbisenc=no)
		enable_external_libs=yes
		fi

	if test x$ac_cv_flac$ac_cv_ogg$ac_cv_vorbis$ac_cv_vorbisenc = "xyesyesyesyes" ; then
		HAVE_EXTERNAL_LIBS=1
		enable_external_libs=yes

		EXTERNAL_CFLAGS="$FLAC_CFLAGS $OGG_CFLAGS $VORBIS_CFLAGS $VORBISENC_CFLAGS $SPEEX_CFLAGS"
		EXTERNAL_LIBS="$FLAC_LIBS $OGG_LIBS $VORBIS_LIBS $VORBISENC_LIBS $SPEEX_LIBS "
	else
		echo
		AC_MSG_WARN([[*** One or more of the external libraries (ie libflac, libogg and]])
		AC_MSG_WARN([[*** libvorbis) is either missing (possibly only the development]])
		AC_MSG_WARN([[*** headers) or is of an unsupported version.]])
		AC_MSG_WARN([[***]])
		AC_MSG_WARN([[*** Unfortunately, for ease of maintenance, the external libs]])
		AC_MSG_WARN([[*** are an all or nothing affair.]])
		echo
		enable_external_libs=no
		fi
	fi

AC_DEFINE_UNQUOTED([HAVE_EXTERNAL_LIBS], $HAVE_EXTERNAL_LIBS, [Will be set to 1 if flac, ogg and vorbis are available.])

#====================================================================================
# Check for libsqlite3 (only used in regtest).

ac_cv_sqlite3=no
if test x$enable_sqlite != xno ; then
	PKG_CHECK_MOD_VERSION(SQLITE3, sqlite3 >= 3.2, ac_cv_sqlite3=yes, ac_cv_sqlite3=no)
	fi

if test x$ac_cv_sqlite3 = "xyes" ; then
	HAVE_SQLITE3=1
else
	HAVE_SQLITE3=0
	fi

AC_DEFINE_UNQUOTED([HAVE_SQLITE3],$HAVE_SQLITE3,[Set to 1 if you have libsqlite3.])

#====================================================================================
# Determine if the processor can do clipping on float to int conversions.

if test x$enable_cpu_clip != "xno" ; then
	MN_C_CLIP_MODE
else
	echo "checking processor clipping capabilities... disabled"
	ac_cv_c_clip_positive=0
	ac_cv_c_clip_negative=0
	fi

AC_DEFINE_UNQUOTED(CPU_CLIPS_POSITIVE, ${ac_cv_c_clip_positive},
	[Target processor clips on positive float to int conversion.])
AC_DEFINE_UNQUOTED(CPU_CLIPS_NEGATIVE, ${ac_cv_c_clip_negative},
	[Target processor clips on negative float to int conversion.])

#====================================================================================
# Target OS specific stuff.

OS_SPECIFIC_CFLAGS=""
OS_SPECIFIC_LINKS=""
os_is_win32=0
os_is_openbsd=0
use_windows_api=0
osx_darwin_version=0

case "$host_os" in
	darwin* | rhapsody*)
		osx_darwin_version=$(echo "$host_os" | sed 's/\..*//;s/darwin//g')
		if test x$HAVE_XCODE_SELECT = xyes ; then
			developer_path=`xcode-select --print-path`
		else
			developer_path="/Developer"
			fi
		OS_SPECIFIC_CFLAGS="-I${developer_path}/Headers/FlatCarbon"
		OS_SPECIFIC_LINKS="-framework CoreAudio -framework AudioToolbox -framework CoreFoundation"
		;;
	mingw*)
		os_is_win32=1
		use_windows_api=1
		OS_SPECIFIC_LINKS="-lwinmm"
		;;
	openbsd*)
		os_is_openbsd=1
		;;
	esac

AC_DEFINE_UNQUOTED(OS_IS_WIN32, ${os_is_win32}, [Set to 1 if compiling for Win32])
AC_DEFINE_UNQUOTED(OS_IS_OPENBSD, ${os_is_openbsd}, [Set to 1 if compiling for OpenBSD])
AC_DEFINE_UNQUOTED(USE_WINDOWS_API, ${use_windows_api}, [Set to 1 to use the native windows API])
AC_DEFINE_UNQUOTED(OSX_DARWIN_VERSION, ${osx_darwin_version}, [The darwin version, no-zero is valid])
AM_CONDITIONAL(USE_WIN_VERSION_FILE, test ${use_windows_api} -eq 1)

#====================================================================================
# Check for ALSA.

ALSA_LIBS=""

if test x$enable_alsa != xno ; then
	AC_CHECK_HEADERS(alsa/asoundlib.h)
	if test x$ac_cv_header_alsa_asoundlib_h = xyes ; then
		ALSA_LIBS="-lasound"
		enable_alsa=yes
		fi
	fi

#====================================================================================
# Check for OpenBSD's sndio.

SNDIO_LIBS=""
HAVE_SNDIO_H=0
AC_CHECK_HEADERS(sndio.h)
if test x$ac_cv_header_sndio_h = xyes ; then
	SNDIO_LIBS="-lsndio"
	HAVE_SNDIO_H=1
	fi
AC_DEFINE_UNQUOTED([HAVE_SNDIO_H],${HAVE_SNDIO_H},[Set to 1 if <sndio.h> is available.])

#====================================================================================
# Test for sanity when cross-compiling.

if test $ac_cv_sizeof_short != 2 ; then
	AC_MSG_WARN([[******************************************************************]])
	AC_MSG_WARN([[*** sizeof (short) != 2.                                          ]])
	AC_MSG_WARN([[******************************************************************]])
	fi

if test $ac_cv_sizeof_int != 4 ; then
	AC_MSG_WARN([[******************************************************************]])
	AC_MSG_WARN([[*** sizeof (int) != 4                                             ]])
	AC_MSG_WARN([[******************************************************************]])
	fi

if test $ac_cv_sizeof_float != 4 ; then
	AC_MSG_WARN([[******************************************************************]])
	AC_MSG_WARN([[*** sizeof (float) != 4.                                          ]])
	AC_MSG_WARN([[******************************************************************]])
	fi

if test $ac_cv_sizeof_double != 8 ; then
	AC_MSG_WARN([[******************************************************************]])
	AC_MSG_WARN([[*** sizeof (double) != 8.                                         ]])
	AC_MSG_WARN([[******************************************************************]])
	fi

if test x"$ac_cv_prog_HAVE_AUTOGEN" = "xno" ; then
	AC_MSG_WARN([[Touching files in directory tests/.]])
	touch tests/*.c tests/*.h
	fi

#====================================================================================
# Settings for the HTML documentation.

if test x$enable_bow_docs = "xyes" ; then
	HTML_BGCOLOUR="white"
	HTML_FGCOLOUR="black"
else
	HTML_BGCOLOUR="black"
	HTML_FGCOLOUR="white"
	fi

#====================================================================================
# Now use the information from the checking stage.

win32_target_dll=0
COMPILER_IS_GCC=0

if test x$ac_cv_c_compiler_gnu = xyes ; then
	MN_ADD_CFLAGS(-std=gnu99)

	MN_GCC_VERSION

	if test "x$GCC_MAJOR_VERSION$GCC_MINOR_VERSION" = "x42" ; then
		AC_MSG_WARN([****************************************************************])
		AC_MSG_WARN([** GCC version 4.2 warns about the inline keyword for no good **])
		AC_MSG_WARN([** reason but the maintainers do not see it as a bug.         **])
		AC_MSG_WARN([** See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=33995      **])
		AC_MSG_WARN([** Using -fgnu-inline to avoid this stupidity.                **])
		AC_MSG_WARN([****************************************************************])
		MN_ADD_CFLAGS([-fgnu89-inline])
		fi

	CFLAGS="$CFLAGS -Wall"
	CXXFLAGS="$CXXFLAGS -Wall"

	MN_ADD_CFLAGS([-Wextra])
	MN_ADD_CFLAGS([-Wdeclaration-after-statement])
	MN_ADD_CFLAGS([-Wpointer-arith])
	MN_ADD_CFLAGS([-funsigned-char])

	AC_LANG_PUSH([C++])
	MN_ADD_CXXFLAGS([-Wextra])
	MN_ADD_CXXFLAGS([-Wpointer-arith])
	MN_ADD_CXXFLAGS([-funsigned-char])
	AC_LANG_POP([C++])


	MN_ADD_CFLAGS([-D_FORTIFY_SOURCE=2])

	if test x$enable_stack_smash_protection = "xyes" ; then
		XIPH_GCC_STACK_PROTECTOR
		XIPH_GXX_STACK_PROTECTOR
		fi

	if test x$enable_test_coverage = "xyes" ; then
		# MN_ADD_CFLAGS([-ftest-coverage])
		MN_ADD_CFLAGS([-coverage])
		fi

	CFLAGS="$CFLAGS     -Wcast-align -Wcast-qual -Wshadow -Wbad-function-cast -Wwrite-strings -Wundef -Wuninitialized -Winit-self -Wnested-externs -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Waggregate-return"
	# -Winline -Wconversion  -Wunreachable-code"
	CXXFLAGS="$CXXFLAGS -Wcast-align -Wcast-qual -Wshadow -Wwrite-strings -Wctor-dtor-privacy -Wnon-virtual-dtor -Woverloaded-virtual -Wreorder -Wsign-promo -Wundef -Wuninitialized -Winit-self"

	if test "x$enable_gcc_opt" = "xno" ; then
		temp_CFLAGS=`echo $CFLAGS | $SED "s/O2/O0/"`
		CFLAGS=$temp_CFLAGS
		AC_MSG_WARN([[*** Compiler optimisations switched off. ***]])
		fi

	# OS specific tweaks.
	case "$host_os" in
		darwin* | rhapsody*)
			# Disable -Wall, -pedantic and -Wshadow for Apple Darwin/Rhapsody.
			# System headers on these systems are broken.
			temp_CFLAGS=`echo $CFLAGS | $SED "s/-Wall -pedantic//" | $SED "s/-Wshadow//" | $SED "s/-Waggregate-return//"`
			CFLAGS=$temp_CFLAGS
			SHLIB_VERSION_ARG="-Wl,-exported_symbols_list -Wl,\$(srcdir)/Symbols.darwin"
			;;
		linux*|kfreebsd*-gnu*|gnu*)
			SHLIB_VERSION_ARG="-Wl,--version-script=\$(srcdir)/Symbols.gnu-binutils"
			;;
		mingw*)
			SHLIB_VERSION_ARG="-Wc,-static-libgcc -Wl,\$(srcdir)/libsndfile-1.def"
			win32_target_dll=1
			if test x"$enable_shared" = xno ; then
				win32_target_dll=0
				fi
			;;
		os2*)
			SHLIB_VERSION_ARG="-Wl,-export-symbols \$(srcdir)/Symbols.os2"
			;;
		*)
			;;
		esac
	if test x$enable_gcc_pipe != "xno" ; then
	 	CFLAGS="$CFLAGS -pipe"
	 	fi

	COMPILER_IS_GCC=1
	fi

if test x$enable_sanitizer = "xyes" ; then
	MN_ADD_CFLAGS([-fsanitize=address])
	MN_ADD_CFLAGS([-fsanitize=integer])
	MN_ADD_CFLAGS([-fsanitize=undefined])

	# If we enable it for the C compiler we have to do it for the C++ compiler as well.
	AC_LANG_PUSH([C++])
	MN_ADD_CXXFLAGS([-fsanitize=address])
	MN_ADD_CXXFLAGS([-fsanitize=integer])
	MN_ADD_CXXFLAGS([-fsanitize=undefined])
	AC_LANG_POP([C++])
	fi

if test x$enable_werror = "xyes" ; then
	MN_ADD_CFLAGS([-Werror])

	AC_LANG_PUSH([C++])
	MN_ADD_CXXFLAGS([-Werror])
	AC_LANG_POP([C++])
	fi


AC_DEFINE_UNQUOTED([WIN32_TARGET_DLL], ${win32_target_dll}, [Set to 1 if windows DLL is being built.])
AC_DEFINE_UNQUOTED([COMPILER_IS_GCC], ${COMPILER_IS_GCC}, [Set to 1 if the compile is GNU GCC.])

CFLAGS="$CFLAGS $OS_SPECIFIC_CFLAGS"

if test x"$CFLAGS" = x ; then
	echo "Error in configure script. CFLAGS has been screwed up."
	exit
	fi

HOST_TRIPLET="${host_cpu}-${host_vendor}-${host_os}"

AC_DEFINE_UNQUOTED([HOST_TRIPLET], "${HOST_TRIPLET}", [The host triplet of the compiled binary.])

if test "$HOST_TRIPLET" = "x86_64-w64-mingw32" ; then
	OS_SPECIFIC_LINKS=" -static-libgcc $OS_SPECIFIC_LINKS"
	fi

WIN_RC_VERSION=`echo $PACKAGE_VERSION | $SED -e "s/p.*//" -e "s/\./,/g"`


if test "$enable_static" = no ; then
	SRC_BINDIR=src/.libs/
	TEST_BINDIR=tests/.libs/
else
	SRC_BINDIR=src/
	TEST_BINDIR=tests/
	fi

#-------------------------------------------------------------------------------

AC_SUBST(HOST_TRIPLET)

AC_SUBST(HTML_BGCOLOUR)
AC_SUBST(HTML_FGCOLOUR)

AC_SUBST(SHLIB_VERSION_ARG)
AC_SUBST(SHARED_VERSION_INFO)
AC_SUBST(CLEAN_VERSION)
AC_SUBST(WIN_RC_VERSION)

AC_SUBST(HAVE_EXTERNAL_LIBS)
AC_SUBST(OS_SPECIFIC_CFLAGS)
AC_SUBST(OS_SPECIFIC_LINKS)
AC_SUBST(ALSA_LIBS)
AC_SUBST(SNDIO_LIBS)

AC_SUBST(EXTERNAL_CFLAGS)
AC_SUBST(EXTERNAL_LIBS)
AC_SUBST(SRC_BINDIR)
AC_SUBST(TEST_BINDIR)

dnl The following line causes the libtool distributed with the source
dnl to be replaced if the build system has a more recent version.
AC_SUBST(LIBTOOL_DEPS)

AC_CONFIG_FILES([ \
	src/Makefile man/Makefile examples/Makefile tests/Makefile regtest/Makefile \
	M4/Makefile doc/Makefile Win32/Makefile Octave/Makefile programs/Makefile \
	Makefile \
	src/version-metadata.rc tests/test_wrapper.sh tests/pedantic-header-test.sh \
	doc/libsndfile.css Scripts/build-test-tarball.mk libsndfile.spec sndfile.pc \
	src/sndfile.h \
	echo-install-dirs
	])
AC_OUTPUT

# Make sure these are executable.
chmod u+x tests/test_wrapper.sh Scripts/build-test-tarball.mk echo-install-dirs

#====================================================================================

AC_MSG_RESULT([
-=-=-=-=-=-=-=-=-=-= Configuration Complete =-=-=-=-=-=-=-=-=-=-

  Configuration summary :

    libsndfile version : .................. ${VERSION}

    Host CPU : ............................ ${host_cpu}
    Host Vendor : ......................... ${host_vendor}
    Host OS : ............................. ${host_os}

    Experimental code : ................... ${enable_experimental:-no}
    Using ALSA in example programs : ...... ${enable_alsa:-no}
    External FLAC/Ogg/Vorbis : ............ ${enable_external_libs:-no}
])

if test -z "$PKG_CONFIG" ; then
	echo "    *****************************************************************"
	echo "    ***          The pkg-config program is missing.               ***"
	echo "    *** External FLAC/Ogg/Vorbis libs cannot be found without it. ***"
	echo "    ***       http://pkg-config.freedesktop.org/wiki/             ***"
	echo "    *****************************************************************"
	echo
	fi

echo "  Tools :"
echo
echo "    Compiler is Clang : ................... ${mn_cv_c_compiler_clang}"
echo "    Compiler is GCC : ..................... ${ac_cv_c_compiler_gnu}"

if test x$ac_cv_c_compiler_gnu = xyes ; then
	echo "    GCC version : ......................... ${GCC_VERSION}"
	if test $GCC_MAJOR_VERSION -lt 3 ; then
		echo "\n"
		echo "    ** This compiler version allows applications to write"
		echo "    ** to static strings within the library."
		echo "    ** Compile with GCC version 3.X or above to avoid this problem."
		fi
	fi
echo "    Sanitizer enabled : ................... ${enable_sanitizer:-no}"
echo "    Stack smash protection : .............. ${enable_stack_smash_protection:-no}"

./echo-install-dirs

# Remove symlink created by Scripts/android-configure.sh.
test -h gdbclient && rm -f gdbclient

(cd src && make genfiles)
(cd tests && make genfiles)
