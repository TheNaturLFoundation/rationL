SUBDIRS = include . test docs
EXTRA_DIST = docs test
AM_CPPFLAGS = -I$(top_srcdir)/src/  -include $(top_srcdir)/include/config.h
AM_LDFLAGS = -shared -Ofast
man3_MANS =	\
	docs/man/regex_compile.man \
	docs/man/regex_free.man \
	docs/man/regex_sub.man \
	docs/man/regex_search.man \
	docs/man/regex_match.man

dist_man_MANS = $(man3_MANS)

lib_LTLIBRARIES = librationl.la
# Put here all the source files
# that need to be compiled with
# the dynamic library.
source_files = \
	src/rationl.c \
	src/automaton/automaton.c \
	src/datatypes/linked_list.c \
	src/datatypes/array.c \
	src/datatypes/map.c \
	src/datatypes/matrix.c \
	src/utils/errors.c \
	src/utils/memory_utils.c\
	src/datatypes/bin_tree.c \
	src/parsing/lexer.c \
	src/parsing/parsing.c \
	src/matching/matching.c \
	src/automaton/thompson.c \
	src/automaton/prune.c \
	src/automaton/delete_eps.c \
	src/automaton/determine.c \
	src/automaton/minimization.c \
    src/automaton/stringify.c

header_files = \
	src/automaton/automaton.h \
	src/datatypes/linked_list.h \
	src/datatypes/array.h \
	src/datatypes/map.h \
	src/datatypes/matrix.h \
	src/utils/errors.h \
	src/utils/memory_utils.h\
	src/datatypes/bin_tree.h \
	src/parsing/lexer.h \
	src/parsing/parsing.h \
	src/matching/matching.h \
	src/automaton/thompson.h \
	src/automaton/delete_eps.h \
	src/automaton/prune.h \
	src/automaton/determine.h \
    src/automaton/minimization.h \
	src/automaton/stringify.h

librationl_la_SOURCES = $(source_files) $(header_files)


if COVERAGE

COVERAGE_INFO_FILE = $(top_builddir)/coverage.info
COVERAGE_REPORT_DIR = $(top_builddir)/reports/coverage
COVERAGE_REPORT_FILES_GCNO = $(source_files:.c=.gcno)
CODE_QUALITY_DIR = $(top_builddir)/reports/scan

COVERAGE_CFLAGS = -O0 -fprofile-arcs -ftest-coverage

.PHONY: coverage clean-local-coverage


clean-local-coverage:
	rm -rf $(COVERAGE_REPORT_FILES_GCNO)
	rm -rf test/*/*.gcda
	rm -rf test/*/*.gcno
	rm -rf test/*.gcda
	rm -rf test/*.gcno

clean-local: clean-local-coverage


coverage:
	make clean
	@echo "Starting to create coverage reports..."
	make check CFLAGS="$(COVERAGE_CFLAGS)"
	@echo "Successfully ran tests with coverage report generation."
	lcov --capture --directory $(top_builddir)/src --output-file $(COVERAGE_INFO_FILE)
	genhtml $(COVERAGE_INFO_FILE) --output-directory $(COVERAGE_REPORT_DIR)
	@echo "Generated coverage report succesfully"

endif
