#!/usr/bin/env python3

##  rFactor MAS unpacker
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

def mas_unpack(masfile, outdir):
    with open(masfile, "rb") as fin:
        signature = fin.read(16)

        if signature != mas_type1:
            print("unknown MAS type")
        else:
            file_count, data_size = struct.unpack("<4xll", fin.read(12))

            file_table = []
            for i in range(0, file_count):
                name, offset, size, zsize = struct.unpack("<4x236slll4x", fin.read(256))
                name = name.decode("ascii").rstrip('\0')
                file_table.append((name, offset, size, zsize))

            # base_offset = 28 + file_count * 256
            base_offset = fin.tell()

            # extracting the data
            os.mkdir(outdir)
            for name, offset, size, zsize in file_table:
                fin.seek(base_offset + offset)
                data = fin.read(zsize)

                outfile = os.path.join(outdir, name)
                print("%8d %8d %8d %s" % (offset, size, zsize, outfile))
                with open(outfile, "wb") as fout:
                    inflated_data = zlib.decompress(data)
                    if len(inflated_data) != size:
                        raise RuntimeError("invalid inflated size %d for %s should be %d" % (len(inflated_data), name, size))
                    fout.write(inflated_data)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: %s MASFILE OUTDIR" % sys.argv[0])
    else:
        mas_unpack(sys.argv[1], sys.argv[2])

# EOF #
