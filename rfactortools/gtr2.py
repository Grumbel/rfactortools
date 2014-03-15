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


import os
import shutil
import logging

import rfactortools


class GTR2ToGSC2013:

    def __init__(self, gtr2_directory, source_directory):
        self.gtr2_directory = os.path.normpath(gtr2_directory)
        self.source_directory = os.path.normpath(source_directory)

    def convert_gtr(self, source_file, target_file):
        logging.info("mas unpacking %s" % source_file)
        mas_content = rfactortools.mas_unpack_to_data(source_file)

        # TOOD: This doesn't work .gmt files have different formats in
        # GSC2013 and rFactor and need to be converted first.

        logging.info("encrypting files")
        encrypted_mas_content = []
        for i, (name, data) in enumerate(mas_content):
            logging.debug("processing %d/%d: %s" % (i, len(mas_content), name))
            encrypted_data = rfactortools.encrypt_data(data, 1, 0x4b1dca9f960524e8, rfactortools.get_skip(name))
            encrypted_mas_content.append((name, encrypted_data))

        rfactortools.mas_pack_from_data(encrypted_mas_content, target_file)

    def convert_trk(self, source_file, target_file):
        with open(source_file, "rt", encoding="latin-1") as fin:
            lines = fin.readlines()

        with open(target_file, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
            for line in lines:
                fout.write(line)

    def convert_all(self, target_directory):
        target_directory = os.path.normpath(target_directory)

        if not os.path.isdir(target_directory):
            logging.info("creating %s" % target_directory)
            os.mkdir(target_directory)

        for path, dirs, files in os.walk(self.source_directory):
            relpath = os.path.relpath(path, self.source_directory)

            for d in dirs:
                target_d = os.path.join(target_directory, relpath, d)
                if not os.path.isdir(target_d):
                    logging.info("creating %s" % target_d)
                    os.mkdir(target_d)

            for fname in files:
                source_file = os.path.normpath(os.path.join(path, fname))
                target_file = os.path.normpath(os.path.join(target_directory, relpath, fname))

                root, ext = os.path.splitext(fname)

                if ext.lower() == ".gtr":
                    target_file = os.path.normpath(os.path.join(target_directory, relpath, root + ".mas"))
                    self.convert_gtr(source_file, target_file)
                elif ext.lower() == ".trk":
                    target_file = os.path.normpath(os.path.join(target_directory, relpath, root + ".scn"))
                    # self.convert_trk(source_file, target_file)
                    shutil.copy(source_file, target_file)
                else:
                    shutil.copy(source_file, target_file)

                print("%s -> %s" % (source_file, target_file))


# EOF #
