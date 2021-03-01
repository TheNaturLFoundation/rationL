SUBDIRS = include
AM_CPPFLAGS = -I$(top_srcdir)/src/ -I$(top_srcdir)/include/
AM_LDFLAGS = -shared -O3

lib_LTLIBRARIES = librationl.la

# Put here all the source files
# that need to be compiled with
# the dynamic library.
librationl_la_SOURCES = \
	src/rationl.c \
	src/automaton/automaton.c \
	src/datatypes/linked_list.c \
	src/utils/memory_utils.c

TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
            $(top_srcdir)/tap-driver.sh


# Setup Criterion TAP output using ENV
AM_TESTS_ENVIRONMENT = \
   CRITERION_OUTPUTS='tap:-'; \
   export CRITERION_OUTPUTS;


AUTOMAKE_OPTIONS = serial-tests

check_PROGRAMS = criterion_tests

# Put here all the criterion test suites
# For instance if I want a test suite to test
# function A i will create a file named test_a.c
# then add a test_a.c to the sources of the
# criterion tests.

criterion_tests_SOURCES = \
			test/simple.c \
			test/linked_list_test.c

criterion_tests_LDADD = $(lib_LTLIBRARIES)

criterion_tests_CFLAGS =  $(CRITERION_CFLAGS) -fsanitize=address
criterion_tests_LDFLAGS =  $(CRITERION_LIBS)

TESTS = criterion_tests

COVERAGE_INFO_FILE = $(top_builddir)/coverage.info
COVERAGE_REPORT_DIR = $(top_builddir)/reports/coverage

CODE_QUALITY_DIR = $(top_builddir)/reports/scan

COVERAGE_CFLAGS = -O0 -fprofile-arcs -ftest-coverage

.PHONY: coverage scan-build analysis

coverage:
	make clean
	@echo "Starting to create coverage reports..."
	make check CFLAGS="$(COVERAGE_CFLAGS)"
	@echo "Successfully ran tests with coverage report generation."
	lcov --capture --directory $(top_builddir)/src --output-file $(COVERAGE_INFO_FILE)
	genhtml $(COVERAGE_INFO_FILE) --output-directory $(COVERAGE_REPORT_DIR)
	@echo "Generated coverage report succesfully"

scan-build:
	make clean
	@echo "Starting to create code quality reports..."
	scan-build -o $(CODE_QUALITY_DIR)  -k -V make
	@echo "Successfully run code quality scan."

analysis: coverage scan-build
