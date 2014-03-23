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


import unittest
import rfactortools.gsc2013


class GSC2013TestCase(unittest.TestCase):
    def setUp(self):
        pass  # print("setUp()")

    def tearDown(self):
        pass  # print("tearDown()")

    def test_find_track_directory_from_searchpath(self):
        lst = [
            # everything normal
            (("mods/70tracks/75monza", [".", "75monza", "70tracks/75monza"]),
             ("mods/70tracks", None)),

            # 70tracks is missing from the mod, but required by SearchPath
            (("mods/75monza", [".", "70tracks", "70tracks/75monza"]),
             ("mods/75monza", "70tracks")),

            # dot is missing
            (("mods/75monza", ["70tracks", "70tracks/75monza"]),
             ("mods/75monza", "70tracks")),

            # SearchPath is to short, doesn't go to till the track file directory
            (("mods/70tracks/75monza/75monza/barakuda", [".", "75monza", "75monza/75monza"]),
             ("mods/70tracks/75monza", None)),

            # SearchPath is to short and 70tracks is missing
            (("mods/75monza/barakuda", [".", "TrackCol/70tracks", "TrackCol/70tracks/75monza"]),
             ("mods/75monza", "TrackCol/70tracks")),
        ]
        for (d, sp), expected in lst:
            self.assertEqual(rfactortools.gsc2013.find_track_directory_from_searchpath(d, sp), expected,
                             "(%r, %r)" % (d, sp))

if __name__ == '__main__':
    unittest.main()


# EOF #
