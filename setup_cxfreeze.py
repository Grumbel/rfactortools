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


from cx_Freeze import setup, Executable
from distutils.core import Extension

setup(name='rfactortools',
      version='0.4.0',
      executables=[
          # Executable("aiwtool.py"),
          # Executable("dirtool.py"),
          # Executable("gentool.py"),
          # Executable("gmttool.py"),
          # Executable("gtr2-to-gsc2013.py"),
          # Executable("imgtool.py"),
          Executable("maspack.py"),
          Executable("masunpack.py"),
          # Executable("race07-ids.py"),
          Executable("rfactorcrypt.py"),
          # Executable("rfactor-to-gsc2013.py"),
          Executable("rfactortools-gui.pyw", base='Win32GUI'),
          # Executable("sfxtool.py"),
          # Executable("vehtool.py"),
      ],
      ext_modules=[Extension('rfactortools._crypt', ['rfactortools_crypt.cpp'])],
      options={'build_exe': {'include_files': ['logo.png',
                                               "HOWTO.txt",
                                               ('FAQ.md', 'FAQ.txt'),
                                               ('README.md', 'README.txt'),
                                               ('COPYING', 'COPYING.txt'),
                                               'checksums/GSC2013-GameData.sha1sums',
                                               'checksums/Race07-full.sha1sums',
                                               "data/racegroove.dds",
                                               "data/skidhard.dds",
                                               "data/sky00.dds",
                                               "data/sky01.dds",
                                               "data/sky02.dds",
                                               "data/sky03.dds",
                                               "data/sky04.dds"],
                             'includes': ['PIL.TgaImagePlugin'],
                             'icon': 'icon.ico'}},
      requires=['PIL', 'pathlib'])


# EOF #
