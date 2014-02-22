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

import re
import ntpath
import os

def nt2os_path(path):
    return path.replace(ntpath.sep, os.path.sep)

def process_vehfile(vfs, filename):
    keyvalue_regex = re.compile(r'^\s*([^=]+)\s*=\s*(.*)\s*')
    comment_regex = re.compile(r'(.*?)(//.*)')

    # print("processing", filename)
    graphics_file = None

    with vfs.open_read(filename, encoding='latin-1') as fin:
        for orig_line in fin.read().splitlines():
            line = orig_line

            m = comment_regex.match(line)
            if m:
                comment = m.group(2)
                line = m.group(1)
            else:
                comment = None

            m = keyvalue_regex.match(line)
            if m:
                key, value = m.group(1), m.group(2)
                if key.lower() == "graphics":
                    graphics_file = value.strip()

    return nt2os_path(graphics_file)

# EOF #