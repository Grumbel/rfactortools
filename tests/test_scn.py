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
from PIL import Image


class SCNTestCase(unittest.TestCase):
    def setUp(self):
        self.test_datadir = os.path.join(os.path.dirname(__file__), 'data')
        self.tmpdir = tempfile.mkdtemp(prefix='rfactortools')

    def tearDown(self):
        shutil.rmtree(self.tmpdir)

    def test_cmaps_fix(self):
        """Make sure that MASFile=cmaps.mas is fixed when the mod contains a cmaps.mas"""

        input_directory = os.path.join(self.test_datadir, "cmaps_fix/GameData/")
        output_directory = os.path.join(self.tmpdir, "cmap_fix")

        cfg = rfactortools.rFactorToGSC2013Config()
        converter = rfactortools.rFactorToGSC2013(input_directory, cfg)
        converter.convert_all(output_directory)

        with rfactortools.open_read(os.path.join(output_directory,
                                                 "GameData/Vehicles/TheMod/Subdir/graphics.gen")) as fin:
            self.assertEqual(fin.read(),
                             "MASFile=TheMod\\cmaps.mas\n")

    def test_cmaps_dontfix(self):
        """Make sure that MASFile=cmaps.mas is not fixed when the cmaps is in another mod directory"""

        input_directory = os.path.join(self.test_datadir, "cmaps_dontfix/GameData/")
        output_directory = os.path.join(self.tmpdir, "cmap_dontfix")

        cfg = rfactortools.rFactorToGSC2013Config()
        converter = rfactortools.rFactorToGSC2013(input_directory, cfg)
        converter.convert_all(output_directory)

        with rfactortools.open_read(os.path.join(output_directory,
                                                 "GameData/Vehicles/TheMod/Subdir/graphics.gen")) as fin:
            self.assertEqual(fin.read(),
                             "MASFile=cmaps.mas\n")

    def test_loading_aspect(self):
        """Test that the aspect ratio of the loading screen is properly converted

        This test gives a "ResourceWarning: unclosed file" warning, see:
        https://github.com/python-imaging/Pillow/issues/477
        """

        input_directory = os.path.join(self.test_datadir, "loading_aspect/GameData/")
        output_directory = os.path.join(self.tmpdir, "loading_aspect")

        cfg = rfactortools.rFactorToGSC2013Config()
        converter = rfactortools.rFactorToGSC2013(input_directory, cfg)
        converter.convert_all(output_directory)

        img = Image.open(os.path.join(output_directory,
                                      "GameData/Locations/TestTrack/TestTrack_loading.jpg"))
        self.assertEqual(img.size, (1024, 768))


if __name__ == '__main__':
    unittest.main()


# EOF #
