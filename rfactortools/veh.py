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


def find_file_backwards(vfs, dir, gen):
    while True:
        filename = os.path.join(dir, gen)
        if vfs.file_exists(filename):
            return filename

        newdir = os.path.dirname(dir)
        if newdir == dir:  # reached the root of the path
            return None
        else:
            dir = newdir


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

    with open(filename, 'rt', encoding='latin-1') as fin:
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


def process_scn_veh_file(vfs, modname, veh_filename, scn_short_filename, vehdir, teamdir, fix, errors):
    # resolve scn_filename to a proper path
    scn_filename = find_file_backwards(vfs, os.path.dirname(veh_filename), scn_short_filename)
    if not scn_filename:
        raise Exception("error: couldn't find .gen file '%s' '%s'" % (veh_filename, scn_short_filename))

    print("gen:", scn_filename)

    info = rfactortools.InfoScnParser()
    rfactortools.process_scnfile(vfs, scn_filename, info)

    print("  SearchPath:", info.search_path)
    print("    MasFiles:", info.mas_files)
    print()

    orig_errs, orig_warns = rfactortools.gen_check_errors(vfs, info.search_path, info.mas_files, vehdir, teamdir)

    if not fix:
        append_errors(scn_filename, orig_errs, orig_warns, errors)
    else:
        # if there is a cmaps in the mod, use that instead of the one in <VEHDIR>
        cmaps = vfs.find_file("cmaps.mas")
        if cmaps:
            cmaps = os.path.relpath(cmaps, vehdir)

        mas_files = []
        for m in info.mas_files:
            if cmaps and m.lower() == "cmaps.mas":
                mas_files.append(cmaps)
            else:
                mas_files.append(m)

        if orig_errs:
            # add modname to the SearchPath to avoid errors
            search_path = [re.sub(r'<VEHDIR>', r'<VEHDIR>/%s' % modname, p) for p in info.search_path]
            search_path.insert(0, "<VEHDIR>")
        else:
            search_path = info.search_path
        search_path.sort()

        new_errs, new_warns = rfactortools.gen_check_errors(vfs, search_path, mas_files, vehdir, teamdir)

        append_errors(scn_filename, new_errs, new_warns, errors)

        # write a new file if there are no or less errors or if there
        # is a cmaps.mas
        if not new_errs or len(new_errs) < len(orig_errs) or cmaps:
            rfactortools.modify_vehicle_file(vfs, scn_filename, search_path, mas_files, vehdir, teamdir)


def process_veh_file(vfs, veh_filename, fix, errors):
    teamdir = os.path.dirname(veh_filename)
    modname = find_modname(os.path.dirname(veh_filename))

    vehdir = find_vehdir(os.path.dirname(veh_filename))
    veh_obj = parse_vehfile(vfs.lookup_file(veh_filename))

    print("[Vehicle]")
    print("veh:", veh_filename)
    print("    <VEHDIR>:", vehdir)
    print("   <TEAMDIR>:", teamdir)
    print("    graphics:", veh_obj.graphics_file)
    print("     spinner:", veh_obj.spinner_file)

    if veh_obj.graphics_file is not None:
        process_scn_veh_file(vfs, modname, veh_filename, veh_obj.graphics_file, vehdir, teamdir, fix, errors)

    if veh_obj.spinner_file is not None:
        process_scn_veh_file(vfs, modname, veh_filename, veh_obj.spinner_file, vehdir, teamdir, fix, errors)


class Tree(defaultdict):

    """A recursize defaultdict()"""

    def __init__(self, value=None):
        super(Tree, self).__init__(Tree)
        self.value = value
        self.content = []


def _print_tree_rec(tree, indent=""):
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

        print("%s%s[%s]" % (indent, sym, k))
        for e in v.content:
            #print("%s%s%-30s %-30s %s" % (indent, symc, e.driver, e.team, e.filename))
            print("%s%s%-30s %-30s" % (indent, symc, e.driver, e.team))
        _print_tree_rec(v, indent + "  ")


def print_veh_tree(vehs):
    tree = Tree()
    for veh in vehs:
        subtree = tree
        for cat in veh.category:
            subtree = subtree[cat]
        subtree.content.append(veh)

    _print_tree_rec(tree)


def print_veh_info(vehs):
        for veh in vehs:
            print("    file:", veh.filename)
            print(" classes:", veh.classes)
            print("graphics:", veh.graphics_file)
            print("category:", veh.category)
            print("    team:", veh.team)
            print()


# EOF #
