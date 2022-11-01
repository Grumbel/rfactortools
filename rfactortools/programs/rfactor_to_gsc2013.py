#!/usr/bin/env python3

# rFactor to GSC2013 converter
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
import sys

import rfactortools


def main():
    cfg = rfactortools.rFactorToGSC2013Config()

    parser = argparse.ArgumentParser(description='rFactor to GSC2013 converter')
    parser.add_argument('DIRECTORY', action='store', type=str, nargs='+',
                        help='directory containing the mod')
    parser.add_argument('-o', '--output', metavar='DIR', type=str,
                        help="output directory")
    parser.add_argument('-i', '--info', action='store_true', default=False,
                        help="show info on the mod")
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    parser.add_argument('-q', '--quiet', action='store_true', default=False,
                        help="be less verbose")
    args = parser.parse_args()

    target_directory = args.output

    if args.verbose:
        logging.basicConfig(level=logging.DEBUG)
    elif args.quiet:
        logging.basicConfig(level=logging.ERROR)
    else:
        logging.basicConfig(level=logging.INFO)

    if args.info:
        for source_directory in args.DIRECTORY:
            converter = rfactortools.rFactorToGSC2013(source_directory, cfg)
            converter.print_info(sys.stdout)
    else:
        if not target_directory:
            raise Exception("--output DIR must be set")
        else:
            for source_directory in args.DIRECTORY:
                converter = rfactortools.rFactorToGSC2013(source_directory, cfg)
                converter.convert_all(target_directory)
                print("-- rfactor-to-gsc2013 conversion complete --")


# EOF #
