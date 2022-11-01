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


from setuptools import setup, find_packages
from setuptools.extension import Extension


setup(name='rfactortools',
      version='0.4.0',
      entry_points = {
          'gui_scripts': [
              "rfactortools-gui = rfactortools.programs.rfactortools_gui:main",
          ],
          'console_scripts': [
              "rft-aiwtool = rfactortools.programs.aiwtool:main",
              "rft-dirtool = rfactortools.programs.dirtool:main",
              "rft-gentool = rfactortools.programs.gentool:main",
              "rft-gmttool = rfactortools.programs.gmttool:main",
              "rft-gtr2-to-gsc2013 = rfactortools.programs.gtr2_to_gsc2013:main",
              "rft-imgtool = rfactortools.programs.imgtool:main",
              "rft-maspack = rfactortools.programs.maspack:main",
              "rft-masunpack = rfactortools.programs.masunpack:main",
              "rft-race07-ids = rfactortools.programs.race07_ids:main",
              "rft-rfactor-to-gsc2013 = rfactortools.programs.rfactor_to_gsc2013:main",
              "rft-rfactorcrypt = rfactortools.programs.rfactorcrypt:main",
              "rft-sfxtool = rfactortools.programs.sfxtool:main",
              "rft-vehtool = rfactortools.programs.vehtool:main",
          ],
      },
      packages=find_packages(),
      ext_modules=[Extension('rfactortools._crypt', ['rfactortools_crypt.cpp'])],
      requires=['PIL', 'pathlib'])


# EOF #
