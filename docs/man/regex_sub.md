---
title: regex_sub
section: 3 
header: rationL User Manual
footer: rationL 0.1.0
date: April 5, 2021
---

# NAME

regex_sub – Replaces matches of a regular expression in a string  

# SYNOPSIS
    #include <rationl.h>
    
    char* regex_sub(reg_t re, char* input, char* replace);
   
# DESCRIPTION
    
**regex_sub()** searches maximal substring that the regular expression given as parameter matches in *intput* and replaces it with the *replace* value. Returns the result of the remplacement. The return value will need to be freed by the programmer. If no occurence is found the string returned is simply the same and the *input* string. 

*re*
    The regular expressions to match against
    
*input*
    The input string to replace from.

*replace*
    The string to replace occurences found with.
    

# RETURN VALUE
    
**regex_sub()** The input string with the replaced values. If there is no match in the input string, the return value is the same as the input. The return value will need to be freed by the programmer.

# EXAMPLES

**Program that replaces any version number in a file with the latest one**
    
    #include <err.h>
    #include <rationl.h>
    #include <stdlib.h>
    #include <stdio.h>

    int main(int argc, char **argv)
    {
        if (argc != 2)
            errx(1, "No input file found.");

        reg_t re = regex_compile("v(\\d+)\\.(\\d+)\\.(\\d+)");

        char *line_buf = NULL;
        size_t line_buf_size = 0;
        int line_count = 0;
        ssize_t line_size;
        FILE *fp = fopen(argv[1], "r");
        if (!fp)
        {
            errx(EXIT_FAILURE, "Error opening file '%s'\n", argv[1]);
        }

        char **groups;

        while ((line_size = getline(&line_buf, &line_buf_size, fp) >= 0))
        {
            char* replaced_value = regex_sub(re, line_buf, "v2.5.0");
            printf("old:%snew:%s", line_buf, replaced_value);
            free(replaced_value);
        }

        free(line_buf);
        fclose(fp);
        regex_free(re);
        return 0;
    }

# SEE ALSO
    regex_match(3) regex_search(3)
