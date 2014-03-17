#!/usr/bin/env python3

# rFactor .scn/.gen file manipulation tool
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

        fin.seek(8)
        print("bounding box: %10.4f" % struct.unpack("<f", fin.read(4)))
        for i in range(0, 33):
            print("%8d) %10.4f  %10.4f  %10.4f" % ((i,) + struct.unpack("<fff", fin.read(12))))
        print()

        fin.seek(372)
        d_obj_count, d_offset, \
            d_obj1_count, d_offset1, \
            d_obj2_count, d_offset2, \
            d_obj3_count, d_offset3 = struct.unpack("<IIIIIIII", fin.read(32))
        print("%8d  %8d" % (d_obj_count, d_offset))
        print("%8d  %8d" % (d_obj1_count, d_offset1))
        print("%8d  %8d" % (d_obj2_count, d_offset2))
        print("%8d  %8d" % (d_obj3_count, d_offset3))

        fin.seek(d_obj_offset)
        d_num1, d_num2, d_num3 = struct.unpack("<HHI", fin.read(8))
        print("numbers:", d_num1, d_num2, d_num3)

        fin.seek(404)
        print("Name:", fin.read(64))
        print(struct.unpack("<HHffHH", fin.read(16)))  # always: C8 CF D2 D8   CE D8 E6 C3 D6  DE D8 BE   BB A8 BF 90

        # parse some floats
        fin.seek(d_offset1 + 4)
        for i in range(0, d_obj1_count):
            d_f_nums = struct.unpack("<HHHHIIffff", fin.read(32))
            print("1 %8d) %s" % (i, d_f_nums))
        print()

        # parse some more floats
        fin.seek(d_offset2 + 4)
        for i in range(0, d_obj2_count):
            d_f_nums = struct.unpack("<fffI", fin.read(16))
            print("2 %8d) %s" % (i, d_f_nums))
        print()

        # parse even more floats
        fin.seek(d_offset3)
        for i in range(0, d_num1 + 3):
            d_f_nums = struct.unpack("<ffffffII", fin.read(32))
            print("3 %8d) %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %8d %8d" % ((i,) + d_f_nums))
        print()

        for i in range(0, d_num1 + 2):
            d_f_nums = struct.unpack("<ffffffII", fin.read(32))
            print("4 %8d) %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %8d %8d" % ((i,) + d_f_nums))
        print()

        for i in range(0, d_obj1_count):
            d_f_nums = struct.unpack("<HHH", fin.read(6))
            print("5 %8d) %s" % (i, d_f_nums))
        print()

        print("Current Offset:", fin.tell())

        # for i in range(0, d_obj1_count*5):
        #    d_f_nums = struct.unpack("<HHHHHH", fin.read(12))
        #    print("6 %8d %s" % (i, d_f_nums))

        fin.seek(d_obj_offset + 8)
        for obj_idx in range(0, d_obj_count):
            d = fin.read(60)
            print(d)
            d_obj_name = asciz2py(struct.unpack("60s", d)[0])
            print("obj:", repr(d_obj_name))

            fin.read(92)

            for i in range(0, 3):
                d_mat_name = asciz2py(fin.read(264))
                print("  mat:", repr(d_mat_name))

            fin.read(24)  # junk?

            d_tex_count = struct.unpack("<I", fin.read(4))[0]
            print("  tex_count:", d_tex_count)
            for tex_idx in range(0, d_tex_count):
                print("  tex:", fin.read(64))

                d_tex_type, d_tex_something = struct.unpack("<HH", fin.read(4))
                print("    tex_type:", d_tex_type, d_tex_something)

                # ffff ffff
                # 0200 0000
                # 0200 0000
                # 0000 0000
                # 0100 0000
                # 0600 0000
                # 0000 803f
                # 0100 0000
                # 0000 0000
                # 0100 0000
                # 0000 0000
                # ....

                def block_fmt(str):
                    for i, c in enumerate(str):
                        sys.stdout.write("%02x " % c)
                        if (i + 1) % 4 == 0:
                            sys.stdout.write(" ")

                        if (i + 1) % 16 == 0:
                            sys.stdout.write("\n")
                    sys.stdout.write("\n")

                if d_tex_type == 32793:
                    # this one is for Sky.gmt
                    d_tex_rest = fin.read(144)
                    print("  rest:")
                    block_fmt(d_tex_rest)

                elif d_tex_type == 24 or d_tex_type == 152:
                    d_tex_rest = fin.read(104)  # fin.read(128)
                    print("  rest:")
                    block_fmt(d_tex_rest)

                elif d_tex_type == 26:  # 131096
                    d_tex_rest = fin.read(104)
                    print("  rest:")
                    block_fmt(d_tex_rest)

                else:
                    # d_tex_rest = fin.read(255)
                    # print("    rest:", block_fmt(d_tex_rest))

                    raise Exception("unknown tex_payload value: %s" % d_tex_type)

        if verbose:
            fin.seek(0)
            i = 0
            while fin:
                data = fin.read(4)
                print("%08d    %8d %8d    %12d    %20.3f    %s" %
                      (i * 4,
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
