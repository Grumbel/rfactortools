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
from collections import defaultdict

import tempfile
import imgtool
import shutil

import rfactortools

########################################################################

def rfactor_to_gsc2013_gdb(filename):
    with open(filename, "rt", encoding="latin-1") as fin:
        lines = fin.readlines()

    with open(filename, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
        for line in lines:
            line = re.sub(r'Filter Properties *=.*',
                          r'Filter Properties = StockV8 \\*/',
                          line, flags=re.IGNORECASE)
            fout.write(line)

def rfactor_to_gsc2013_veh(filename):
    with open(filename, "rt", encoding="latin-1") as fin:
        lines = fin.readlines()

    with open(filename, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
        for line in lines:
            line = re.sub(r'Classes="',
                          r'Classes="reiza5, ',
                          line, flags=re.IGNORECASE)
            fout.write(line)

def rfactor_to_gsc2013_gmt(filename):
    rfactortools.encrypt_file(filename, filename)

def rfactor_to_gsc2013_mas(filename):
    print("mas unpacking %s" % filename)
    mas_content = rfactortools.mas_unpack_to_data(filename)

    print("encrypting files")
    encrypted_mas_content = []
    for i, (name, data) in enumerate(mas_content):
        print("processing %d/%d: %s" % (i, len(mas_content), name))
        encrypted_data = rfactortools.encrypt_data(data, 1, 0x4b1dca9f960524e8, rfactortools.get_skip(name))
        encrypted_mas_content.append((name, encrypted_data))

    rfactortools.mas_pack_from_data(encrypted_mas_content, filename)

########################################################################

def rfactor_to_gsc2013(directory):
    for path, dirs, files in os.walk(directory):
        for d in dirs:
            pass

        files_by_type = defaultdict(list)
        for fname in files:
            filename = os.path.normpath(os.path.join(path, fname))
            ext = os.path.splitext(filename)[1].lower()
            files_by_type[ext].append(filename)

    for ext, files in files_by_type.items():
        for i, filename in enumerate(files):
            print("Processing %s file %d/%d: %s" % (ext, i+1, len(files), filename))
            if ext == ".gdb":
                rfactor_to_gsc2013_gdb(filename)
            elif ext == ".veh":
                rfactor_to_gsc2013_veh(filename)
            elif ext == ".gmt":
                rfactor_to_gsc2013_gmt(filename)
            elif ext == ".mas":
                rfactor_to_gsc2013_mas(filename)
            else:
                pass

    imgtool.process_directory(directory)

    # delete GameData/Shared/coreshader.mas if it exists

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor to GSC2013 converter')
    parser.add_argument('DIRECTORY', action='store', type=str, nargs='+',
                        help='directory containing the mod')
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()

    for d in args.DIRECTORY:
        rfactor_to_gsc2013(d)

    print("-- rfactor-to-gsc2013 conversion complete --")

# EOF #
