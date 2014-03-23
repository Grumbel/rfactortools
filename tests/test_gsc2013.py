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
        self.assertEqual(rfactortools.gsc2013.find_track_directory_from_searchpath
                         ("mods/70tracks/75monza", [".", "75monza", "70tracks/75monza"]),
                         ("mods/70tracks", None))
        self.assertEqual(rfactortools.gsc2013.find_track_directory_from_searchpath
                         ("mods/75monza", [".", "70tracks", "70tracks/75monza"]),
                         ("mods/75monza", "70tracks"))
        self.assertEqual(rfactortools.gsc2013.find_track_directory_from_searchpath
                         ("mods/70tracks/75monza/75monza", [".", "75monza", "75monza/75monza"]),
                         ("mods/70tracks/", None))

if __name__ == '__main__':
    unittest.main()


# EOF #
