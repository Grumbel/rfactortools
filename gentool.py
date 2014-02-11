#!/usr/bin/env python3

##  rFactor .gen file manipulation tool
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

import argparse
import re

keyvalue_regex = re.compile(r'^\s*([^=]+)\s*=([^\s]+)+')
comment_regex = re.compile(r'(.*?)(//.*)')
section_start_regex = re.compile(r'\s*{')
section_end_regex = re.compile(r'\s*}')

def process_genfile(filename):
    with open(filename, 'r') as fin:
        for orig_line in fin.read().splitlines():
            line = orig_line

            m = comment_regex.match(line)
            if m:
                comment = m.group(2)
                line = m.group(1)
            else:
                comment = None

            m = keyvalue_regex.match(line)
            m_sec_start = section_start_regex.match(line)
            m_sec_stop  = section_end_regex.match(line)
            if m:
                key, value = m.group(1), m.group(2)
                if key.lower() == "masfile":
                    if comment:
                        print("%s=%s %s" % (key, value, comment))
                    else:
                        print("%s=%s" % (key, value))
                elif key.lower() == "searchpath":
                    print(orig_line)
                else:
                    print(orig_line)
            elif m_sec_start:
                print(orig_line)
            elif m_sec_stop:
                print(orig_line)
            else:
                print(orig_line)

# TODO: possible operations:
#   MasFile=basename(MASFile)?
#   SearchPath=...
# Make it a two pass process, one to parse the .gen file, another to change it while preserving the original

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor MAS packer')
    parser.add_argument('FILE', action='store', type=str,
                        help='.gen file to process')
    parser.add_argument('-c', '--check', action='store_true', default=False,
                        help="check the file for errors")
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()


    process_genfile(args.FILE)

# EOF #
