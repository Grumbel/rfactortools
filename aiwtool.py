#!/usr/bin/env python3

# rFactor .aiw file processing tool
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
import PIL.Image

import rfactortools


if __name__ == "__main__":
    def size(s):
        return [int(x) for x in s.split('x', 1)]

    parser = argparse.ArgumentParser(description="rFactor Tools .aiw processor")
    parser.add_argument('FILE', action='store', type=str,
                        help=".aiw file to process")
    parser.add_argument("-o", "--output", metavar='FILE', type=str, required=True,
                        help=".aiw file to process")
    parser.add_argument("-s", "--size", metavar='INT', type=size, default=(512, 512),
                        help="size for the rendered image")
    args = parser.parse_args()

    aiw = rfactortools.parse_aiwfile(args.FILE)
    img = rfactortools.render_aiw(aiw, args.size[0], args.size[1])
    img.write_to_png(args.output)

    pil_img = PIL.Image.open(args.output)
    pil_img.save(args.output)


# EOF #
