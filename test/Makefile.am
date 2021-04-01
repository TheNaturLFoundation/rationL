#Setup linking
AM_CPPFLAGS = -I$(top_srcdir)/src/  -include $(top_srcdir)/include/config.h
AM_LDFLAGS  = $(CRITERION_LIBS)
AM_CFLAGS 	= $(CRITERION_CFLAGS)

LDADD = $(top_srcdir)/librationl.la

TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
            $(top_srcdir)/tap-driver.sh


# Setup Criterion TAP output using ENV
AM_TESTS_ENVIRONMENT = \
   CRITERION_OUTPUTS='tap:-'; \
   export CRITERION_OUTPUTS;


AUTOMAKE_OPTIONS = serial-tests

check_PROGRAMS = \
			array_tests \
			bin_tree_tests \
			linked_list_tests \
			lexer_tests \
			automaton_tests \
			parsing_tests


array_tests_SOURCES = array/array_test.c

bin_tree_tests_SOURCES = bin_tree/bin_tree_test.c

linked_list_tests_SOURCES = linked_list/linked_list_test.c

lexer_tests_SOURCES = lexer/lexer_test.c

automaton_tests_SOURCES = \
			automaton/automaton_test.c \
			automaton/daut_test.c \
			automaton/matching_test.c \
			automaton/thompson_test.c

parsing_tests_SOURCES = \
			parsing/parsing_test_union_or_concatenation.c \
			parsing/parsing_test_union_and_concatenation.c \
			parsing/parsing_test_kleene_star.c \
			parsing/parsing_test_paranthesis_simple.c \
			parsing/parsing_test_paranthesis_nested.c \
			parsing/parsing_test_paranthesis_combinaision.c \
			parsing/parsing_test_escaped_characters.c

TESTS = $(check_PROGRAMS)
