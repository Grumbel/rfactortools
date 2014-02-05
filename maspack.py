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

mas_type1 = b"\xC8\xCF\xD2\xD8\xCE\xD8\xE6\xC9\xCA\xDD\xD8\xBE\xBB\xA6\xBF\x90"

def mas_pack(inputdir, masfile):
    with open(masfile, "wb") as fout:
        files = os.listdir(inputdir)
        
        fout.write(mas_type1)

        base_offset = 28 + len(files) * 256

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

        fout.seek(16)
        data_size = offset - base_offset
        fout.write(struct.pack("<4xll", len(file_table), data_size))

        # write file table to the start of the file
        fout.seek(28)
        for name, offset, size, zsize in file_table:
            print("%8d %8d %8d %s" % (offset, size, zsize, name))
            fout.write(struct.pack("<4x236slll4x", bytearray(name, "ascii"), offset, size, zsize))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: %s INPUTDIR OUTPUTMASFILE" % sys.argv[0])
    else:
        mas_pack(sys.argv[1], sys.argv[2])

# EOF #
