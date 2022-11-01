#!/usr/bin/env python3

# GTR2 to GSC2013 converter
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


import argparse
import logging

import rfactortools


def main():
    parser = argparse.ArgumentParser(description='GTR2 to GSC2013 converter')
    parser.add_argument('DIRECTORY', action='store', type=str, nargs=1,
                        help='directory containing the mod')
    parser.add_argument('-o', '--output', metavar='DIR', type=str, required=True,
                        help="output directory")
    parser.add_argument('-d', '--datadir', metavar='DIR', type=str, required=True,
                        help="GTR2 directory")
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()

    if args.verbose:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)

    conv = rfactortools.GTR2ToGSC2013(args.datadir, args.DIRECTORY[0])
    conv.convert_all(args.output)


# EOF #
