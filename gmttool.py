#!/usr/bin/env python3

##  rFactor .scn/.gen file manipulation tool
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

import os
import sys
import struct
import argparse

def asciz2py(bytes):
    return bytes.split(b'\0', 1)[0].decode('latin-1')

def process_gmt(filename, verbose=False):
    print("[[[ %s ]]]" % filename)
    with open(filename, "rb") as fin:       
        d_obj_offset, d_32 = struct.unpack("<II", fin.read(8))

        print("%8d  %8d" % (d_obj_offset, d_32))
        print()

        fin.seek(372)
        d_obj_count, d_offset = struct.unpack("<II", fin.read(8))
        print("%8d  %8d" % (d_obj_count, d_obj_count))
        print("%8d  %8d" % struct.unpack("<II", fin.read(8)))
        print("%8d  %8d" % struct.unpack("<II", fin.read(8)))
        print("%8d  %8d" % struct.unpack("<II", fin.read(8)))

        fin.seek(404)
        print("Name:", repr(asciz2py(fin.read(64))))

        fin.seek(d_obj_offset)
        d_num1, d_num2, d_num3 = struct.unpack("<HHI", fin.read(8))
        print("numbers:", d_num1, d_num2, d_num3)

        for obj_idx in range(0, d_obj_count):
            d_obj_name = asciz2py(struct.unpack("60s", fin.read(60))[0])
            print("obj:", repr(d_obj_name))

            fin.read(92)

            for i in range(0, 3):
                d_mat_name = asciz2py(fin.read(264))
                print("  mat:", repr(d_mat_name))

            d_obj_junk = fin.read(24)

            d_tex_count = struct.unpack("<I", fin.read(4))[0]
            print("  tex_count:", d_tex_count)
            for tex_idx in range(0, d_tex_count):
                print("  tex:", fin.read(64))

                d = fin.read(4)
                print(struct.unpack("<HH", d))
                d_tex_payload = struct.unpack("<I", d)[0]
                print("    tex_payload:", d_tex_payload)

                if d_tex_payload == 32793:
                    # this one is for Sky.gmt
                    d_tex_rest = fin.read(144)
                    # print("  rest:", d_tex_rest)
                    
                elif d_tex_payload == 24 or d_tex_payload == 152:
                    d_tex_rest = fin.read(104)
                    # print("    rest:", d_tex_rest)
                    
                elif d_tex_payload == 131096:
                    d_tex_rest = fin.read(104)
                    print("    rest:", d_tex_rest)

                elif d_tex_payload == 131098:
                    d_tex_rest = fin.read(104)
                    print("    rest:", d_tex_rest)
                    
                else:
                    d_tex_rest = fin.read(255)
                    print("    rest:", d_tex_rest)

                    raise Exception("unknown tex_payload value: %s" % d_tex_payload)

        if verbose:
            fin.seek(0)
            i = 0
            while fin:
                data = fin.read(4)
                print("%08d    %8d %8d    %12d    %20.3f    %s" %
                      (i*4, 
                       struct.unpack("<HH", data)[0], 
                       struct.unpack("<HH", data)[1],
                       struct.unpack("<I", data)[0],
                       struct.unpack("<f", data)[0],
                       struct.unpack("<4s", data)[0]))
                i += 1

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor .gmt processor')
    parser.add_argument('FILE', action='store', type=str, nargs='+',
                        help='.gmt files to process')
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()

    for f in args.FILE:
        process_gmt(f, args.verbose)
        print()

# EOF #
