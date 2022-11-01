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
      version='0.3.1',
      entry_points = {
          'gui_scripts': [
              "minised-gui = rfactortools.programs.minised_gui:main",
              "rfactortools-gui = rfactortools.programs.rfactortools_gui:main",
          ],
          'console_scripts': [
              "aiwtool = rfactortools.programs.aiwtool:main",
              "dirtoo = rfactortools.programs.dirtool:main",
              "gentool = rfactortools.programs.gentool:main",
              "gmttool = rfactortools.programs.gmttool:main",
              "gtr2-to-gsc2013 = rfactortools.programs.gtr2_to_gsc2013:main",
              "imgtool = rfactortools.programs.imgtool:main",
              "maspack = rfactortools.programs.maspack:main",
              "masunpack = rfactortools.programs.masunpack:main",
              "minised = rfactortools.programs.minised:main",
              "race07-ids = rfactortools.programs.race07_ids:main",
              "rfactorcrypt = rfactortools.programs.rfactorcrypt:main",
              "rfactor-to-gsc2013 = rfactortools.programs.rfactor_to_gsc2013:main",
              "sfxtool = rfactortools.programs.sfxtool:main",
              "vehtool = rfactortools.programs.vehtool:main",
          ],
      },
      packages=['rfactortools', 'rfactortools.gui'],
      ext_modules=[Extension('rfactortools._crypt', ['rfactortools_crypt.cpp'])],
      requires=['PIL', 'pathlib'])


# EOF #
