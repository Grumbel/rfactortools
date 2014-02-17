#!/usr/bin/env python3

##  rFactor to GSC2013 converter
##  Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
##
##  This program is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with this program.  If not, see <http://www.gnu.org/licenses/>.

import argparse
import os
import re
import sys
import subprocess

def rfactor_to_gsc2013_gdb(filename):
    with open(filename, "r") as fin:
        lines = fin.readlines()

    with open(filename, "w") as fout:
        for line in lines:
            line = re.sub(r'Filter Properties *=.*',
                          r'Filter Properties = StockV8 \\*/',
                          line, flags=re.IGNORECASE)
            fout.write(line)

def rfactor_to_gsc2013_veh(filename):
    with open(filename, "r") as fin:
        lines = fin.readlines()

    with open(filename, "w") as fout:
        for line in lines:
            line = re.sub(r'Classes=',
                          r'Classes="reiza5, ',
                          line, flags=re.IGNORECASE)
            fout.write(line)

def rfactor_to_gsc2013_gmt(filename):
    subprocess.check_call(["./rfactorcrypt.py", "-e", filename])
    
def rfactor_to_gsc2013_mas(filename):
    subprocess.check_call(["./rfactor-to-gsc2013-mas.sh", filename])

def rfactor_to_gsc2013(directory):
    for path, dirs, files in os.walk(directory):
        for fname in files:
            filename = os.path.join(path, fname)
            ext = os.path.splitext(filename)[1].lower()

            if ext == ".gdb":
                rfactor_to_gsc2013_gdb(filename)
            elif ext == ".veh":
                rfactor_to_gsc2013_veh(filename)
            elif ext == ".gmt":
                rfactor_to_gsc2013_gmt(filename)
            elif ext == ".mas":
                rfactor_to_gsc2013_mas(filename)
                
    subprocess.check_call(["./imgtool.py", directory])

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor to GSC2013 converter')
    parser.add_argument('DIRECTORY', action='store', type=str, nargs='+',
                        help='directory containing the mod')
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()

    for d in args.DIRECTORY:
        rfactor_to_gsc2013(d)

# EOF #
