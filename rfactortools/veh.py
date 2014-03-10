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
import ntpath
import os
import posixpath
import re

import rfactortools

keyvalue_regex = re.compile(r'^\s*([^=]+)\s*=\s*(.*)\s*')
comment_regex = re.compile(r'(.*?)(//.*)')

quoted_string_regex = re.compile(r'"(.*)"')


def nt2os_path(path):
    return path.replace(ntpath.sep, os.path.sep)


def find_modname(path):
    m = re.match(r'^.*GameData/Vehicles/([^.*/]*)', path, re.IGNORECASE)
    if m:
        return m.group(1)
    else:
        raise Exception("couldn't locate modname")


def find_vehdir(path):
    m = re.match(r'^(.*GameData/Vehicles)', path, re.IGNORECASE)
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
                comment = m.group(2)
                line = m.group(1)
            else:
                comment = None

            m = keyvalue_regex.match(line)
            if m:
                key, value = m.group(1), m.group(2)
                if key.lower() == "graphics":
                    veh.graphics_file = nt2os_path(value.strip())
                elif key.lower() == "spinner":
                    veh.spinner_file = nt2os_path(value.strip())
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


def process_veh_file(vfs, veh_filename, fix, errors):
    teamdir = os.path.dirname(veh_filename)
    modname = find_modname(os.path.dirname(veh_filename))
    vehdir = find_vehdir(os.path.dirname(veh_filename))

    veh_obj = parse_vehfile(vfs.lookup_file(veh_filename))
    gen_name = veh_obj.graphics_file
    gen_filename = find_file_backwards(vfs, os.path.dirname(veh_filename), gen_name)
    if not gen_filename:
        raise Exception("%s: error: couldn't find .gen file '%s'" % (veh_filename, gen_name))
    print("[Vehicle]")
    print("  veh:", veh_filename)
    print("  gen:", gen_filename)
    info = rfactortools.InfoScnParser()
    rfactortools.process_scnfile(vfs, gen_filename, info)
    print("    <VEHDIR>:", vehdir)
    print("   <TEAMDIR>:", teamdir)
    print("  SearchPath:", info.search_path)
    print("    MasFiles:", info.mas_files)

    errs, warns = rfactortools.gen_check_errors(vfs, info.search_path, info.mas_files, vehdir, teamdir)
    for err in errs:
        errors.append("%s: %s" % (gen_filename, err))
    for warn in warns:
        errors.append("%s: %s" % (gen_filename, warn))
    print()

    if errs and fix:
        cmaps = vfs.find_file("cmaps.mas")
        print("XXX", vehdir)
        print("XXX", cmaps)
        if cmaps:
            cmaps = posixpath.relpath(cmaps, vehdir)

        # add modname to the search path
        search_path = [re.sub(r'<VEHDIR>', r'<VEHDIR>/%s' % modname, p) for p in info.search_path]
        search_path.insert(0, "<VEHDIR>")
        
        # make cmaps unique
        # TODO: do this even if there are no obvious errors
        mas_files = []
        for m in info.mas_files:
            if m.lower() == "cmaps.mas":
                mas_files.append(cmaps)
            else:
                mas_files.append(m)

        errs, warns = rfactortools.gen_check_errors(vfs, search_path, mas_files, vehdir, teamdir)
        if not errs:
            rfactortools.modify_vehicle_file(vfs, gen_filename, search_path, mas_files, vehdir, teamdir)
        else:
            errors.append("-- Automatic fixing failed:")
            for err in errs:
                errors.append("%s: %s" % (gen_filename, err))
            for warn in warns:
                errors.append("%s: %s" % (gen_filename, warn))


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
