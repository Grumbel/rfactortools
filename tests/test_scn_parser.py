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
import rfactortools.scn_parser


class SCNParserTestCase(unittest.TestCase):
    def setUp(self):
        pass  # print("setUp()")

    def tearDown(self):
        pass  # print("tearDown()")

    def test_posix2scn_path(self):
        self.assertEqual(rfactortools.scn_parser.posix2scn_path("<TEAMDIR>/.."), "<TEAMDIR>\\..")
        self.assertEqual(rfactortools.scn_parser.posix2scn_path("<TEAMDIR>"), "<TEAMDIR>")

    def test_scn2posix_path(self):
        self.assertEqual(rfactortools.scn_parser.scn2posix_path("<TEAMDIR>\\.."), "<TEAMDIR>/..")
        self.assertEqual(rfactortools.scn_parser.scn2posix_path("<TEAMDIR>"), "<TEAMDIR>")


if __name__ == '__main__':
    unittest.main()


# EOF #
