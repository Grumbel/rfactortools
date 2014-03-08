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


import re
import ntpath
import os

import rfactortools


keyvalue_regex = re.compile(r'^\s*([^=]+)\s*=\s*(.*)\s*')
comment_regex = re.compile(r'(.*?)(//.*)')
section_start_regex = re.compile(r'\s*{')
section_end_regex = re.compile(r'\s*}')

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


def nt2os_path(path):
    return os.path.normpath(path.replace(ntpath.sep, os.path.sep))


class ScnParser:

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


class InfoScnParser(ScnParser):

    def __init__(self):
        self.section = 0
        self.search_path = []
        self.mas_files = []
        self.has_skyboxi = None

    def on_key_value(self, key, value, comment, orig):
        if self.section == 0:
            if key.lower() == "masfile":
                value = nt2os_path(value)
                if os.path.isabs(value):
                    value = value[1:]
                self.mas_files.append(value)
            elif key.lower() == "searchpath":
                self.search_path.append(nt2os_path(value))
            elif key.lower() == "instance" and value.lower() == "skyboxi":
                self.has_skyboxi = True
            else:
                pass

    def on_section_start(self, comment, orig):
        self.section += 1

    def on_section_end(self, comment, orig):
        self.section -= 1


class SearchReplaceScnParser(ScnParser):

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
                if self.search_path is not None:
                    for p in self.mas_files:
                        self.fout.write("MASFile=%s\n" % ntpath.normpath(p))

                    self.mas_files = None
                    self.delete_mas_file = True

                elif not self.delete_mas_file:
                    self.fout.write(orig + '\n')

            elif key.lower() == "searchpath":
                if self.search_path is not None:
                    for p in self.search_path:
                        self.fout.write("SearchPath=%s\n" % ntpath.normpath(p))

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


def process_scnfile(vfs, filename, parser):
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
            m_sec_stop = section_end_regex.match(line)
            if m:
                key, value = m.group(1), m.group(2)
                parser.on_key_value(key, value.strip(), comment, orig_line)
            elif m_sec_start:
                parser.on_section_start(comment, orig_line)
            elif m_sec_stop:
                parser.on_section_end(comment, orig_line)
            else:
                parser.on_unknown(orig_line)


def modify_track_file(vfs, scn):
    outfile = scn + ".new"
    with open(outfile, "wt", encoding="latin-1", errors="replace") as fout:
        sr_parser = rfactortools.SearchReplaceScnParser(fout)
        # sr_parser.mas_files   =
        # sr_parser.search_path =
        rfactortools.process_scnfile(vfs, scn, sr_parser)


def modify_vehicle_file(vfs, gen, search_path, mas_files, vehdir, teamdir):
    outfile = gen + ".new"
    with open(outfile, "wt", encoding="latin-1", errors="replace") as fout:
        sr_parser = rfactortools.SearchReplaceScnParser(fout)
        sr_parser.mas_files = mas_files
        sr_parser.search_path = search_path
        rfactortools.process_scnfile(vfs, gen, sr_parser)


def gen_check_errors(vfs, search_path, mas_files, vehdir, teamdir):
    def expand_path(p):
        p = re.sub(r'<VEHDIR>', vehdir + "/", p)
        p = re.sub(r'<TEAMDIR>', teamdir + "/", p)
        return p

    expanded_search_path = [expand_path(d) for d in search_path]

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


def process_gen_directory(directory, fix):
    vfs = rfactortools.VFS(directory)

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
            rfactortools.process_gdb_file(vfs, gdb, fix, errors)
        except Exception:
            e = traceback.format_exc()
            print("error:\n%s\n" % e)
            errors.append(e)

    for veh in sorted(veh_files):
        try:
            rfactortools.process_veh_file(vfs, veh, fix, errors)
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
