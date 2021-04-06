---
title: regex_search 
section: 3 
header: rationL User Manual
footer: rationL 0.1.0
date: April 5, 2021
---

# NAME

regex_search – Searches substring that matches a regular expression 

# SYNOPSIS
    #include <rationl.h>
    
    size_t regex_search(reg_t re, char *input, char **matches[]);
    
    
# DESCRIPTION


**regex_search()** searches maximal subtring that the regular expression given as parameter matches. Copies all occurences found in the form of a list of string in the *output* parameter. The output parameter does not need to be allocated but will need to be freed.

*re*
    The regular expression compiled using (regex_compile).
    
*input*
    The input string to match against the regular expression
    
*matches*
    This pointer contains a list of string that was allocated by the regex_search function. The string in the list are ordered in order of detection from left to right in the input string. This list will need to be freed by the user.

# RETURN VALUE

**regex_search()** returns the size of the list of string that has been matched by the regular expression.

# EXAMPLES

**Programs that returns all function defined in a C source file**

    #include <err.h>
    #include <rationl.h>
    #include <stdio.h>
    #include <stdlib.h>

    int main(int argc, char **argv)
    {
        if (argc != 2)
            errx(1, "No input file found.");

        reg_t re = regex_compile(
            "\\w+(\\s|\\*)+\\w+\\([ A-Za-z0-9,*]*\\)");

        char *line_buf = NULL;
        size_t line_buf_size = 0;
        int line_count = 0;
        ssize_t line_size;
        FILE *fp = fopen(argv[1], "r");
        if (!fp)
        {
            errx(EXIT_FAILURE, "Error opening file '%s'\n", argv[1]);
        }

        char**groups;



        while ((line_size = getline(&line_buf, &line_buf_size, fp) >= 0))
        {
            line_count++;

            size_t match_size = regex_search(re, line_buf, &groups);

            for (size_t i = 0; i < match_size; i++)
            {
                printf("%s\n", groups[i]);
                free(groups[i]);
            }
            free(groups);
        }

        free(line_buf);
        fclose(fp);
        regex_free(re);
        return 0;
    }
