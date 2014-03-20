# rFactor .scn/.gen file manipulation tool
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


import io
import os
import re
import traceback

import rfactortools


skybox_rfactor = """Instance=skyboxi
{
  Moveable=True
  MeshFile=skyboxi.gmt CollTarget=False HATTarget=False
  Instance=Sky
  {
    Moveable=True
    MeshFile=Sky.gmt CollTarget=False HATTarget=False
  }
}
"""

skybox_gsc2013 = """Instance=skyboxi
{
  Moveable=True
  MeshFile=skyboxi.gmt CollTarget=False HATTarget=False Render=False
  Instance=CLOUDS
  {

    Moveable=True
    MeshFile=skyboxi.gmt CollTarget=False HATTarget=False
    Instance=Sky
    {
      Moveable=True
      MeshFile=Sky.gmt CollTarget=False HATTarget=False
    }
  }
}
"""


def modify_vehicle_file(gen, search_path, mas_files, vehdir, teamdir):
    strio = io.StringIO()
    sr_parser = rfactortools.SearchReplaceScnParser(strio)
    sr_parser.mas_files = mas_files
    sr_parser.search_path = search_path
    rfactortools.process_scnfile(gen, sr_parser)

    with open(gen, "wt", encoding="latin-1", newline='\r\n', errors="replace") as fout:
        fout.write(strio.getvalue())


def gen_check_errors(search_path, mas_files, vehdir, teamdir):
    def expand_path(p):
        p = re.sub(r'<VEHDIR>', (vehdir + "/").replace("\\", "\\\\"), p)
        p = re.sub(r'<TEAMDIR>', (teamdir + "/").replace("\\", "\\\\"), p)
        return p

    expanded_search_path = [expand_path(d) for d in search_path]

    errors = []
    warnings = []

    for p, d in zip(search_path, expanded_search_path):
        if not rfactortools.directory_exists(d) and d != ".":
            print("warning: couldn't locate SearchPath %s" % p)
            warnings.append("warning: couldn't locate SearchPath %s" % p)

    default_mas_files = ["cmaps.mas"]
    for mas in mas_files:
        mas_found = False
        for d in expanded_search_path:
            f = os.path.join(d, mas)
            if rfactortools.file_exists(f):
                mas_found = True
                break
        if not mas_found and mas.lower() not in default_mas_files:
            print("error: couldn't locate %s" % mas)
            errors.append("error: couldn't locate %s" % mas)

    return errors, warnings


def process_gen_directory(directory, fix):
    gen_files = []
    veh_files = []
    gdb_files = []
    scn_files = []
    mas_files = []

    for fname in rfactortools.find_files(directory):
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
            rfactortools.process_gdb_file(gdb, fix, errors)
        except Exception:
            e = traceback.format_exc()
            print("error:\n%s\n" % e)
            errors.append(e)

    for veh in sorted(veh_files):
        try:
            rfactortools.process_veh_file(veh, fix, errors)
        except Exception as e:
            e = traceback.format_exc()
            print("raised error:\n%s\n" % e)
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


# EOF #
