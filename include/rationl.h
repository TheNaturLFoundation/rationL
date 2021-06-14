#include <stddef.h>

/**
 * @struct reg_t
 * This type contains all the necessary informations about a regular
 * expression and it's automaton in order to be used by the functions
 * Needs to be freed using match_free
 */
typedef struct reg_t reg_t;

typedef struct match
{
	/**
	* The string the regexp was matched against.
	*/
    const char *string;
	/**
	* Offset in string where the match begins.
	*/
    size_t start;
	/**
	* The size of the match.
	*/
    size_t length;
	/**
	* The number of groups matched.
	*/
    size_t nb_groups;
	/**
	* The values matched by groups.
	*/
    char **groups;
} match;

/**
 * Compiles a pattern into a regular expression without operators
 * This functions optimises the compilation of the regular expression
 * in case the regular expression you want to compile only contains
 * simple caracters and no regex operators.
 * @param pattern: string containing the pattern to compile.
 * @return The compiled regular expression.
*/
reg_t regexp_compile_string(char *pattern);

/**
 * Compiles a pattern into a regular expression
 * This functions compiles the regular expression pattern passed
 * as parameter.
 * @param pattern: string containing the pattern to compile.
 * @return The compiled regular expression.
*/
reg_t regex_compile(char* pattern);

/**
 * Compiles a regular expression from an automaton daut file format
 * @param pattern: path to a .daut file.
 * @return The compiled regular expression.
*/
reg_t regex_read_daut(char *path);

/**
 * Matches the pattern against str parameter and returns a match
 * value.
 * @param re: The regular expression.
 * @param str: The string to match against.
 * @return The match struct with data information about the match if found
 * else NULL.
*/
match *regex_match(reg_t re, char* str);

/**
 * Matches the pattern against str parameter and returns all submatches as a list
 * Fills the groups array with the currect matches structs.
 * @param re: The regular expression.
 * @param str: The string to match against.
 * @param groups: A pointer to a match array.
 * @return The number of matches
*/
size_t regex_search(reg_t re, char *str, match **groups[]);

/**
 * Substitute matches of re in str by sub.
 * @param re: The regular expression.
 * @param str: The string to match against.
 * @param sub: The string to replace with
 * @return The replaced string if found else NULL
*/
char *regex_sub(reg_t re, char *str, char *sub);

/**
 * Frees the regular expression.
 * @param re the regular expression to free.
 */
void regex_free(reg_t re);

/**
 * Frees the match struct.
 * @param re the regular expression to free.
 */
void match_free(match *match);
