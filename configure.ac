#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(bson_c_lib, 1.1.0, jacob@livio.io)
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIRS([m4])
LT_INIT
AC_CONFIG_SRCDIR([examples/sample.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_ENABLE_SHARED
AC_DISABLE_STATIC

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_ARG_WITH(
	[lua-wrapper], 
	[AS_HELP_STRING(
		[--with-lua-wrapper], 
		[Build Lua wrapper library]
	)], 
	[AS_CASE(
		${withval}, 
		[yes], 
		[with_lua=yes], 
		[no], 
		[with_lua=no], 
		[AC_MSG_ERROR(
			[invalid value (${withval}) for --with-lua-wrapper]
		)]
	)], 
	[with_lua=no])

AM_CONDITIONAL(
	[BUILD_LUA], 
	[test x$with_lua = xyes])

# Note: requires libcheck installed with pkg-config file (.pc)
PKG_CHECK_MODULES(CHECK, [check], [HAVE_LIBCHECK=1], [HAVE_LIBCHECK=0])

#AM_PATH_CHECK(,[have_check="yes"],
#	AC_MSG_WARN([Check not found; cannot run unit tests!])
#	[have_check="no"])
#AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes")

AC_OUTPUT(Makefile src/Makefile src/emhashmap/Makefile lua/Makefile test/Makefile)
