# rFactor to GSC2013 converter
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
import re
import shutil
import sys
import PIL.Image
import random
import posixpath

import imgtool
import rfactortools


def find_modname(path):
    # TODO: do a better scan for a usable modname, not all
    # tracks and vehicles have one which makes this tricky, random
    # string will be enough for the moment to avoid conflicts

    path = path.replace(os.path.sep, posixpath.sep)

    m = re.match(r'^.*/Vehicles/([^/]+)', path, re.IGNORECASE)
    if m:
        return m.group(1)
    else:
        return "RND%d" % random.randint(0, 999)


class rFactorToGSC2013:
    """
    Converter for rFactor vehicles and tracks to Game Stock Car 2013
    """

    def __init__(self, source_directory):
        self.source_directory = os.path.normpath(source_directory)

        self.mod_name = find_modname(source_directory)

        # gather files and directories, dir_tree is relative to source_directory
        self.dir_tree = []
        self.files_by_type = defaultdict(list)

        for path, dirs, files in os.walk(self.source_directory):
            relpath = os.path.relpath(path, self.source_directory)

            for d in dirs:
                self.dir_tree.append(os.path.normpath(os.path.join(relpath, d)))

            for fname in files:
                filename = os.path.normpath(os.path.join(relpath, fname))

                ext = os.path.splitext(filename)[1].lower()
                self.files_by_type[ext].append(filename)

        self.dir_tree.sort()

        self._find_gamedata_directory()

        self.unique_team_names = True
        self.force_track_thumbnails = False

    def _find_gamedata_directory(self):
        self.gamedata = []
        basedir = os.path.basename(self.source_directory)
        if basedir.lower() == "gamedata":
            self.gamedata.append(self.source_directory)
        else:
            for d in self.dir_tree:
                tail, head = os.path.split(d)
                if head.lower() == "gamedata":
                    self.gamedata.append(d)
                elif head.lower() == "vehicles" or head.lower() == "locations":
                    self.gamedata.append(tail)

        # remove duplicate directories
        self.gamedata = list(set(self.gamedata))        

    def print_info(self):
        vehicle_count = len(self.files_by_type['.veh'])
        track_count = len(self.files_by_type['.gdb'])
        print("Vehicles: %d" % vehicle_count)
        print("  Tracks: %d" % track_count)

        for d in self.gamedata:
            print("GameData: \"%s\"" % d)

    def convert_gdb(self, filename, target_file):
        with open(filename, "rt", encoding="latin-1") as fin:
            lines = fin.readlines()

        with open(target_file, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
            for line in lines:
                line = re.sub(r'Filter Properties *=.*',
                              r'Filter Properties = StockV8 *',
                              line, flags=re.IGNORECASE)
                fout.write(line)

    def convert_aiw(self, source_file, target_file):
        shutil.copy(source_file, target_file)

        # generate the thumbnail if there isn't somebody already
        rest, ext = os.path.splitext(source_file)
        trest, text = os.path.splitext(target_file)
        source_mini_file = os.path.join(rest + "mini.tga")
        target_mini_file = os.path.join(trest + "mini.tga")

        print("generating track thumbnail: %s" % target_mini_file)
        if not rfactortools.lookup_path_icase(source_mini_file) or self.force_track_thumbnails:
            aiw = rfactortools.parse_aiwfile(source_file)
            img = rfactortools.render_aiw(aiw, 252, 249)
            img.save(target_mini_file)

    def convert_veh(self, source_file, target_file):
        with open(source_file, "rt", encoding="latin-1") as fin:
            lines = fin.readlines()

        if self.unique_team_names:
            team_suffix = " %s" % self.mod_name
        else:
            team_suffix = ""

        with open(target_file, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
            for line in lines:
                # reiza5 (Mini Challenge) is needed for the cars to
                # show up in the car list
                line = re.sub(r'^Classes="',
                              r'Classes="reiza5, ',
                              line, flags=re.IGNORECASE)

                # Adding a suffix to Team is needed as conflicts in
                # Team names lead to cars getting sorted into the
                # wrong hierachy (e.g. a F1 car would show up in the
                # wrong year of F1, as multiple F1 years share the
                # same team names)
                line = re.sub(r'^Team="([^"]*)"',
                              r'Team="\1%s"' % team_suffix,
                              line, flags=re.IGNORECASE)

                fout.write(line)

    def convert_gmt(self, source_file, target_file):
        rfactortools.encrypt_file(source_file, target_file)

    def convert_mas(self, source_file, target_file):
        print("mas unpacking %s" % source_file)
        mas_content = rfactortools.mas_unpack_to_data(source_file)

        print("encrypting files")
        encrypted_mas_content = []
        for i, (name, data) in enumerate(mas_content):
            print("processing %d/%d: %s" % (i, len(mas_content), name))
            encrypted_data = rfactortools.encrypt_data(data, 1, 0x4b1dca9f960524e8, rfactortools.get_skip(name))
            encrypted_mas_content.append((name, encrypted_data))

        rfactortools.mas_pack_from_data(encrypted_mas_content, target_file)

    def convert_tdf(self, source_file, target_file):
        shutil.copy(source_file, target_file)

        # TODO: insert check if additional textures are needed
        # shutil.copy("gsc2013/RACEGROOVE.dds", os.path.dirname(target_file))
        # shutil.copy("gsc2013/SKIDHARD.dds", os.path.dirname(target_file))

    def convert_all(self, target_directory):
        target_directory = os.path.normpath(target_directory)
        print("Converting %s to %s" % (self.source_directory, target_directory))

        # create target directory hierachy
        if not os.path.isdir(target_directory):
            os.makedirs(os.path.normpath(target_directory))
        for d in self.dir_tree:
            t = os.path.join(target_directory, d)
            print("creating %s" % t)
            if not os.path.isdir(t):
                os.mkdir(t)

        # convert and copy files
        for ext, files in self.files_by_type.items():
            for i, filename in enumerate(files):
                source_file = os.path.join(self.source_directory, filename)
                target_file = os.path.join(target_directory, filename)

                print("Processing '%s' file %d/%d: %s" % (ext, i + 1, len(files), filename))
                if ext == ".gdb":
                    self.convert_gdb(source_file, target_file)
                elif ext == ".veh":
                    self.convert_veh(source_file, target_file)
                elif ext == ".aiw":
                    self.convert_aiw(source_file, target_file)
                elif ext == ".gmt":
                    self.convert_gmt(source_file, target_file)
                elif ext == ".tdf":
                    self.convert_tdf(source_file, target_file)
                elif ext == ".mas":
                    if filename.lower().endswith("shared" + os.path.sep + "coreshaders.mas"):
                        pass
                    else:
                        self.convert_mas(source_file, target_file)
                else:
                    shutil.copy(source_file, target_file)

        imgtool.process_directory(target_directory)


# EOF #
