#!/usr/bin/env python3

# rFactorTools
# Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


from cx_Freeze import setup, Executable
from distutils.core import Extension

setup(name='rfactortools',
      version='0.1',
      executables=[Executable('aiwtool.py'),
                   Executable('aiwtool.py'),
                   Executable('dirtool.py'),
                   Executable('gentool.py'),
                   Executable('gmttool.py'),
                   Executable('imgtool.py'),
                   Executable('maspack.py'),
                   Executable('masunpack.py'),
                   Executable('minised.py'),
                   Executable('rfactorcrypt.py'),
                   Executable('rfactor-to-gsc2013.py'),
                   Executable('rfactortools-gui.pyw'),
                   Executable('vehtool.py')],
      ext_modules=[Extension('rfactortools._crypt', ['rfactortools_crypt.cpp'])],
      include_files=['logo.png'],
      requires=['PIL', 'pathlib'])


# EOF #
