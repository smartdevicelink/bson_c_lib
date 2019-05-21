TESTS = bson_util_test bson_object_test
check_PROGRAMS = bson_util_test bson_object_test

bson_util_test_SOURCES = bson_util_test.c $(top_builddir)/src/bson_util.c
bson_util_test_CFLAGS = -Wall -I../src @CHECK_CFLAGS@
bson_util_test_LDADD = @CHECK_LIBS@

bson_object_test_SOURCES = bson_object_test.c
bson_object_test_CFLAGS = -Wall -I../src @CHECK_CFLAGS@
bson_object_test_LDADD = $(top_builddir)/src/libbson.la @CHECK_LIBS@
bson_object_test_LDFLAGS = -static
