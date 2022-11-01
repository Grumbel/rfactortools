#!/usr/bin/env python3

# rFactor MAS unpacker
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

import rfactortools


def main():
    parser = argparse.ArgumentParser(description='rFactor MAS packer')
    parser.add_argument('MASFILE', action='store', type=str,
                        help='.mas file to unpack')
    parser.add_argument('OUTDIR', action='store', type=str, nargs='?',
                        help='output directory')
    parser.add_argument('-l', '--list', action='store_true',
                        help="list only, don't extract")
    parser.add_argument('-H', '--with-filename', action='store_true', default=False,
                        help="prefix listing with filename")
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()

    if args.list:
        rfactortools.mas_list(args.MASFILE, args.verbose, args.with_filename)
    else:
        rfactortools.mas_unpack(args.MASFILE, args.OUTDIR, args.verbose)


# EOF #
