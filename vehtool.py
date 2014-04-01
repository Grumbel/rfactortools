#!/usr/bin/env python3

# rFactor .scn/.gen file manipulation tool
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
import os
import sys

import rfactortools


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor .veh/.gen processor')
    parser.add_argument('FILE', action='store', type=str, nargs='+',
                        help='.veh file or directory containing .veh files')
    parser.add_argument('-t', '--tree', action='store_true', default=False,
                        help="print tree")
    args = parser.parse_args()

    files = []
    for path in args.FILE:
        if os.path.isdir(path):
            files += rfactortools.find_files(path, ".veh")
        else:
            files.append(path)

    vehs = [rfactortools.parse_vehfile(filename) for filename in files]

    if args.tree:
        rfactortools.print_veh_tree(vehs, sys.stdout)
    else:
        rfactortools.print_veh_info(vehs, sys.stdout)

# EOF #
