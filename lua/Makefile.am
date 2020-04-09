ACLOCAL_AMFLAGS = -I m4 --install

AM_LDFLAGS = -export-symbols-regex '^luaopen_' -module -avoid-version

luaexec_LTLIBRARIES = bson4lua.la
bson4lua_la_SOURCES = bson_lua_wrapper.c
bson4lua_la_LIBADD = $(top_builddir)/src/libbson.la
bson4lua_la_CFLAGS = -I$(top_builddir)/src/