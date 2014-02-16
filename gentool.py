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
import os
import ntpath
from io import StringIO
from vfs import VFS

keyvalue_regex = re.compile(r'^\s*([^=]+)\s*=\s*(.*)\s*')
comment_regex = re.compile(r'(.*?)(//.*)')
section_start_regex = re.compile(r'\s*{')
section_end_regex = re.compile(r'\s*}')

def process_vehfile(vfs, filename):
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

class GenParser:
    def __init__(self):
        pass

    def on_key_value(self, key, value, comment, orig):
        pass

    def on_section_start(self, comment, orig):
        pass

    def on_section_end(self, comment, orig):
        pass

    def on_unknown(self, orig):
        pass

class InfoGenParser(GenParser):
    def __init__(self):
        self.section = 0
        self.search_path = []
        self.mas_files = []
        self.has_skyboxi = None

    def on_key_value(self, key, value, comment, orig):
        if self.section == 0:
            if key.lower() == "masfile":
                self.mas_files.append(value)
            elif key.lower() == "searchpath":
                self.search_path.append(value)
            elif key.lower() == "instance" and value.lower() == "skyboxi":
                self.has_skyboxi = True
            else:
                pass

    def on_section_start(self, comment, orig):
        self.section += 1

    def on_section_end(self, comment, orig):
        self.section -= 1

class SearchReplaceGenParser(GenParser):
    def __init__(self, fout):
        self.fout = fout

        self.section = 0
        self.search_path = None
        self.mas_files = None

        self.delete_search_path = False
        self.delete_mas_file = False

        self.delete_next_section = False

    def on_key_value(self, key, value, comment, orig):
        if self.section == 0:
            if key.lower() == "instance" and value.lower() == "skyboxi":
                self.delete_next_section = True
            elif key.lower() == "masfile":
                if self.search_path != None:
                    for p in self.mas_files:
                        self.fout.write("MASFile=%s\n" % p)

                    self.mas_files = None
                    self.delete_mas_file = True

                elif not self.delete_mas_file:
                    self.fout.write(orig + '\n')

            elif key.lower() == "searchpath":
                if self.search_path != None:
                    for p in self.search_path:
                        self.fout.write("SearchPath=%s\n" % p)

                    self.search_path = None
                    self.delete_search_path = True

                elif not self.delete_search_path:
                    self.fout.write(orig + '\n')

            else:
                self.fout.write(orig + '\n')
        else:
            if self.delete_next_section and self.section > 0:
                pass
            else:
                self.fout.write(orig + '\n')

    def on_section_start(self, comment, orig):
        self.section += 1
        if not self.delete_next_section:
            self.fout.write(orig + '\n')

    def on_section_end(self, comment, orig):
        self.section -= 1
        if not self.delete_next_section:
            self.fout.write(orig + '\n')
        else:
            if self.delete_next_section and self.section == 0:
                self.delete_next_section = False

    def on_unknown(self, orig):
        self.fout.write(orig + '\n')

def process_genfile(vfs, filename, parser):
    # print("processing", filename)
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
            m_sec_start = section_start_regex.match(line)
            m_sec_stop  = section_end_regex.match(line)
            if m:
                key, value = m.group(1), m.group(2)
                parser.on_key_value(key, value, comment, orig_line)
            elif m_sec_start:
                parser.on_section_start(comment, orig_line)
            elif m_sec_stop:
                parser.on_section_end(comment, orig_line)
            else:
                parser.on_unknown(orig_line)

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
        sr_parser = SearchReplaceGenParser(fout)
        # sr_parser.mas_files   =
        # sr_parser.search_path =
        process_genfile(vfs, scn, sr_parser)

def modify_vehicle_file(vfs, gen, search_path, mas_files, vehdir, teamdir):
    with open("/tmp/out.gen", "w") as fout:
        sr_parser = SearchReplaceGenParser(fout)
        sr_parser.mas_files   = mas_files
        sr_parser.search_path = search_path
        process_genfile(vfs, gen, sr_parser)

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

            info = InfoGenParser()
            process_genfile(vfs, scn, info)
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
            info = InfoGenParser()
            process_genfile(vfs, gen, info)
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
