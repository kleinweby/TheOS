TheOS BuildSupport
==================

This folder contains files that help building TheOS:
 * a description to build a toolchain
 * helper scripts for building TheOS

TheOS building will base upon rake.

Object files will reside in an obj dir let the rest be clean.

Currently we use the system clang of Mac OS X and therefor
the only build platform tested is Mac OS X ;)

Build the toolchain
-------------------

Download the last release of binutils and compile it

 $ ./configure --prefix="$TOOLCHAIN_ROOT" --target=i386-unkown-elf --program-prefix=i386-theos-elf-
 $ make
 $ make install

TOOLCHAIN_ROOT must point to the fullpath of BuildSupport/toolchain
