#!/usr/bin/env python3

# rfactortools test cases
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


import os
import rfactortools
import tempfile
import unittest
import shutil


class SCNTestCase(unittest.TestCase):
    def setUp(self):
        self.test_datadir = os.path.join(os.path.dirname(__file__), 'data')
        self.tmpdir = tempfile.mkdtemp(prefix='rfactortools')

    def tearDown(self):
        shutil.rmtree(self.tmpdir)

    def test_cmaps_fix(self):
        input_directory = os.path.join(self.test_datadir, "cmaps_fix/GameData/")
        output_directory = os.path.join(self.tmpdir, "cmap_fix")

        cfg = rfactortools.rFactorToGSC2013Config()
        converter = rfactortools.rFactorToGSC2013(input_directory, cfg)
        converter.convert_all(output_directory)

        with rfactortools.open_read(os.path.join(output_directory,
                                                 "GameData/Vehicles/TheMod/Subdir/graphics.gen")) as fin:
            self.assertEqual(fin.read(),
                             "MASFile=TheMod\\cmaps.mas\n")


if __name__ == '__main__':
    unittest.main()

# EOF #
