\mainpage Main Page
\tableofcontents

# What is rationL

**rationL** is a lightweight and quick regular expression (regexp) dynamic library written in C.

It is written as part of the 4th semester project in the French IT School [EPITA](https://epita.fr).

# Install

## Arch Linux

Arch linux users can use the aur package [rationl](https://aur.archlinux.org/packages/rationl/) with their favorite
aur package manager such as ``yay``

```
yay -S rationl 
```

## Build from source

To compile and install **rationL** you will need the following dependencies :

+ gcc (or any other C compiler supporting C99)
+ GNU Libtool
+ Criterion (optional) for performing tests

Get the latest release tarball [here](https://github.com/TheNaturLFoundation/rationl/releases/latest) and install using the following command :

```
./configure && make install
```

The default install directory is ``/usr/local/`` but can be overriden using ``--prefix=dir`` with the ``configure`` script.

For **Arch Linux** users the default install directory should be ``/usr/`` for instance.

# Developers build

If you want to build the project as a developer you will need **GNU Autotools**.

Clone the ``dev`` branch of the git and run the ``./bootstrap`` script to configure the project for development.

# Documentation

You can compile the documentation of the library in the ``docs`` folder using the ``make public`` command.

You can also find the online documentation [here](https://thenaturlfoundation.github.io/rationL/)
