#!/usr/bin/env python3

##  Race07 Tool
##  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

import argparse

import rfactortools.race07

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Race07 Tool")
    parser.add_argument('-c', '--class-id', action='store_true', default=False,
                        help='list class ids')
    parser.add_argument('-m', '--model-id', action='store_true', default=False,
                        help='list model ids')
    parser.add_argument('-i', '--inverse', action='store_true', default=False,
                        help='list free ids, instead of occupied ones')
    args = parser.parse_args()

    if args.class_id:
        if args.inverse:
            for i in range(0, 256):
                if not i in rfactortools.race07.class_ids:
                    print(i)
        else:
            for id, name in rfactortools.race07.class_ids.items():
                print("%3d: %r" % (id, name))

    if args.model_id:
        if args.inverse:
            for i in range(0, 256):
                if not i in rfactortools.race07.class_ids:
                    print(i)
        else:
            for id, name in rfactortools.race07.model_ids.items():
                print("%3d: %r" % (id, name))

# EOF #
