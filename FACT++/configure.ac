######################################################################
# Autoconf initial setup
######################################################################

#[AC_]PACKAGE_NAME
#[AC_]PACKAGE_TARNAME
#[AC_]PACKAGE_VERSION
#[AC_]PACKAGE_STRING
#[AC_]PACKAGE_BUGREPORT
AC_INIT([FACT++],[1.0],[thomas.bretz@phys.ethz.ch],[FACTpp],[https://www.fact-project.org/svn/trunk/FACT++/])
AC_PREREQ([2.65])
AC_CONFIG_FILES([Makefile])   # causes x/Makefile.in to be created if x/Makefile.am exists
#AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([.macro_dir])
AC_CONFIG_AUX_DIR([.aux_dir]) # /usr/share/libtool/config /usr/share/automake-x.xx

# Make sure none of the following will set -O2
AC_ARG_ENABLE([optimization],
    AS_HELP_STRING([--disable-optimization], [Compile with -O0 instead of -O3])
)
AS_IF([test "x$enable_optimization" != "xno"], [MYFLAGS+=" -O3"], [MYFLAGS+=" -O0"])

AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [Compile with debugging symbols (-g)])
)
AS_IF([test "x$enable_debug" = "xyes"], [MYFLAGS+=" -g"])

CFLAGS+=$MYFLAGS
CXXFLAGS+=$MYFLAGS

AC_PROG_CC([gcc])
AC_PROG_CXX([g++])

COLORGCC=`which colorgcc`
AS_IF([test -n "$COLORGCC"], [
    AC_CONFIG_LINKS([g++:$COLORGCC gcc:$COLORGCC])
    PATH=./:$PATH
])

#AC_PROG_CC_C99
AC_PROG_CC_STDC

######################################################################
# GNUC extension support (needed for the event builder)
######################################################################

AC_USE_SYSTEM_EXTENSIONS(_GNU_SOURCE)

######################################################################
# Check for right C++ standard
######################################################################

#AC_CXX_HEADER_STDCXX_0X
AC_CXX_COMPILE_STDCXX_0X
AS_IF([test "$ax_cv_cxx_compile_cxx0x_cxx" != yes], 
   AC_MSG_ERROR([C++0x standard (-std=c++0x) not supported by compiler.]))

# Postponed after the BOOST library tests otherwise the check for boost::thread fails
#CXXFLAGS+=" -std=c++0x"

PKG_CHECK_MODULES(DBUS, dbus-1 dbus-glib-1, HAVE_DBUS=yes, HAVE_DBUS=no)

CPPFLAGS+=" `pkg-config --cflags dbus-1 dbus-glib-1`"
LDFLAGS+=" `pkg-config --libs dbus-1 dbus-glib-1`"

# dbus-1
# dbus-glib-1
# QtOpenGL
# gl
# QtCore
# cfitsio

######################################################################
# Setup the libtool and the language
######################################################################

AM_PROG_AR
LT_INIT([disable-static])
AC_LANG(C++)
#AC_PATH_XTRA

######################################################################
# Automake initial setup
######################################################################
AM_INIT_AUTOMAKE([1.11 -Wall subdir-objects std-options no-define color-tests parallel-tests silent-rules])
AM_SILENT_RULES([yes])

AM_PROG_CC_C_O

######################################################################
# DOXYGEN SUPPORT
######################################################################

### FIXME: Need a configure commandline switch
DX_ENV_APPEND(EXTRACT_ALL, YES)
DX_ENV_APPEND(RECURSIVE,   YES)
DX_ENV_APPEND(ALL_GRAPHS,  NO)  # change to yes to switch on call(er) graphs

#DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)                # sets HAVE_DOT
#DX_HTML_FEATURE(ON)              # sets GENERATE_HTML (default)
#DX_CHM_FEATURE(ON|OFF)           # sets GENERATE_HTMLHELP
#DX_CHI_FEATURE(ON|OFF)           # sets GENERATE_CHI
#DX_MAN_FEATURE(ON)               # sets GENERATE_MAN (segfaults)
#DX_RTF_FEATURE(ON|OFF)           # sets GENERATE_RTF
#DX_XML_FEATURE(ON|OFF)           # sets GENERATE_XML
#DX_PDF_FEATURE(ON|OFF)           # sets GENERATE_PDF (default)
DX_PS_FEATURE(OFF)                # sets GENERATE_PS  (default)
DX_INIT_DOXYGEN($PACKAGE_NAME)#, DOXYFILE-PATH, [OUTPUT-DIR])

#USE_HTAGS              = $(USE_HTAGS)

######################################################################
# pthread/Readline/NCurses (pthread needed by dim and boost)
######################################################################

AC_LANG_PUSH(C)

# Check for math library (some linux need this to compile cfitsio)
AC_CHECK_LIB([m],[cos])

# Needed to compile dim
ACX_PTHREAD

CPPFLAGS+=" "$PTHREAD_CFLAGS
LDFLAGS+=" "$PTHREAD_CFLAGS


# Needed to compile FACT++
AC_CHECK_READLINE
AS_IF([test "x$have_readline" != "xyes"], 
   AC_MSG_ERROR([The readline library is not properly installed.]))

CPPFLAGS+=" "$READLINE_INCLUDES
LDFLAGS+=" "$READLINE_LIBS

# Needed to compile FACT++
AC_CHECK_HEADERS([panel.h],, 
   AC_MSG_ERROR([ncurses header not found]))
   
# Needed to compile FACT++
AC_CHECK_LIB(panel, update_panels,, 
   AC_MSG_ERROR([ncurses panel library not found]))

# Xm.h (lesstif/motif, needed to compile did)
AC_FIND_MOTIF

CPPFLAGS+=" "$MOTIF_INCL
LDFLAGS+=" "$MOTIF_LDFLAGS

AM_COND_IF(HAS_LIBXP,, 
   AC_MSG_WARN([ Motif/lesstif not found!])
)

# Required in did.c
LDFLAGS+=" -lXt -lX11 "

# Check for zlib and exit with error if not found (defines HAVE_LIBZ)
AC_CHECK_PACKAGE(zlib, inflateEnd, z, zlib.h,
		 HAVE_ZLIB=yes, HAVE_ZLIB=no)

# Check for GL and GLU needed for the raw event viewer
AC_CHECK_PACKAGE(GL, glLoadIdentity, GL, GL/gl.h, HAVE_GL=yes, HAVE_GL=no)
AC_CHECK_PACKAGE(GLU, gluNewTess, GLU, GL/glu.h, HAVE_GLU=yes, HAVE_GLU=no)

# Check for libnova
AC_CHECK_PACKAGE(nova, ln_get_julian_from_sys, nova, libnova/julian_day.h, HAVE_NOVA=yes, HAVE_NOVA=no)

# Taken from http://code.google.com/p/autoconf-gl-macros/
#AX_CHECK_GL
#AX_CHECK_GLU
#AX_CHECK_GLUT

# Needed to compile FACT++
AC_CHECK_PACKAGE(cfitsio, ffpss, cfitsio, fitsio.h, 
		 HAVE_CFITSIO=yes, HAVE_CFITSIO=no)

#AC_CHECK_HEADERS([fitsio.h],,AC_MSG_ERROR([cfitsio headers not found]))
#AC_CHECK_LIB([cfitsio], main,,AC_MSG_ERROR([cfitsio library not found]))

AC_LANG_POP(C)

# Needed to compile FACT++
AC_CHECK_PACKAGE(ccfits, main, CCfits, CCfits/CCfits, 
		 HAVE_CCFITS=yes, HAVE_CCFITS=no)

#AC_CHECK_HEADERS([CCfits/CCfits],,
#   AC_MSG_ERROR(CCfits headers not found))
#AC_CHECK_LIB(CCfits, main,,
#   AC_MSG_ERROR(CCfits library not found))

######################################################################
# MySQL(++) SUPPORT
######################################################################

# Needed to compile FACT++
MYSQL_DEVEL
MYSQLPP_DEVEL

LDFLAGS+=" -lmysqlpp"

######################################################################
# BOOST SUPPORT
######################################################################

# Needed to compile FACT++
AX_BOOST_BASE([1.40], [],
   AC_MSG_ERROR([The boost C++ libraries (>=1.40) are not properly installed.])
)

AC_MSG_CHECKING([for BOOST_CPPFLAGS])
AC_MSG_RESULT([$BOOST_CPPFLAGS])

AC_MSG_CHECKING([for BOOST_LDFLAGS])
AC_MSG_RESULT([$BOOST_LDFLAGS])

# Keep this order AX_BOOST_FILESYSTEM needs AX_BOOST_SYSTEM_LIB
AX_BOOST_SYSTEM
AS_IF([test "x$ax_cv_boost_system" != "xyes"],
   AC_MSG_ERROR([Problems with the Boost::System libarary.]))

AX_BOOST_ASIO
AS_IF([test "x$ax_cv_boost_asio" != "xyes"],
   AC_MSG_ERROR([Problems with the Boost::ASIO libarary.]))

AX_BOOST_DATE_TIME
AS_IF([test "x$ax_cv_boost_date_time" != "xyes"],
   AC_MSG_ERROR([Problems with the Boost::Date_Time libarary.]))

AX_BOOST_FILESYSTEM
AS_IF([test "x$ax_cv_boost_filesystem" != "xyes"],
   AC_MSG_ERROR([Problems with the Boost::Filesystem libarary.]))

AX_BOOST_PROGRAM_OPTIONS
AS_IF([test "x$ax_cv_boost_program_options" != "xyes"],
   AC_MSG_ERROR([Problems with the Boost::Program_Options libarary.]))

AX_BOOST_REGEX
AS_IF([test "x$ax_cv_boost_regex" != "xyes"],
   AC_MSG_ERROR([Problems with the Boost::Regex libarary.]))

AX_BOOST_THREAD
AS_IF([test "x$ax_cv_boost_thread" != "xyes"],
   AC_MSG_ERROR([Problems with the Boost::Thread libarary.]))
#AX_BOOST_IOSTREAMS
#AX_BOOST_PYTHON
#AX_BOOST_SERIALIZATION
#AX_BOOST_SIGNALS
#AX_BOOST_TEST_EXEC_MONITOR
#AX_BOOST_UNIT_TEST_FRAMEWORK
#AX_BOOST_WAVE
#AX_BOOST_WSERIALIZATION

LDFLAGS+=" "$BOOST_LDFLAGS
LDFLAGS+=" "$BOOST_SYSTEM_LIB
LDFLAGS+=" "$BOOST_ASIO_LIB
LDFLAGS+=" "$BOOST_DATE_TIME_LIB
LDFLAGS+=" "$BOOST_FILESYSTEM_LIB
LDFLAGS+=" "$BOOST_PROGRAM_OPTIONS_LIB
LDFLAGS+=" "$BOOST_REGEX_LIB
LDFLAGS+=" "$BOOST_THREAD_LIB

CPPFLAGS+=" "$BOOST_CPPFLAGS

# Now we can safely add the compiler option for your prefered standard
CXXFLAGS+=" -std=c++0x"

#AC_CHECK_HEADERS(
#   [\
#      boost/bind.hpp \
#      boost/lexical_cast.hpp \
#      boost/filesystem.hpp \
#      boost/thread.hpp \
#      boost/function.hpp \
#      boost/regex.hpp \
#      boost/asio.hpp \
#      boost/enable_shared_from_this.hpp \
#      boost/asio/deadline_timer.hpp \
#      boost/date_time/posix_time/posix_time.hpp \
#      boost/date_time/local_time/local_time.hpp \
#      boost/date_time/gregorian/gregorian.hpp 
#   ], [],
#   [
#      echo "Error! At least one needed header of the boost C++ libararies is missing." 
#      exit -1
#   ]
#)

######################################################################
# v8 / JavaScript
######################################################################

AC_CHECK_CLASS(v8, [v8::HandleScope handle_scope], v8, v8.h, 
	HAVE_V8=yes, HAVE_V8=no)

######################################################################
# QT4/GUI SUPPORT
######################################################################

AC_ARG_WITH([qt4], 
   [AS_HELP_STRING([--without-qt4], [Disable qt4, i.e. disable gui support.])],
   [], [QT4_DO_IT_ALL])

# This allows to list the QT4 stuff independantly later
CPPFLAGS+=" "${QT4_INCLUDES}" "
LDFLAGS+=" -lQtCore "
AC_CHECK_CLASS(QGL, [QGLWidget qgl], QtOpenGL, QtOpenGL/QGLWidget,
	HAVE_QGL=yes, HAVE_QGL=no)
AC_CHECK_CLASS(qwt, [QwtPlot qwt],   qwt-qt4,  qwt_plot.h,
	HAVE_QWT=yes, HAVE_QWT=no)

######################################################################
# ROOT SUPPORT
######################################################################

AC_ARG_WITH([root], 
   [AS_HELP_STRING([--without-root], [Disable root, i.e. disable gui support.])],
   [], [ROOT_PATH([5.12/00])]
)

if [test "$ROOTEXEC" != no -a -n "$ROOTVERSION"]  ;
then

   ROOTCPPFLAGS=$ROOTCFLAGS
   ROOTCXXFLAGS=$ROOTAUXCFLAGS
   ROOTLDFLAGS="-L"$ROOTLIBDIR

   #AC_CHECK_PROG(HAVE_ROOT_QT, libGQt.so.$ROOTSOVERSION, yes, no, $ROOTLIBDIR)
   #AC_CHECK_PROG(HAVE_GQT,     libGQt.so.$ROOTSOVERSION, yes, no, $LD_LIBRARY_PATH)

   # It seems it dooesn't work on older root versions
   ROOT_FEATURE([qt], [HAVE_ROOT_QT=yes])

   AC_SUBST(ROOTCPPFLAGS)
   AC_SUBST(ROOTCXXFLAGS)
   AC_SUBST(ROOTLDFLAGS)
fi


######################################################################
# Check if we have colordiff to colorize 'svn diff'
######################################################################

# Nice to have to support colored diff
AC_CHECK_PROG(COLORDIFF, colordiff,    colordiff, cat)
AC_CHECK_PROG(GROFF,     groff,        yes, no)
AC_CHECK_PROG(PS2PDF,    ps2pdf,       yes, no)
AC_CHECK_PROG(HELP2MAN,  help2man,     yes, no)
AC_CHECK_PROG(JSDOC,     jsdoc,        yes, no)
AC_CHECK_PROG(MAILX,     mailx,        yes, no)
AC_CHECK_PROG(CURL,      curl,         yes, no)

##########################################################################
# debug compilation support
##########################################################################
#
#AC_MSG_CHECKING([whether to build with debug information])
#AC_ARG_ENABLE([debug],
#    [AS_HELP_STRING([--enable-debug],
#        [enable debug data generation (def=no)])],
#    [debugit="$enableval"],
#    [debugit=no])
#AC_MSG_RESULT([$debugit])
#
#if test x"$debugit" = x"yes"; then
#    AC_DEFINE([DEBUG],[],[Debug Mode])
#    AM_CXXFLAGS="$AM_CXXFLAGS -g -Wall -Werror -Wno-uninitialized -O0"
#else
#    AC_DEFINE([NDEBUG],[],[No-debug Mode])
#    AM_CXXFLAGS="$AM_CXXFLAGS -O3"
#fi
#

##########################################################################
# produce conditionals for Makefile.am and for summary
##########################################################################

AM_CONDITIONAL(HAS_COLORDIFF, [test "$COLORDIFF" = colordiff])
AM_CONDITIONAL(HAS_GROFF,     [test "$GROFF" = yes])
AM_CONDITIONAL(HAS_PS2PDF,    [test "$PS2PDF" = yes])
AM_CONDITIONAL(HAS_HELP2MAN,  [test "$HELP2MAN" = yes])
AM_CONDITIONAL(HAS_JSDOC,     [test "$JSDOC" = yes])
AM_CONDITIONAL(HAS_MAILX,     [test "$MAILX" = yes])
AM_CONDITIONAL(HAS_CURL,      [test "$CURL" = yes])
AM_CONDITIONAL(HAS_DOXYGEN,   [test "$DX_DOXYGEN"])
AM_CONDITIONAL(HAS_DOT,       [test "$DX_DOT"])
AM_CONDITIONAL(HAS_COLORGCC,  [test "$COLORGCC"])
AM_CONDITIONAL(HAS_QT4,       [test "$QT4_VERSION"])
AM_CONDITIONAL(HAS_SQL,       [test "$MYSQLPP_LIB_DIR" -a "$MYSQLPP_INC_DIR" ])
AM_CONDITIONAL(HAS_ROOT,      [test "$ROOTEXEC" != no -a -n "$ROOTVERSION"])
AM_CONDITIONAL(HAS_ROOT_QT,   [test "$HAVE_ROOT_QT" = yes])
AM_CONDITIONAL(HAS_CFITSIO,   [test "$HAVE_CFITSIO" = yes])
AM_CONDITIONAL(HAS_CCFITS,    [test "$HAVE_CCFITS" = yes])
AM_CONDITIONAL(HAS_FITS,      [test "$HAVE_CFITSIO" = yes -a "$HAVE_CCFITS" = yes])
AM_CONDITIONAL(HAS_ZLIB,      [test "$HAVE_ZLIB" = yes])
AM_CONDITIONAL(HAS_GL,        [test "$HAVE_GL" = yes])
AM_CONDITIONAL(HAS_GLU,       [test "$HAVE_GLU" = yes])
AM_CONDITIONAL(HAS_QGL,       [test "$HAVE_QGL" = yes])
AM_CONDITIONAL(HAS_QWT,       [test "$HAVE_QWT" = yes])
AM_CONDITIONAL(HAS_NOVA,      [test "$HAVE_NOVA" = yes])
AM_CONDITIONAL(HAS_DBUS,      [test "$HAVE_DBUS" = yes])
AM_CONDITIONAL(HAS_V8,        [test "$HAVE_V8" = yes])

AM_CONDITIONAL(HAS_GUI,       [test "$QT4_VERSION" -a "$HAVE_GL" = yes -a "$HAVE_GLU" = yes -a "$HAVE_QGL" = yes -a "$HAVE_ROOT_QT" = yes ])
AM_CONDITIONAL(HAS_VIEWER,    [test "$QT4_VERSION" -a "$HAVE_GL" = yes -a "$HAVE_GLU" = yes -a "$HAVE_QGL" = yes -a "$HAVE_QWT" = yes])

AM_CONDITIONAL(IS_FALSE,      [test "x" = "y"])
AM_CONDITIONAL(IS_TRUE,       [test "x" = "x"])

AM_COND_IF(HAS_FITS,  [AC_DEFINE(HAVE_FITS)], )
AM_COND_IF(HAS_ROOT,  [AC_DEFINE(HAVE_ROOT)], )
AM_COND_IF(HAS_ZLIB,  [AC_DEFINE(HAVE_ZLIB)], )
AM_COND_IF(HAS_NOVA,  [AC_DEFINE(HAVE_NOVA)], )
AM_COND_IF(HAS_DBUS,  [AC_DEFINE(HAVE_DBUS)], )
AM_COND_IF(HAS_SQL,   [AC_DEFINE(HAVE_SQL)],  )
AM_COND_IF(HAS_V8,    [AC_DEFINE(HAVE_V8)],   )
AM_COND_IF(HAS_MAILX, [AC_DEFINE(HAVE_MAILX)],)
AM_COND_IF(HAS_CURL,  [AC_DEFINE(HAVE_CURL)],)

##########################################################################
# print summary
##########################################################################

AC_OUTPUT

AC_MSG_NOTICE()
AC_MSG_NOTICE(============================== summary =============================)
AC_MSG_NOTICE()
AC_MSG_NOTICE([ CFLAGS   = $CFLAGS])
AC_MSG_NOTICE()
AC_MSG_NOTICE([ CXXFLAGS = $CXXFLAGS])
AC_MSG_NOTICE()
AC_MSG_NOTICE([ CPPFLAGS = $CPPFLAGS])
AC_MSG_NOTICE()
AC_MSG_NOTICE([ LDFLAGS  = $LDFLAGS])
AC_MSG_NOTICE()
AC_MSG_NOTICE(--------------------------------------------------------------------)
AC_MSG_NOTICE()
AC_MSG_NOTICE([ BOOST_CPPFLAGS = $BOOST_CPPFLAGS])
AC_MSG_NOTICE([ BOOST_LDFLAGS  = $BOOST_LDFLAGS])
AC_MSG_NOTICE()
AM_COND_IF(HAS_SQL, [
   AC_MSG_NOTICE(--------------------------------------------------------------------)
   AC_MSG_NOTICE()
   AC_MSG_NOTICE([ MYSQLPP_INC_DIR = $MYSQLPP_INC_DIR])
   AC_MSG_NOTICE([ MYSQLPP_LIB_DIR = $MYSQLPP_LIB_DIR])
   AC_MSG_NOTICE()
],[])
AM_COND_IF(HAS_QT4, [
   AC_MSG_NOTICE(--------------------------------------------------------------------)
   AC_MSG_NOTICE()
   AC_MSG_NOTICE([ QT4_VERSION  = $QT4_VERSION])
   AC_MSG_NOTICE()
   AC_MSG_NOTICE([ QT4_LIB      = $QT4_LIB])
   AC_MSG_NOTICE([ QT4_LDFLAGS  = $QT4_LDFLAGS])
   AC_MSG_NOTICE([ QT4_INCLUDES = $QT4_INCLUDES])
   AC_MSG_NOTICE()
   AC_MSG_NOTICE([ MOC4 = $MOC4])
   AC_MSG_NOTICE([ UIC4 = $UIC4])
   AC_MSG_NOTICE([ RCC4 = $RCC4])
   AC_MSG_NOTICE()
],[])
AM_COND_IF(HAS_ROOT, [
   AC_MSG_NOTICE(--------------------------------------------------------------------)
   AC_MSG_NOTICE()
   AC_MSG_NOTICE([ ROOTVERSION   = $ROOTVERSION])
   AC_MSG_NOTICE()
   AC_MSG_NOTICE([ ROOTEXEC      = $ROOTEXEC])
   AC_MSG_NOTICE([ ROOTCONF      = $ROOTCONF])
   AC_MSG_NOTICE([ ROOTCINT      = $ROOTCINT])
   AC_MSG_NOTICE([ ROOTCPPFLAGS  = $ROOTCPPFLAGS])
   AC_MSG_NOTICE([ ROOTCXXFLAGS  = $ROOTCXXFLAGS])
   AC_MSG_NOTICE()
],[])
AC_MSG_NOTICE(--------------------------------------------------------------------)
AC_MSG_NOTICE()
AC_MSG_NOTICE([ zlib                   / 'zlib.h'           found: $HAVE_ZLIB])
AC_MSG_NOTICE([ nova                   / libnova.so         found: $HAVE_NOVA])
AC_MSG_NOTICE([ v8                     / 'v8.h' / libv8.so  found: $HAVE_V8])
AC_MSG_NOTICE([ GL                     / GL/gl.h            found: $HAVE_GL])
AC_MSG_NOTICE([ GLU                    / GL/glu.h           found: $HAVE_GLU])
AC_MSG_NOTICE([ QtOpenGL               / QtOpenGL/QGLWidget found: $HAVE_QGL])
AC_MSG_NOTICE([ Qwt                    / qwt_plot.h         found: $HAVE_QWT])
AC_MSG_NOTICE([ cfitsio                / 'fitsio.h'         found: $HAVE_CFITSIO])
AC_MSG_NOTICE([ CCfits                 / 'CCfits/CCfits'    found: $HAVE_CCFITS])
AM_COND_IF(HAS_SQL,
   AC_MSG_NOTICE([ MySQL/MySQL++          / 'mysql++.h'        found: yes]),
   AC_MSG_NOTICE([ MySQL/MySQL++          / 'mysql++.h'        found: no])
)
AC_MSG_NOTICE([ DBus-1, DBus-glib-1    / pkg-config         found: $HAVE_DBUS])
AM_COND_IF(HAS_LIBXP,
   AC_MSG_NOTICE([ Motif/lesstif          / 'Xm/Xm.h' 'libXp'  found: yes]),
   AC_MSG_NOTICE([ Motif/lesstif          / 'Xm/Xm.h' 'libXp'  found: no])
)
AM_COND_IF(HAS_COLORDIFF,
   AC_MSG_NOTICE([ colored svn diff       / 'colordiff'        found: yes]),
   AC_MSG_NOTICE([ colored svn diff       / 'colordiff'        found: no])
)
AM_COND_IF(HAS_JSDOC,
   AC_MSG_NOTICE([ JavaScript docu        / 'jsdoc'            found: yes]),
   AC_MSG_NOTICE([ JavaScript docu        / 'jsdoc'            found: no])
)
AM_COND_IF(HAS_V8,[
   AM_COND_IF(HAS_MAILX,
      AC_MSG_NOTICE([ Mail support           / 'mailx'            found: yes]),
      AC_MSG_NOTICE([ Mail support           / 'mailx'            found: no])
   )
   AM_COND_IF(HAS_CURL,
      AC_MSG_NOTICE([ Curl support           / 'curl'             found: yes]),
      AC_MSG_NOTICE([ Curl support           / 'curl'             found: no])
   )
])

AM_COND_IF(HAS_HELP2MAN,[
   AC_MSG_NOTICE([ man-pages enabled      / 'help2man'         found: $HELP2MAN])
   AM_COND_IF(HAS_GROFF,
      AC_MSG_NOTICE([ build of html enabled  / 'groff'            found: yes])
      AC_MSG_NOTICE([ build of pdf  enabled  / 'ps2pdf'           found: $PS2PDF]),
      AC_MSG_NOTICE([ build of html and pdf  / 'groff'            found: no])
   )
],[
   AC_MSG_NOTICE([ man, html, pdf enabled / 'help2man'         found: no])
])   

AM_COND_IF(HAS_DOXYGEN,
   AC_MSG_NOTICE([ doxygen docu enabled   / 'doxygen'          found: $DX_DOXYGEN]),
   AC_MSG_NOTICE([ doxygen docu enabled   / 'doxygen'          found: no])
)
AM_COND_IF(HAS_DOT,
   AC_MSG_NOTICE([ doxygen graphs enabled / 'dot' (graphviz)   found: $DX_DOT]),
   AC_MSG_NOTICE([ doxygen graphs enabled / 'dot' (graphviz)   found: no])
)
AM_COND_IF(HAS_QT4,
   AC_MSG_NOTICE([ QT4 support enabled    /  QT4 toolkit       found: yes]),
   AC_MSG_NOTICE([ QT4 support enabled    /  QT4 toolkit       found: no])
)
AC_MSG_NOTICE([ ROOT support enabled   /  root >= 5.12/00   found: $ROOTEXEC])
AM_COND_IF(HAS_ROOT_QT,
   AC_MSG_NOTICE([ ROOT built with --with-qt                   found: yes]),
   AC_MSG_NOTICE([ ROOT built with --with-qt                   found: no])
)
AC_MSG_NOTICE()
AC_MSG_NOTICE(--------------------------------------------------------------------)
AC_MSG_NOTICE()
AC_MSG_NOTICE([Suggestions and hints:])
AC_MSG_NOTICE()
AC_MSG_NOTICE([ - Add 'V=1' to you make-call to switch on verbose output.])
AC_MSG_NOTICE([ - Do not try to create man-pages with doxygen,])
AC_MSG_NOTICE([   it does most probably not work.])
AM_COND_IF(HAS_COLORGCC,,
   AC_MSG_NOTICE([ - Install colorgcc to get colored compiler output.])
)
AC_MSG_NOTICE([ - If you send me a bug report about:])
AC_MSG_NOTICE([   + configure: please attach the file config.log])
AC_MSG_NOTICE([   + make: please send me the output of 'make V=1'])
AC_MSG_NOTICE()
AC_MSG_NOTICE(--------------------------------------------------------------------)
AC_MSG_NOTICE()
AC_MSG_NOTICE([Some interesting build targets:])
AC_MSG_NOTICE()
AC_MSG_NOTICE([ diff:          colored 'svn diff' if colordiff is installed])
AM_COND_IF(HAS_HELP2MAN,[
   AC_MSG_NOTICE([ program.man:   build man-page from 'program --help'])
   AC_MSG_NOTICE([                (display with 'man ./program.man')])
   AM_COND_IF(HAS_GROFF,[
      AC_MSG_NOTICE([ program.html:  build html page from man-page])
      AM_COND_IF(HAS_PS2PDF,
         AC_MSG_NOTICE([ program.pdf:   build pdf documentation from man page])
      )
   ])
])
AM_COND_IF(HAS_DOXYGEN,
   AC_MSG_NOTICE([ doxygen-run:   build html documentation with doxygen])
#   AC_MSG_NOTICE([ doxygen-doc:   build html and pdf documentation with doxygen])
)
AM_COND_IF(HAS_JSDOC,
   AC_MSG_NOTICE([ jsdoc:         build JavaScript documentation with jsdoc])
)
AC_MSG_NOTICE([ doc:           build all buildable documentation])
AC_MSG_NOTICE()
AC_MSG_NOTICE(--------------------------------------------------------------------)
AC_MSG_NOTICE()
AM_COND_IF(HAS_ZLIB,, 
   AC_MSG_WARN([ zlib not found. Support for reading .fits.gz disabled.])
)
AM_COND_IF(HAS_LIBXP,, 
   AC_MSG_WARN([ Motif/lesstif not found. The dim gui did will not be built!])
)
AM_COND_IF(HAS_FITS,, 
   AC_MSG_WARN([ FITS support disabled. cfitsio/CCfits missing!])
)
AM_COND_IF(HAS_ROOT,  
   [AM_COND_IF(HAS_ROOT_QT,, 
      AC_MSG_WARN([ root found but it was built without the --with-qt option])
   )],
   AC_MSG_WARN([ root not found - ROOT support disabled.])
)
AM_COND_IF(HAS_QT4,, AC_MSG_WARN([ QT4 missing - GUIs disabled!]))
AM_COND_IF(HAS_GL,,  AC_MSG_WARN([ GL not found - GUIs will be disabled.]))
AM_COND_IF(HAS_GLU,, AC_MSG_WARN([ GLU not found - GUIs will be disabled.]))
AM_COND_IF(HAS_QGL,, AC_MSG_WARN([ QtOpenGl not found - GUIs will be disabled.]))
AM_COND_IF(HAS_QWT,, AC_MSG_WARN([ Qwt not found - viewer will be disabled.]))
AM_COND_IF(HAS_GUI, [], 
      AC_MSG_WARN([ Either of the necessary packages not found - fact GUI disabled!])
)
AM_COND_IF(HAS_VIEWER, [], 
      AC_MSG_WARN([ Either of the necessary packages not found - viewer GUI disabled!])
)
AM_COND_IF(HAS_JSDOC,,   AC_MSG_WARN([ JavaScript documentation disabled!]))
AM_COND_IF(HAS_MAILX,,   AC_MSG_WARN([ Mail functionality in JavaScript disabled!]))
AM_COND_IF(HAS_CURL,,    AC_MSG_WARN([ Curl functionality in JavaScript disabled!]))
AM_COND_IF(HAS_SQL,,     AC_MSG_WARN([ database support globally disabled!]))
AM_COND_IF(HAS_NOVA,,    AC_MSG_WARN([ libnova missing - smartfact will be compiles without astronomy support!]))
AM_COND_IF(HAS_V8,,      AC_MSG_WARN([ libv8 missing - JavaScript support will be disabled!]))
AM_COND_IF(HAS_DBUS,,    AC_MSG_WARN([ DBus libraries missing - skypeclient will not be compiled!]))
AC_MSG_NOTICE()
