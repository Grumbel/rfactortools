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

import argparse
import re
import os
import ntpath
from io import StringIO
from vfs import VFS

import rfactortools

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

        return graphics_file

def find_file_backwards(vfs, dir, gen):
    while True:
        filename = os.path.join(dir, gen)
        if vfs.file_exists(filename):
            return filename

        newdir = os.path.dirname(dir)
        if newdir == dir: # reached the root of the path
            return None
        else:
            dir = newdir

def find_vehdir(path):
    m = re.match(r'^(.*GameData/Vehicles)', path, re.IGNORECASE)
    if m:
        return m.group(1)
    else:
        raise Exception("couldn't locate <VEHDIR> in %s" % path)

def modify_track_file(vfs, scn):
    with open("/tmp/out.scn", "w") as fout:
        sr_parser = rfactortools.SearchReplaceScnParser(fout)
        # sr_parser.mas_files   =
        # sr_parser.search_path =
        rfactortools.process_scnfile(vfs, scn, sr_parser)
        
def modify_vehicle_file(vfs, gen, search_path, mas_files, vehdir, teamdir):
    with open("/tmp/out.gen", "w") as fout:
        sr_parser = rfactortools.SearchReplaceScnParser(fout)
        sr_parser.mas_files   = mas_files
        sr_parser.search_path = search_path
        rfactortools.process_scnfile(vfs, gen, sr_parser)

def process_directory(directory):
    gen_files = []
    veh_files = []
    gdb_files = []
    scn_files = []

    vfs = VFS(directory)

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

    # print(veh_files)
    errors = []
    for gdb in gdb_files:
        try:
            trackdir = os.path.dirname(gdb)
            scn = os.path.splitext(gdb)[0] + ".scn"

            info = rfactortools.InfoScnParser()
            rfactortools.process_scnfile(vfs, scn, info)
            print("[Track]")
            print("  gdb: %s" % gdb)
            print("  scn: %s" % scn)
            print("  SearchPath:", info.search_path)
            print("    MasFiles:", info.mas_files)
            print()
            modify_track_file(vfs, scn)

        except Exception as e:
            print("error:", e)
            errors.append(e)

    for veh in veh_files:
        try:
            teamdir = os.path.dirname(veh)
            vehdir  = find_vehdir(os.path.dirname(veh))

            gen_name = process_vehfile(vfs, veh)
            gen = find_file_backwards(vfs, os.path.dirname(veh), gen_name)
            if not gen:
                raise Exception("%s: error: couldn't find .gen file '%s'" % (veh, gen_name))
            print("[Vehicle]")
            print("  veh:", veh)
            print("  gen:", gen)
            info = rfactortools.InfoScnParser()
            rfactortools.process_scnfile(vfs, gen, info)
            print("  SearchPath:", info.search_path)
            print("    MasFiles:", info.mas_files)
            print("    <VEHDIR>:", vehdir)
            print("   <TEAMDIR>:", teamdir)
            print()

            modify_vehicle_file(vfs, gen, info.search_path, info.mas_files, vehdir, teamdir)

        except Exception as e:
            print("error:", e)
            errors.append(e)

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
    args = parser.parse_args()

    process_directory(args.DIRECTORY)

# EOF #
