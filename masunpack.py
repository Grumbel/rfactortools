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
import argparse

mas_type0 = b"GMOTORMAS10\0\0\0\0\0"
mas_type1 = b"\xC8\xCF\xD2\xD8\xCE\xD8\xE6\xC9\xCA\xDD\xD8\xBE\xBB\xA6\xBF\x90"
mas_type3 = b"CUBEMAS4.10\0\0\0\0\0"

def get_mas_type(signature):
    if signature == mas_type0:
        return 0
    elif signature == mas_type1:
        return 1
    elif signature == mas_type3:
        return 3
    else:
        raise RuntimeError("unknown mas type: %s" % signature)

def mas_unpack(masfile, outdir):
    with open(masfile, "rb") as fin:
        signature = fin.read(16)

        mas_type = get_mas_type(signature)

        if mas_type == 1:
            file_count, data_size = struct.unpack("<4xll", fin.read(12))
        else:
            file_count, data_size = struct.unpack("<ll", fin.read(8))

        file_table = []
        for i in range(0, file_count):

            if mas_type == 0:
                offset, size, zsize, name = struct.unpack("<4xlll240s", fin.read(256))
            elif mas_type == 1:
                name, offset, size, zsize = struct.unpack("<4x236slll4x", fin.read(256))
            elif mas_type == 2:
                name, offset, size, zsize = struct.unpack("<4x16slll4x", fin.read(256))
            elif mas_type == 3:
                name, offset, size, zsize = struct.unpack("<4xlll4x236s", fin.read(256))
            else:
                raise RuntimeError("invalid map_type")

            # No support for ASCIZ strings struct.unpack, thus ugly hackery
            name = os.fsdecode(name.split(b'\0', 1)[0])

            file_table.append((name, offset, size, zsize))

        # base_offset = 28 + file_count * 256
        base_offset = fin.tell()

        # extracting the data
        if not outdir:
            print("%-8s %-8s %-8s %-8s" % ("offset:", "size:", "zsize:", "name:"))
            for name, offset, size, zsize in file_table:
                print("%8d %8d %8d %s" % (offset, size, zsize, name))

            print()
            print("number of files:       %12d" % len(file_table))
            print("header file_count:     %12d" % file_count)
            print()
            print("total extracted size:  %12d" % sum([s for (_, _, s, _) in file_table]))
            print()
            print("total compressed size: %12d" % sum([zs for (_, _, _, zs) in file_table]))
            print("header data_size:      %12d" % data_size)

        else:
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
    parser = argparse.ArgumentParser(description='rFactor MAS packer')
    parser.add_argument('MASFILE', action='store', type=str,
                        help='.mas file to unpack')
    parser.add_argument('OUTDIR', action='store', type=str, nargs='?',
                        help='output directory')
    parser.add_argument('-l', '--list', action='store_true',
                        help="list only, don't extract")
    args = parser.parse_args()

    if args.list:
        mas_unpack(args.MASFILE, args.OUTDIR)
    else:
        mas_unpack(args.MASFILE, args.OUTDIR)

# EOF #
