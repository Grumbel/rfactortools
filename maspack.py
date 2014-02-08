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

class FileType:
    UNKNOWN = 0
    MISC = 16 # BIK, GFX, PSD, PSH, RTF, TXT, VSH
    GMT  = 17 
    BMP  = 18 
    SCN  = 19 
    TGA  = 20 
    PNG  = 21 
    JPG  = 22 
    # DDS  = 23 # FIXME: might need to inspect file content to tell the difference
    DDS  = 55

def get_file_type(filename):
    ext = os.path.splitext(filename)[1].lower()

    if ext == ".bik" or  ext == ".gfx" or ext == ".psd"  or ext == ".rtf" or ext == ".txt" or ext == ".vsh":
        return FileType.MISC
    elif ext == ".gmt":
        return FileType.GMT
    elif ext == ".bmp":
        return FileType.BMP
    elif ext == ".scn":
        return FileType.SCN
    elif ext == ".tga":
        return FileType.TGA
    elif ext == ".png":
        return FileType.PNG
    elif ext == ".jpg":
        return FileType.JPG
    elif ext == ".dds":
        return FileType.DDS # FIXME: need to handle the other DDS
    else:
        print("%s: warning: unknown file type" % filename)
        return FileType.UNKNOWN

def mas_pack(files, masfile, mas_type, files_from_file=True):
    with open(masfile, "wb") as fout:
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
        for filename in files:
            print("processing %s" % filename)
            name = os.path.basename(filename)
            file_type = get_file_type(filename)
            flags = 0

            with open(filename, "rb") as fin:
                data = fin.read()
                deflated_data = zlib.compress(data)

                file_table.append((file_type, flags, name, offset, len(data), len(deflated_data)))
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

        for file_type, flags, name, offset, size, zsize in file_table:
            print("%8d %8d %8d %s" % (offset, size, zsize, name))

            if mas_type == 0:
                fout.write(struct.pack("<4xlll240s", offset, size, zsize, bytearray(name, "latin-1")))
            elif mas_type == 1:
                fout.write(struct.pack("<BBxx236slll4x", file_type, flags, os.fsencode(name), offset, size, zsize))
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
    parser.add_argument('-i', '--inputfile', action='store_true', default=False,
                        help="Read input files from file instead of directory")
    parser.add_argument('-t', '--type', metavar='INT', action='store', type=int, default=1,
                        help='MAS file type (0: GTL/GTR2, 1: rFactor, GSC2013)')
    args = parser.parse_args()

    if args.inputfile:
        with open (args.INPUTDIR, "r") as fin:
            files = fin.read().splitlines()
    else:
        files = [os.path.join(args.INPUTDIR, name) for name in sorted(os.listdir(args.INPUTDIR))]

    mas_pack(files, args.MASFILE, args.type)

# EOF #
