SUBDIRS = include
AM_CPPFLAGS = -I$(top_srcdir)/src/  -include $(top_srcdir)/include/config.h
AM_LDFLAGS = -shared -O3

lib_LTLIBRARIES = librationl.la

# Put here all the source files
# that need to be compiled with
# the dynamic library.
librationl_la_SOURCES = \
	src/rationl.c \
	src/automaton/automaton.c \
	src/datatypes/linked_list.c \
	src/datatypes/array.c \
	src/utils/memory_utils.c \
	src/datatypes/bin_tree.c \
	src/parsing/lexer.c \
	src/parsing/parsing.c \
	src/matching/matching.c \
	src/automaton/thompson.c

TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
            $(top_srcdir)/tap-driver.sh


# Setup Criterion TAP output using ENV
AM_TESTS_ENVIRONMENT = \
   CRITERION_OUTPUTS='tap:-'; \
   export CRITERION_OUTPUTS;


AUTOMAKE_OPTIONS = serial-tests

check_PROGRAMS = array_tests \
				 bin_tree_tests \
				 linked_list_tests \
				 lexer_tests \
				 automaton_tests \
				 parsing_tests


array_tests_SOURCES = test/array/array_test.c
array_tests_LDADD = $(lib_LTLIBRARIES)
array_tests_CFLAGS =  $(CRITERION_CFLAGS)
array_tests_LDFLAGS =  $(CRITERION_LIBS)

bin_tree_tests_SOURCES = test/bin_tree/bin_tree_test.c
bin_tree_tests_LDADD = $(lib_LTLIBRARIES)
bin_tree_tests_CFLAGS =  $(CRITERION_CFLAGS)
bin_tree_tests_LDFLAGS =  $(CRITERION_LIBS)

linked_list_tests_SOURCES = test/linked_list/linked_list_test.c
linked_list_tests_LDADD = $(lib_LTLIBRARIES)
linked_list_tests_CFLAGS =  $(CRITERION_CFLAGS)
linked_list_tests_LDFLAGS =  $(CRITERION_LIBS)

lexer_tests_SOURCES = test/lexer/lexer_test.c
lexer_tests_LDADD = $(lib_LTLIBRARIES)
lexer_tests_CFLAGS =  $(CRITERION_CFLAGS)
lexer_tests_LDFLAGS =  $(CRITERION_LIBS)

automaton_tests_SOURCES = \
			test/automaton/automaton_test.c \
			test/automaton/daut_test.c \
			test/automaton/matching_test.c \
			test/automaton/thompson_test.c
automaton_tests_LDADD = $(lib_LTLIBRARIES)
automaton_tests_CFLAGS =  $(CRITERION_CFLAGS)
automaton_tests_LDFLAGS =  $(CRITERION_LIBS)

parsing_tests_SOURCES = \
			test/parsing/parsing_test_union_or_concatenation.c \
			test/parsing/parsing_test_union_and_concatenation.c \
			test/parsing/parsing_test_kleene_star.c \
			test/parsing/parsing_test_paranthesis_simple.c \
			test/parsing/parsing_test_paranthesis_nested.c \
			test/parsing/parsing_test_paranthesis_combinaision.c \
			test/parsing/parsing_test_escaped_characters.c
parsing_tests_LDADD = $(lib_LTLIBRARIES)
parsing_tests_CFLAGS =  $(CRITERION_CFLAGS)
parsing_tests_LDFLAGS =  $(CRITERION_LIBS)

TESTS = array_tests \
				 bin_tree_tests \
				 linked_list_tests \
				 lexer_tests \
				 automaton_tests \
				 parsing_tests

COVERAGE_INFO_FILE = $(top_builddir)/coverage.info
COVERAGE_REPORT_DIR = $(top_builddir)/reports/coverage
COVERAGE_REPORT_FILES_GCNO = $(librationl_la_SOURCES:.c=.gcno)
CODE_QUALITY_DIR = $(top_builddir)/reports/scan

COVERAGE_CFLAGS = -O0 -fprofile-arcs -ftest-coverage


.PHONY: coverage scan-build analysis clean-local-coverage


clean-local-coverage:
	rm -rf $(COVERAGE_REPORT_FILES_GCNO)
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
