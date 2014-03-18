#!/usr/bin/env python3

# rFactorTools
# Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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


from distutils.core import setup
from distutils.extension import Extension

setup(name='rfactortools',
      version='0.2.0',
      scripts=[
          "aiwtool.py",
          "dirtool.py",
          "gentool.py",
          "gmttool.py",
          "gtr2-to-gsc2013.py",
          "imgtool.py",
          "maspack.py",
          "masunpack.py",
          "minised-gui.py",
          "minised.py",
          "race07-ids.py",
          "rfactorcrypt.py",
          "rfactor-to-gsc2013.py",
          "rfactortools-gui.py",
          "sfxtool.py",
          "vehtool.py",
      ],
      packages=['rfactortools'],
      ext_modules=[Extension('rfactortools._crypt', ['rfactortools_crypt.cpp'])],
      requires=['PIL', 'pathlib'])


# EOF #
