---
title: regex_match
section: 3 
header: rationL User Manual
footer: rationL 0.1.0
date: April 5, 2021
---

# NAME

regex_match – Matche a string from against a compiled regular expression 

# SYNOPSIS
    #include <rationl.h>
    
    int regex_match(reg_t re, char *string);
    
# DESCRIPTION


**regex_match()** matches a regular expression against a string and returns true(1) if the regular expression matches the string or false(0) otherwise. 

*re*
    The regular expression that was compiled using **regex_compile**
    
*string*
    The string to match the regular expression against

# RETURN VALUE

**regex_match()** returns true(1) if the string passed as parameter is matched by the regular expression false(0) if not. 


# EXAMPLES

**Program that matches an email address passed as first parameter**
    
    #include <rationl.h>
    #include <err.h>
    #include <stdio.h>

    int main(int argc, char** argv)
    {
        if(argc != 2)
            errx(1, "No input email address found.");

        reg_t re = regex_compile("\\w+(\\.\\w+)?@\\w+(\\.[a-z]{2,})+");

        if(regex_match(re, argv[1]))
            printf("%s is a valid email.\n", argv[1]);
        else
            printf("%s is not a valid email.\n", argv[1]);
        regex_free(re);
        return 0;
    }
