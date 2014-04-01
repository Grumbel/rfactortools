# rFactor .veh file manipulation tool
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


from collections import defaultdict
import os
import posixpath
import re

import rfactortools


keyvalue_regex = re.compile(r'^\s*([^=]+)\s*=\s*(.*)\s*')
comment_regex = re.compile(r'(.*?)(//.*)')

quoted_string_regex = re.compile(r'"(.*)"')


def find_modname(path):
    path = path.replace(os.path.sep, posixpath.sep)

    m = re.match(r'^.*/Vehicles/([^/]+)', path, re.IGNORECASE)
    if m:
        return m.group(1)
    else:
        raise Exception("couldn't locate modname: %s" % path)


def find_vehdir(path):
    path = path.replace(os.path.sep, posixpath.sep)

    m = re.match(r'^(.*/Vehicles)', path, re.IGNORECASE)
    if m:
        return m.group(1)
    else:
        raise Exception("couldn't locate <VEHDIR> in %s" % path)


def find_file_backwards(directory, gen):
    while True:
        filename = os.path.join(directory, gen)
        if rfactortools.file_exists(filename):
            return filename

        newdir = os.path.dirname(directory)
        if newdir == directory:  # reached the root of the path
            return None
        else:
            directory = newdir


class Veh:

    def __init__(self):
        self.filename = None
        self.graphics_file = None
        self.spinner_file = None
        self.gen_string = None
        self.sounds = None
        self.cameras = None
        self.upgrades = None
        self.hd_vehicle = None
        self.eye_point = None
        self.driver = None
        self.team = None
        self.description = None
        self.category = None
        self.classes = []


def unquote(str):
    m = quoted_string_regex.match(str)
    if m:
        return m.group(1)
    return str


def parse_vehfile(filename):
    veh = Veh()

    veh.filename = filename

    with rfactortools.open_read(filename) as fin:
        for orig_line in fin.read().splitlines():
            line = orig_line

            m = comment_regex.match(line)
            if m:
                # comment = m.group(2)
                line = m.group(1)

            m = keyvalue_regex.match(line)
            if m:
                key, value = m.group(1), m.group(2)
                if key.lower() == "graphics":
                    veh.graphics_file = rfactortools.nt2posixpath(value.strip())
                elif key.lower() == "spinner":
                    veh.spinner_file = rfactortools.nt2posixpath(value.strip())
                elif key.lower() == "classes":
                    veh.classes = [c.strip() for c in unquote(value).split(",")]
                elif key.lower() == "category":
                    veh.category = [c.strip() for c in unquote(value).split(",")]
                elif key.lower() == "driver":
                    veh.driver = unquote(value)
                elif key.lower() == "team":
                    veh.team = unquote(value)
                else:
                    pass

    return veh


def append_errors(context, errs, warns, errors):
    for err in errs:
        errors.append("%s: %s" % (context, err))
    for warn in warns:
        errors.append("%s: %s" % (context, warn))


def process_scn_veh_file(modname, veh_filename, scn_short_filename, vehdir, teamdir, fix, errors, fout):
    # resolve scn_filename to a proper path
    scn_filename = find_file_backwards(os.path.dirname(veh_filename), scn_short_filename)
    if not scn_filename:
        raise Exception("error: couldn't find .gen file '%s' '%s'" % (veh_filename, scn_short_filename))

    fout.write("gen: %s\n" % scn_filename)

    info = rfactortools.InfoScnParser()
    rfactortools.process_scnfile(scn_filename, info)

    fout.write("  SearchPath: %s\n" % info.search_path)
    fout.write("    MasFiles: %s\n" % info.mas_files)
    fout.write("\n")

    if not fix:
        orig_errs, orig_warns = rfactortools.gen_check_errors(info.search_path, info.mas_files, vehdir, teamdir)
        append_errors(scn_filename, orig_errs, orig_warns, errors)
    else:
        # if there is a cmaps in the mod, use that instead of the one in <VEHDIR>
        cmaps = rfactortools.find_file(os.path.join(vehdir, modname), "cmaps.mas")
        if cmaps:
            cmaps = os.path.relpath(cmaps, vehdir)
            for i, m in enumerate(info.mas_files):
                if m.lower() == "cmaps.mas":
                    info.mas_files[i] = cmaps

        orig_errs, orig_warns = rfactortools.gen_check_errors(info.search_path, info.mas_files, vehdir, teamdir)

        if orig_errs:
            # add modname to the SearchPath to avoid errors
            search_path = [re.sub(r'<VEHDIR>', r'<VEHDIR>/%s' % modname, p) for p in info.search_path]

            rel_teamdir = os.path.relpath(teamdir, vehdir)
            while rel_teamdir:
                search_path.append(os.path.join("<VEHDIR>", rel_teamdir))
                rel_teamdir = os.path.dirname(rel_teamdir)
            search_path.append("<VEHDIR>")
        else:
            search_path = info.search_path

        # make list items unique
        search_path = sorted(set(search_path))

        new_errs, new_warns = rfactortools.gen_check_errors(search_path, info.mas_files, vehdir, teamdir)

        append_errors(scn_filename, new_errs, new_warns, errors)

        # write a new file if there are no or less errors
        if not new_errs or len(new_errs) < len(orig_errs):
            rfactortools.modify_vehicle_file(scn_filename, search_path, info.mas_files, vehdir, teamdir)
        elif cmaps:
            rfactortools.modify_vehicle_file(scn_filename, info.search_path, info.mas_files, vehdir, teamdir)


def process_veh_file(veh_filename, fix, errors, fout):
    teamdir = os.path.dirname(veh_filename)
    modname = find_modname(os.path.dirname(veh_filename))

    vehdir = find_vehdir(os.path.dirname(veh_filename))
    veh_obj = parse_vehfile(rfactortools.lookup_path_icase(veh_filename))

    fout.write("[Vehicle]\n")
    fout.write("veh: %s\n" % veh_filename)
    fout.write("    <VEHDIR>: %s\n" % vehdir)
    fout.write("   <TEAMDIR>: %s\n" % teamdir)
    fout.write("    graphics: %s\n" % veh_obj.graphics_file)
    fout.write("     spinner: %s\n" % veh_obj.spinner_file)

    if veh_obj.graphics_file is not None:
        process_scn_veh_file(modname, veh_filename, veh_obj.graphics_file, vehdir, teamdir, fix, errors, fout)

    if veh_obj.spinner_file is not None:
        process_scn_veh_file(modname, veh_filename, veh_obj.spinner_file, vehdir, teamdir, fix, errors, fout)


class Tree(defaultdict):

    """A recursize defaultdict()"""

    def __init__(self, value=None):
        super(Tree, self).__init__(Tree)
        self.value = value
        self.content = []


def _print_tree_rec(tree, indent, fout):
    for i, (k, v) in enumerate(sorted(tree.items())):
        if indent == "":
            sym = ""
            symc = " - "
        elif i < len(tree) - 1:
            sym = "+ "
            symc = "| - "
        else:
            sym = "+ "
            symc = "  - "

        fout.write("%s%s[%s]\n" % (indent, sym, k))
        for e in v.content:
            #fout.write("%s%s%-30s %-30s %s\n" % (indent, symc, e.driver, e.team, e.filename))
            fout.write("%s%s%-30s %-30s\n" % (indent, symc, e.driver, e.team))
        _print_tree_rec(v, indent + "  ", fout)


def print_veh_tree(vehs, fout):
    tree = Tree()
    for veh in vehs:
        subtree = tree
        for cat in veh.category:
            subtree = subtree[cat]
        subtree.content.append(veh)

    _print_tree_rec(tree, "", fout)


def print_veh_info(vehs, fout):
    for veh in vehs:
        fout.write("    file: %s\n" % veh.filename)
        fout.write(" classes: %s\n" % veh.classes)
        fout.write("graphics: %s\n" % veh.graphics_file)
        fout.write("category: %s\n" % veh.category)
        fout.write("    team: %s\n" % veh.team)
        fout.write("\n")


# EOF #
