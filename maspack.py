#!/usr/bin/env python3

##  rFactor MAS packer
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

import sys
import struct
import zlib
import os
import argparse

mas_type0 = b"GMOTORMAS10\0\0\0\0\0"
mas_type1 = b"\xC8\xCF\xD2\xD8\xCE\xD8\xE6\xC9\xCA\xDD\xD8\xBE\xBB\xA6\xBF\x90"
mas_type3 = b"CUBEMAS4.10\0\0\0\0\0"

def mas_pack(inputdir, masfile, mas_type):
    with open(masfile, "wb") as fout:
        files = os.listdir(inputdir)
        
        if mas_type == 0:
            fout.write(mas_type0)
        elif mas_type == 1:
            fout.write(mas_type1)
        elif mas_type == 2:
            fout.write(mas_type2)
        elif mas_type == 3:
            fout.write(mas_type3)
        else:
            raise RuntimeError("invalid map_type")

        if mas_type == 1:
            base_offset = 28 + len(files) * 256
        else:
            base_offset = 24 + len(files) * 256

        file_table = []
        offset = 0
        fout.seek(base_offset)
        for name in files:
            filename = os.path.join(inputdir, name)

            print("processing %s" % filename)
            with open(filename, "rb") as fin:
                data = fin.read()
                deflated_data = zlib.compress(data)

                file_table.append((name, offset, len(data), len(deflated_data)))
                fout.write(deflated_data)
                offset += len(deflated_data)

        data_size = offset
        if mas_type == 1:
            fout.seek(20)
        else:
            fout.seek(16)
        fout.write(struct.pack("<ll", len(file_table), data_size))

        # write file table to the start of the file
        if mas_type == 1:
            fout.seek(28)
        else:
            fout.seek(24)

        for name, offset, size, zsize in file_table:
            print("%8d %8d %8d %s" % (offset, size, zsize, name))

            if mas_type == 0:
                fout.write(struct.pack("<4xlll240s", offset, size, zsize, bytearray(name, "latin-1")))
            elif mas_type == 1:
                fout.write(struct.pack("<4x236slll4x", os.fsencode(name), offset, size, zsize))
            elif mas_type == 2:
                fout.write(struct.pack("<4x16slll4x", os.fsencode(name), offset, size, zsize))
            elif mas_type == 3:
                fout.write(struct.pack("<4xlll4x236s", os.fsencode(name), offset, size, zsize))
            else:
                raise RuntimeError("invalid map_type")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor MAS packer')
    parser.add_argument('INPUTDIR', action='store', type=str,
                        help='directory to be packed')
    parser.add_argument('MASFILE', action='store', type=str,
                        help='output file')
    parser.add_argument('-t', '--type', metavar='INT', action='store', type=int, default=1,
                        help='MAS file type (0: GTL/GTR2, 1: rFactor, GSC2013)')
    args = parser.parse_args()

    mas_pack(args.INPUTDIR, args.MASFILE, args.type)

# EOF #
