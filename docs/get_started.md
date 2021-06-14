\page get_started Getting started
\tableofcontents

During this documentation we are going to use ``gcc`` for compilation
commands but beware that these commands should work with any POSIX compilers.

# Compile with the library

After having successfully installed the library, you can compile the library using

```
gcc main.c -lrationl
```

# A simple program : verify an email

For the sake of simplicity, the regular expression that we are going to use 
to match our email is ``\w+(\.\w+)?@\w+(\.[a-z]{2,})+``.

The folowing program uses ``regex_compile`` to first compile the pattern into a
regular expression and then uses ``regex_match`` to check if the argument of the 
program matches the regular expression. 

```c
#include <rationl.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc < 2)
        return 1;
    reg_t re = regex_compile("\\w+(\\.\\w+)?@\\w+(\\.[a-z]{2,})+");
    if(regex_match(re, argv[1]))
        printf("%s is a valid email.\n", argv[1]);
    else
        printf("%s is not a valid email.\n", argv[1]);
    regex_free(re);
    return 0;
}
```

Do not forget to free the regular expression before exiting the program.

# Extract all include headers from a C/C++ file.

For this program we are going to use the function ``regex_matches`` to get all submatches of a regular expression
in a single file.

To extract headers from a C/C++ file we are going to use the folowing regular expression ``#include <[\w+/]+.h>``

```c

#include <rationl.h>
#include <stdio.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    if (argc < 2)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
        errx(1, "No such file or directory: %s", argv[1]);

    reg_t re = regex_compile("#include <(?:\\w|/)+.h>");

    // Ugly file management
    struct stat buff_stat;
    stat(argv[1], &buff_stat);
    char *fbuf = malloc(sizeof(char) * (buff_stat.st_size));
    fread(fbuf, buff_stat.st_size, 1, f);
    fclose(f);

    match **match_list;
    size_t match_size = 0;
    if ((match_size = regex_search(re, fbuf, &match_list)) != 0)
    {
       for (size_t i = 0; i < match_size; i++)
        {
            match *match = match_list[i];
            printf("%.*s\n", (int)(match->length), fbuf + match->start);
        }
    }
	free(fbuf);
    regex_free(re);
    return 0;
}
```
