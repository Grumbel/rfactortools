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


def process_gdb_file(gdb, fix, errors, fout):
    scn = os.path.splitext(gdb)[0] + ".scn"

    info = rfactortools.InfoScnParser()
    rfactortools.process_scnfile(scn, info)

    fout.write("[Track]\n")
    fout.write("  gdb: %s\n" % gdb)
    fout.write("  scn: %s\n" % scn)
    fout.write("  SearchPath: %s\n" % info.search_path)
    fout.write("    MasFiles: %s\n" % info.mas_files)
    fout.write("\n")


# EOF #
