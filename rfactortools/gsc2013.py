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

from collections import defaultdict
import os
import re
import shutil
import sys

import imgtool
import rfactortools

########################################################################

def rfactor_to_gsc2013_gdb(filename, target_file):
    with open(filename, "rt", encoding="latin-1") as fin:
        lines = fin.readlines()

    with open(target_file, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
        for line in lines:
            line = re.sub(r'Filter Properties *=.*',
                          r'Filter Properties = StockV8 \\*/',
                          line, flags=re.IGNORECASE)
            fout.write(line)

def rfactor_to_gsc2013_veh(filename, target_file):
    with open(filename, "rt", encoding="latin-1") as fin:
        lines = fin.readlines()

    with open(target_file, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
        for line in lines:
            line = re.sub(r'Classes="',
                          r'Classes="reiza5, ',
                          line, flags=re.IGNORECASE)
            fout.write(line)

def rfactor_to_gsc2013_gmt(filename, target_file):
    rfactortools.encrypt_file(filename, target_file)

def rfactor_to_gsc2013_mas(filename, target_file):
    print("mas unpacking %s" % filename)
    mas_content = rfactortools.mas_unpack_to_data(filename)

    print("encrypting files")
    encrypted_mas_content = []
    for i, (name, data) in enumerate(mas_content):
        print("processing %d/%d: %s" % (i, len(mas_content), name))
        encrypted_data = rfactortools.encrypt_data(data, 1, 0x4b1dca9f960524e8, rfactortools.get_skip(name))
        encrypted_mas_content.append((name, encrypted_data))

    rfactortools.mas_pack_from_data(encrypted_mas_content, target_file)

########################################################################

def rfactor_to_gsc2013(source_directory, target_directory):
    print("Converting %s to %s" % (source_directory, target_directory))

    # gather files and directories
    dir_tree = []
    files_by_type = defaultdict(list)

    for path, dirs, files in os.walk(source_directory):
        relpath = os.path.relpath(path, source_directory)

        for d in dirs:
            dir_tree.append(os.path.normpath(os.path.join(relpath, d)))

        for fname in files:
            filename = os.path.normpath(os.path.join(relpath, fname))

            ext = os.path.splitext(filename)[1].lower()
            files_by_type[ext].append(filename)

    dir_tree.sort()

    # create target directory hierachy
    if not os.path.isdir(target_directory):
        os.makedirs(os.path.normpath(target_directory))
    for d in dir_tree:
        t = os.path.join(target_directory, d)
        print("creating %s" % t)
        if not os.path.isdir(t):
            os.mkdir(t)

    # convert and copy files
    for ext, files in files_by_type.items():
        for i, filename in enumerate(files):
            source_file = os.path.join(source_directory, filename)
            target_file = os.path.join(target_directory, filename)

            print("Processing '%s' file %d/%d: %s" % (ext, i+1, len(files), filename))
            if ext == ".gdb":
                rfactor_to_gsc2013_gdb(source_file, target_file)
            elif ext == ".veh":
                rfactor_to_gsc2013_veh(source_file, target_file)
            elif ext == ".gmt":
                rfactor_to_gsc2013_gmt(source_file, target_file)
            elif ext == ".mas":
                if filename.lower() == "shared/coreshader.mas":
                    pass
                else:
                    rfactor_to_gsc2013_mas(source_file, target_file)
            else:
                shutil.copy(source_file, target_file)

    imgtool.process_directory(target_directory)
