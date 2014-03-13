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


import argparse
import os
import io
import sys

import rfactortools


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor .sfx processor')
    parser.add_argument('FILE', action='store', type=str, nargs='+',
                        help='.sfx file or directory containing .sfx files')
    parser.add_argument('-m', '--modify', metavar='PREFIX', type=str,
                        help="add PREFIX to the .wav path")
    parser.add_argument('-i', '--in-place', action='store_true', default=False,
                        help="overwrite origin files after changes")
    parser.add_argument('-b', '--backup', metavar='SUFFIX', type=str,
                        help="make backups by copying file to FILENAME+SUFFIX before changing")
    args = parser.parse_args()

    files = []
    for path in args.FILE:
        if os.path.isdir(path):
            files += rfactortools.find_files(path, ".sfx")
        else:
            files.append(path)

    if args.modify is None:
        for filename in files:
            wavs = rfactortools.parse_sfxfile(filename)
            for wav in wavs:
                print(wav)
    else:
        for filename in files:
            sout = io.StringIO()
            rfactortools.modify_sfxfile(sout, filename, args.modify)

            if args.in_place:
                if args.backup:
                    os.rename(filename, filename + args.backup)
                with open(filename, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
                    fout.write(sout.getvalue())
            else:
                sys.stdout.write(sout.getvalue())


# EOF #
