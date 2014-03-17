#!/usr/bin/env python3

# rFactor .gen file manipulation tool
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
import argparse

import rfactortools


def process_directory(directory):
    vfs = rfactortools.VFS(directory)

    for fname in vfs.files():
        name, ext = os.path.splitext(fname)
        if ext.lower() == ".veh":
            img = os.path.join(name + "number.tga")
            if vfs.file_exists(img):
                rfactortools.resize_to_fit_img_file(vfs.lookup_file(img), 252, 64)

        elif ext.lower() == ".gdb":
            img = os.path.join(name + "mini.tga")
            if vfs.file_exists(img):
                rfactortools.resize_to_fit_img_file(vfs.lookup_file(img), 252, 249)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor thumbnail image resizer')
    parser.add_argument('DIRECTORY', action='store', type=str,
                        help='images in DIRECTORY will be resized to GSC2013 standards')
    args = parser.parse_args()

    process_directory(args.DIRECTORY)


# EOF #
