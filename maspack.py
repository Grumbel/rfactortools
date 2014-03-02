#!/usr/bin/env python3

# rFactor MAS packer
# Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
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
import argparse

import rfactortools

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor MAS packer')
    parser.add_argument('INPUTDIR', action='store', type=str,
                        help='directory to be packed')
    parser.add_argument('MASFILE', action='store', type=str,
                        help='output file')
    parser.add_argument('-i', '--inputfile', action='store_true', default=False,
                        help="Read input files from file instead of directory")
    parser.add_argument('-t', '--type', metavar='INT', action='store', type=int, default=1,
                        help='MAS file type (0: GTL/GTR2, 1: rFactor, GSC2013)')
    args = parser.parse_args()

    if args.inputfile:
        with open(args.INPUTDIR, "r") as fin:
            files = fin.read().splitlines()
    else:
        files = [os.path.join(args.INPUTDIR, name) for name in sorted(os.listdir(args.INPUTDIR))]

    rfactortools.mas_pack(files, args.MASFILE, args.type)

# EOF #
