#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(bson_c_lib, 1.2.1, jacob@livio.io)
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall])
AC_CONFIG_MACRO_DIRS([m4])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR]) # see https://lists.gnu.org/archive/html/automake/2012-05/msg00014.html
LT_INIT
AC_CONFIG_SRCDIR([examples/sample.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([libbson.pc])
AC_OUTPUT

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

AM_COND_IF(
	[BUILD_LUA],
	[AX_PROG_LUA([5.1], [5.4])])
AM_COND_IF(
	[BUILD_LUA],
	[AX_LUA_HEADERS])

AC_ARG_WITH(
	[tests],
	[AS_HELP_STRING(
		[--with-tests],
		[Build unit tests (requires libcheck)]
	)],
	[AS_CASE(
		${withval},
		[yes],
		[with_tests=yes],
		[no],
		[with_tests=no],
		[AC_MSG_ERROR(
			[invalid value (${withval}) for --with-tests]
		)]
	)],
	[with_tests=no])

AM_CONDITIONAL(
	[BUILD_TESTS],
	[test x$with_tests = xyes])

AM_COND_IF(
	[BUILD_TESTS],
	# Note: requires libcheck installed with pkg-config file (.pc)
	[PKG_CHECK_MODULES(CHECK, [check])])

AC_OUTPUT(Makefile src/Makefile src/emhashmap/Makefile lua/Makefile test/Makefile)
