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

import re
import os
import rfactortools

def show_buildin_ids():
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

keyvalue_regex = re.compile(r'^\s*([^=]+)\s*=\s*(.*)\s*')
comment_regex = re.compile(r'(.*?)(//.*)')

class Car:
    def __init__(self):
        self.class_id = None
        self.model_id = None
        self.classes = None
        self.model = None

def read_ids(filename):
    with open(filename, 'rt', encoding='latin-1') as fin:
        lines = fin.read().splitlines()

    car = Car()
    for line in lines:
        m = comment_regex.match(line)
        if m:
            comment = m.group(2)
            line = m.group(1)
        else:
            comment = None

        m = keyvalue_regex.match(line)
        if m:
            key, value = m.group(1).lower(), m.group(2)
            if key == "classid":
                car.class_id = int(value)
            elif key == "classes":
                car.classes = value
            elif key == "model":
                car.model = value
            elif key == "modelid":
                car.model_id = int(value)
            else:
                pass # print("unhandled: \"%s\"" % key)
        
    return car

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Race07 Tool")
    parser.add_argument('DIRECTORY', action='store', type=str,
                        help='directory containing .gen and .veh files')
    parser.add_argument('-c', '--class-id', action='store_true', default=False,
                        help='list class ids')
    parser.add_argument('-m', '--model-id', action='store_true', default=False,
                        help='list model ids')
    parser.add_argument('-i', '--inverse', action='store_true', default=False,
                        help='list free ids, instead of occupied ones')
    args = parser.parse_args()

    if args.DIRECTORY:
        vfs = rfactortools.VFS(args.DIRECTORY)
        for fname in vfs.files():
            ext = os.path.splitext(fname)[1].lower()
            if ext == ".car" or ext == ".inccar":
                car = read_ids(fname)
                if not car.class_id and not car.model_id:
                    pass
                elif not car.class_id or not car.model_id:
                    print("%4s %4s %s" % (car.class_id, car.model_id, fname))
                else:
                    print("%4d %4d %s" % (car.class_id, car.model_id, fname))

# EOF #
