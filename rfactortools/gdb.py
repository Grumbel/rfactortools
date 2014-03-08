# rFactor .gdb file manipulation tool
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
import rfactortools


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


# EOF #