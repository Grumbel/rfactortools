#!/usr/bin/env python3

# rFactor .scn/.gen file manipulation tool
# Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
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

from io import StringIO
import argparse
import ntpath
import os
import re
import traceback

import rfactortools


def process_directory(directory, fix):
    vfs = rfactortools.VFS(directory)

    gen_files = []
    veh_files = []
    gdb_files = []
    scn_files = []
    mas_files = []

    for fname in vfs.files():
        ext = os.path.splitext(fname)[1].lower()
        if ext == ".gen":
            gen_files.append(fname)
        elif ext == ".veh":
            veh_files.append(fname)
        elif ext == ".scn":
            scn_files.append(fname)
        elif ext == ".gdb":
            gdb_files.append(fname)
        elif ext == ".mas":
            mas_files.append(fname)

    errors = []
    for gdb in sorted(gdb_files):
        try:
            rfactortools.process_gdb_file(vfs, gdb, fix, errors)
        except Exception:
            e = traceback.format_exc()
            print("error:\n%s\n" % e)
            errors.append(e)

    for veh in sorted(veh_files):
        try:
            rfactortools.process_veh_file(vfs, veh, fix, errors)
        except Exception as e:
            e = traceback.format_exc()
            print("raised error:\n%s\n" % e)
            errors.append(e)

    print("[MASFiles]")
    for mas in sorted(mas_files):
        print("  %s" % mas)
    print()

    if errors:
        print("Error summary:")
        print("==============")
        for e in errors:
            print("error:", e)
    else:
        print("No errors")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor .veh/.gen processor')
    parser.add_argument('DIRECTORY', action='store', type=str,
                        help='directory containing .gen and .veh files')
    parser.add_argument('-c', '--check', action='store_true', default=False,
                        help="check the file for errors")
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    parser.add_argument('--add-searchpath', metavar="PATH",
                        help="be more verbose")
    parser.add_argument('-f', '--fix', action='store_true', default=False,
                        help="try to fix all detected errors")
    args = parser.parse_args()

    process_directory(args.DIRECTORY, args.fix)


# EOF #
