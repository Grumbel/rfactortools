#!/bin/bash

i586-mingw32msvc-gcc -s -mdll -std=c99 _crypt.c -o rfactortools/_crypt.pyd \
  -Iexternal/win32/Python34/include/ \
  -Lexternal/win32/Python34/libs/ \
  -lpython34

# EOF #
