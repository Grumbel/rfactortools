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

## Editing ###################################################################

def modify_track_file(vfs, scn):
    outfile = scn + ".new"
    with open(outfile, encoding="latin-1", errors="replace") as fout:
        sr_parser = rfactortools.SearchReplaceScnParser(fout)
        # sr_parser.mas_files   =
        # sr_parser.search_path =
        rfactortools.process_scnfile(vfs, scn, sr_parser)
        
def modify_vehicle_file(vfs, gen, search_path, mas_files, vehdir, teamdir):
    outfile = gen + ".new"
    with open(outfile, "wt", encoding="latin-1", errors="replace") as fout:
        sr_parser = rfactortools.SearchReplaceScnParser(fout)
        sr_parser.mas_files   = mas_files
        sr_parser.search_path = search_path
        rfactortools.process_scnfile(vfs, gen, sr_parser)

##############################################################################

def gen_check_errors(vfs, search_path, mas_files, vehdir, teamdir):
    def expand_path(p):
        p = re.sub(r'<VEHDIR>', vehdir + "/", p)
        p = re.sub(r'<TEAMDIR>', teamdir + "/", p)
        return p

    expanded_search_path = [ expand_path(d) for d in search_path ]

    errors = []

    for p, d in zip(search_path, expanded_search_path):
        if not vfs.directory_exists(d):
            print("error: couldn't locate SearchPath %s" % p)
            errors.append("error: couldn't locate SearchPath %s" % p)

    for mas in mas_files:
        mas_found = False
        for d in expanded_search_path:
            f = os.path.join(d, mas)
            if vfs.file_exists(f):
                mas_found = True
                break
        if not mas_found:
            print("error: couldn't locate %s" % mas)
            errors.append("error: couldn't locate %s" % mas)

    return errors

##############################################################################

def process_gdb_file(vfs, gdb, fix, errors):
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

    if fix:
        modify_track_file(vfs, scn)

def process_veh_file(vfs, veh, fix, errors):
    teamdir = os.path.dirname(veh)
    vehdir  = find_vehdir(os.path.dirname(veh))

    gen_name = rfactortools.process_vehfile(vfs, veh)
    gen = find_file_backwards(vfs, os.path.dirname(veh), gen_name)
    if not gen:
        raise Exception("%s: error: couldn't find .gen file '%s'" % (veh, gen_name))
    print("[Vehicle]")
    print("  veh:", veh)
    print("  gen:", gen)
    info = rfactortools.InfoScnParser()
    rfactortools.process_scnfile(vfs, gen, info)
    print("    <VEHDIR>:", vehdir)
    print("   <TEAMDIR>:", teamdir)
    print("  SearchPath:", info.search_path)
    print("    MasFiles:", info.mas_files)

    for err in gen_check_errors(vfs, info.search_path, info.mas_files, vehdir, teamdir):
        errors.append("%s: %s" % (gen, err))
    print()

    if fix:
        modify_vehicle_file(vfs, gen, info.search_path, info.mas_files, vehdir, teamdir)

def process_directory(directory, fix):
    vfs = VFS(directory)

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
            process_gdb_file(vfs, gdb, fix, errors)
        except Exception as e:
            print("error:", e)
            errors.append(e)           

    for veh in sorted(veh_files):
        try:
            process_veh_file(vfs, veh, fix, errors)
        except Exception as e:
            print("raised error:", e, "\n")
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
