#Setup linking
AM_CPPFLAGS = -I$(top_srcdir)/src/ \
				-I$(top_srcdir)/include/ \
				-I$(top_srcdir)/test/ \
				-DTEST_PATH=\"$(top_srcdir)/test/\"

AM_LDFLAGS  = $(CRITERION_LIBS)
AM_CFLAGS 	= $(CRITERION_CFLAGS)
LDADD = $(top_builddir)/librationl.la

TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
            $(top_srcdir)/tap-driver.sh


# Setup Criterion TAP output using ENV
AM_TESTS_ENVIRONMENT = \
   CRITERION_OUTPUTS='tap:-'; \
   export CRITERION_OUTPUTS;



AUTOMAKE_OPTIONS = serial-tests

check_PROGRAMS = \
            map_tests \
			array_tests \
			bin_tree_tests \
			linked_list_tests \
			matrix_tests \
			lexer_tests \
			automaton_tests \
			parsing_tests \
			interface_tests

map_tests_SOURCES = map/map_test.c

array_tests_SOURCES = array/array_test.c

bin_tree_tests_SOURCES = bin_tree/bin_tree_test.c

linked_list_tests_SOURCES = linked_list/linked_list_test.c

matrix_tests_SOURCES = matrix/matrix_test.c

lexer_tests_SOURCES = lexer/lexer_test.c

automaton_tests_SOURCES = \
            automaton/utils.c \
			automaton/thompson_test.c \
			automaton/matching_test.c \
			automaton/automaton_test.c \
			automaton/delete_eps_test.c \
			automaton/prune_test.c \
			automaton/daut_test.c \
			automaton/determine_test.c \
            automaton/minimization_test.c \
			automaton/stringify_test.c \
			automaton/build_search_dfa_test.c


parsing_tests_SOURCES = \
			test_utils.c \
			parsing/binary_basics.c \
			parsing/binary_more.c \
			parsing/paranthesis_basics.c \
			parsing/paranthesis_more.c \
			parsing/paranthesis_unary.c \
			parsing/paranthesis_unary_more.c \
			parsing/paranthesis_nested.c \
			parsing/escaped_characters.c \
			parsing/unary_basics.c \
			parsing/groups.c

interface_tests_SOURCES = interface/interface_test.c


TESTS = $(check_PROGRAMS)
