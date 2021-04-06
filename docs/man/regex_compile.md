---
title: regex_compile 
section: 3 
header: rationL User Manual
footer: rationL 0.1.0
date: April 5, 2021
---

# NAME

regex_compile regex_free – Compile a regular expression from a pattern 

# SYNOPSIS
    #include <rationl.h>
    
    reg_t regex_compile(char* pattern);
    
    void regex_free(reg_t re);
    
# DESCRIPTION
    
**regex_compile()** takes a regular expression pattern and compiles it into a regular expression matching the pattern.

The value returned is a **reg_t** structure containing the folowing fields:
    
    typedef struct reg_t
    {
        Automaton* aut;
        char* pattern;
    } regex_t;

*aut* 
    This fields hold a compiled automaton that matches the pattern passed as argument to the compile function.
    This automaton will then be used by the other function to performs the operations on the automaton.

*pattern* 
    This fields hold the pattern passed as argument.

    
## REGULAR EXPRESSION FORMAT
Currently supported regular expression operators are :

*?*
    Corresponds to zero or one occurence of the element. For instance, the regular expression **aa?** will match **a** and **aa** but will not match **aaa**.

*\**
    Corresponds to zero, one or several the the precedent element. The regular expression **a\*** matches an empty string, **a**, **aaaa...** but will not match **ab**.

*+* 
    Corresponds to at least one element. The regular expression **a+** will match **a**, **aa**, **aaaaaa...** but will not match an empty string.

*{n}*
    Repeats the element n times. For instance the regular expression **b{4}** wil match only match **bbbb**.
    
*{n,}*
    Repeats at least the element n times. The regular expression **b{4,}** wil match **bbbb**, **bbbbb...** but will not match **bb** or **bbb**.

*{n,p}*
    Repeats between n and p times the element preceding the . The regular expression **b{4,}** wil match **bbbb**, **bbbbb...** but will not match **bb** or **bbb**.

*a|b*
    Matches expression a or b. For instance the regular expression **a|b** will match **a** and **b** but will not match **ab**. This operator as the lowest precedence meaning that **aa|bb** will match either **aa** or **bb**.

*()*
    When a pattern or an expression is put between parenthesis, it enforces precedence. For example the regular expression defined as **(aa)?** will match if **aa** occurs zero or one time.
    
*[]*
    This operator creates a group of caracters than can be matched. Consider the regular expression **[abc]**. This expression will match **a**, **b** or **c**. When a group is defined using - like in [A-Z] it will match all the caracters in the ASCII order contained between A and Z. To match all alphanumerical word you can use the folowing expression [A-Za-z0-9] that will match either a caracter between A and Z, a caracter between a and z or a number between 0 and 9.

*\\w*
    This is equivalent to writing [A-Za-z0-9_]. Matches all alphanumerical words including and underscores.

*\\W*
    This matches everything except all the caracters in \\w

*\\d*
    This is equivalent to writing [0-9]. Matches all numbers.
    
*\\D*
    This matches everything except all the caracters in \\d

*\\s*
    Matches all blank characters (\\t, \\n, \\r, \\v) and spaces.

*\\S*
    Matches everything that is not a blank character.


# RETURN VALUE
    
**regex_compiles()** returns a element of the reg_t struct or NULL on failure to compile the regular expression.

This regular expression needs to be freed using the **regex_free()** function.
