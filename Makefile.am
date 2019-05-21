ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign
confdir = $(sysconfdir)
conf_DATA = libbson.pc
SUBDIRS = src
if BUILD_LUA
SUBDIRS += lua
endif 
if BUILD_TESTS
SUBDIRS += test
endif
