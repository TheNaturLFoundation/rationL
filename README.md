<div align="center">
 
![rationL Logo](https://github.com/TheNaturLFoundation/rationL/blob/main/rationL.png)

![GitHub release (latest by date)](https://img.shields.io/github/v/release/TheNaturLFoundation/rationL?style=flat-square)
![GitHub last commit](https://img.shields.io/github/last-commit/TheNaturLFoundation/rationL?style=flat-square)
![GitHub Workflow Status](https://img.shields.io/github/workflow/status/TheNaturLFoundation/rationL/build-and-documentation?label=Build&style=flat-square)
![GitHub issues](https://img.shields.io/github/issues/TheNaturLFoundation/rationL?label=Issues&style=flat-square)
![GitHub release (latest by date)](https://img.shields.io/github/downloads/TheNaturLFoundation/rationL/latest/total?style=flat-square)
![GitHub](https://img.shields.io/github/license/TheNaturLFoundation/rationL?style=flat-square)

# 

[Install](#install) • [Developer build](#developers-build) • [Documentation](#documentation) • [Doxygen Documentation](https://thenaturlfoundation.github.io/rationL/)

</div>

# What is rationL

**rationL** is a lightweight and quick regular expression (regexp) dynamic library written in C.

It is written as part of the 4th semester project in the French IT School [EPITA](https://epita.fr).

# Install

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
