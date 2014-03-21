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


import logging
import os
import pathlib
import re
import shutil

import rfactortools


exclude_files = [
    "locations/commonmaps.mas",
    "locations/terrain.tdf",
    "scripts/pits.mas",
    "shared/coreshaders.mas",
    "vehicles/rfhud.mas",
    "vehicles/cmaps.mas",
    "vehicles/damage.ini",
    "vehicles/default.cam",
    "vehicles/default.sfx",
    "vehicles/doublewishbone.pm",
    "vehicles/multicar.mas",
    "vehicles/multiplayervehicle.scn",
    "vehicles/night_lightfield.tga",
    "vehicles/f3_ambientshadow.dds",
    "vehicles/headphysics.ini",
    "vehicles/showroom.mas",
    "vehicles/spark.tga",
    "vehicles/vehicle_commonmaps.mas",
    "vehicles/vview.scn",
]
exclude_files = [os.path.normpath(f) for f in exclude_files]


def prefix_split(lhs, rhs):
    length = min(len(lhs), len(rhs))
    for i in range(0, length):
        if lhs[i].lower() != rhs[i].lower():
            return lhs[0:i], lhs[i:], rhs[i:]
    return lhs[0:length], lhs[length:], rhs[length:]


def find_track_directory_from_searchpath(directory_str, search_path_str, depth=0):
    """
    Given the directory of the ``.gdb`` file and a SearchPath, try to
    locate the root directory of a track.

    Returns a tuple of (root, prefix), prefix is either ``None`` or
    set such the tracks will function when installed into
    GameData/Locations/{prefix}/ (aka. the ``70tracks/`` problem).
    """

    directory = list(reversed(pathlib.Path(directory_str).parts))
    length = 0
    max_path = None
    for p in search_path_str:
        path = list(reversed(pathlib.Path(p).parts))
        if length < len(path) and p != ".":
            length = len(path)
            max_path = path

    if not max_path:
        raise Exception("empty SearchPath")
    else:
        prefix, lhs, rhs = prefix_split(max_path, directory)

        if prefix:
            if lhs:
                rest = str(pathlib.Path(*reversed(lhs)))
            else:
                rest = None
            return (str(pathlib.Path(*reversed([prefix[-1]] + rhs))),
                    rest)
        elif depth < 2:
            return find_track_directory_from_searchpath(os.path.dirname(directory_str), search_path_str,
                                                        depth=depth+1)
        else:
            raise Exception("no common prefix")


def find_track_directory(gdb_filename):
    rest, ext = os.path.splitext(gdb_filename)
    scn_filename = rest + ".scn"
    scn_filename = rfactortools.lookup_path_icase(scn_filename)
    if not scn_filename:
        raise Exception("couldn't locate .scn file matching %s" % gdb_filename)
    else:
        info = rfactortools.InfoScnParser()
        rfactortools.process_scnfile(scn_filename, info)

        result = find_track_directory_from_searchpath(os.path.dirname(gdb_filename),
                                                      info.search_path)
        return result


def find_data_directories(directory):
    """Returns the ``GameData/`` directory inside of ``directory``, throws
    exception when more then one ``GamaData/`` is found, return
    ``None``, if none is found (not an error, as tracks don't contain
    a ``GameData/``)
    """

    basedir = os.path.basename(directory)
    if basedir.lower() == "gamedata":
        return [directory]
    else:
        gamedata_dirs = []
        track_dirs = []
        for path, dirs, files in os.walk(directory):
            for d in list(dirs):
                if d.lower() == "gamedata":
                    gamedata_dirs.append(os.path.join(path, d))
                    dirs.remove(d)

            for f in files:
                rest, ext = os.path.splitext(f)
                if ext.lower() == ".gdb":
                    try:
                        track_dirs.append(find_track_directory(os.path.join(path, f)))
                    except Exception:
                        logging.exception("track directory location failed")

        return gamedata_dirs, track_dirs


class rFactorToGSC2013Config:

    def __init__(self):
        self.unique_team_names = True
        self.force_track_thumbnails = True
        self.clear_classes = False
        self.single_gamadata = True
        self.reiza_class = "reiza5"
        self.vehicle_category = ""
        self.track_category = ""
        self.track_filter_properties = "StockV8 *"


class rFactorToGSC2013:

    """
    Converter for rFactor vehicles and tracks to Game Stock Car 2013
    """

    def __init__(self, source_directory, cfg):
        self.source_directory = os.path.normpath(source_directory)
        self.cfg = cfg or rFactorToGSC2013Config()
        self.source_gamedata_directories, self.source_track_directories \
            = find_data_directories(self.source_directory)

        if not self.source_gamedata_directories and not self.source_track_directories:
            raise Exception("couldn't locate 'GameData/' directory")

    def print_info(self):
        vehicle_count = len(self.files_by_type['.veh'])
        track_count = len(self.files_by_type['.gdb'])
        print("Vehicles: %d" % vehicle_count)
        print("  Tracks: %d" % track_count)
        print("GameData: \"%s\"" % self.source_gamedata_directories)

    def convert_gdb(self, filename, target_file):
        with rfactortools.open_read(filename) as fin:
            lines = fin.readlines()

        with open(target_file, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
            for line in lines:
                line = re.sub(r'Filter Properties *=.*',
                              r'Filter Properties = %s' % self.cfg.track_filter_properties,
                              line, flags=re.IGNORECASE)

                if self.cfg.track_category:
                    line = re.sub(r'^(\s*)VenueName\s*=\s*(.*)',
                                  r'\1VenueName = %s, \2' % self.cfg.track_category,
                                  line, flags=re.IGNORECASE)

                fout.write(line)

    def convert_sfx(self, source_file, target_file, modname):
        with open(target_file, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
            rfactortools.modify_sfxfile(fout, source_file,
                                        lambda wav:
                                        rfactortools.try_fix_wav_path(self.source_gamedata_directory,
                                                                      modname, wav))

    def convert_aiw(self, source_file, target_file):
        shutil.copy(source_file, target_file)

        # generate the thumbnail if there isn't somebody already
        rest, ext = os.path.splitext(source_file)
        trest, text = os.path.splitext(target_file)
        source_mini_file = os.path.join(rest + "mini.tga")
        target_mini_file = os.path.join(trest + "mini.tga")

        logging.info("generating track thumbnail: %s" % target_mini_file)
        if not rfactortools.lookup_path_icase(source_mini_file) or self.cfg.force_track_thumbnails:
            aiw = rfactortools.parse_aiwfile(source_file)
            img = rfactortools.render_aiw(aiw, 252, 249)
            img.save(target_mini_file)

    def convert_veh(self, source_file, target_file, mod_name):
        with rfactortools.open_read(source_file) as fin:
            lines = fin.readlines()

        if self.cfg.unique_team_names:
            team_suffix = " %s" % mod_name
        else:
            team_suffix = ""

        with open(target_file, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
            for line in lines:
                # reiza5 (Mini Challenge) is needed for the cars to
                # show up in the car list
                if self.cfg.clear_classes:
                    line = re.sub(r'^Classes=".*',
                                  r'Classes="%s"' % self.cfg.reiza_class,
                                  line, flags=re.IGNORECASE)
                else:
                    line = re.sub(r'^Classes="',
                                  r'Classes="%s, ' % self.cfg.reiza_class,
                                  line, flags=re.IGNORECASE)

                # Adding a suffix to Team is needed as conflicts in
                # Team names lead to cars getting sorted into the
                # wrong hierachy (e.g. a F1 car would show up in the
                # wrong year of F1, as multiple F1 years share the
                # same team names)
                line = re.sub(r'^Team="([^"]*)"',
                              r'Team="\1%s"' % team_suffix,
                              line, flags=re.IGNORECASE)

                if self.cfg.vehicle_category:
                    line = re.sub(r'^Category="([^"]*)"',
                                  r'Category="%s, \1"' % self.cfg.vehicle_category,
                                  line, flags=re.IGNORECASE)

                fout.write(line)

    def convert_gmt(self, source_file, target_file):
        rfactortools.encrypt_file(source_file, target_file)

    def convert_mas(self, source_file, target_file):
        logging.info("mas unpacking %s" % source_file)
        mas_content = rfactortools.mas_unpack_to_data(source_file)

        logging.info("encrypting files")
        encrypted_mas_content = []
        for i, (name, data) in enumerate(mas_content):
            logging.debug("processing %d/%d: %s" % (i, len(mas_content), name))
            encrypted_data = rfactortools.encrypt_data(data, 1, 0x4b1dca9f960524e8, rfactortools.get_skip(name))
            encrypted_mas_content.append((name, encrypted_data))

        rfactortools.mas_pack_from_data(encrypted_mas_content, target_file)

    def convert_tdf(self, source_file, target_file):
        shutil.copy(source_file, target_file)

        # TODO: insert check if additional textures are needed
        # shutil.copy("gsc2013/RACEGROOVE.dds", os.path.dirname(target_file))
        # shutil.copy("gsc2013/SKIDHARD.dds", os.path.dirname(target_file))

    def copy_directory_hierachy(self, source_directory, target_directory):
        """Recreates the directory hierachy in ``source_directory`` in target_directory"""
        if not os.path.isdir(target_directory):
            os.makedirs(os.path.normpath(target_directory))

        for path, dirs, files in os.walk(source_directory):
            relpath = os.path.relpath(path, source_directory)

            for d in dirs:
                t = os.path.join(target_directory, relpath, d)
                logging.info("creating %s" % t)
                if not os.path.isdir(t):
                    os.mkdir(t)

    def convert_tga(self, source_file, target_file):
        is_vehicle_thumbnail = bool(source_file.lower().endswith("number.tga") and
                                    rfactortools.file_exists(source_file[:-10] + ".veh"))

        is_track_thumbnail = bool(source_file.lower().endswith("mini.tga") and
                                  rfactortools.file_exists(source_file[:-8] + ".gdb"))

        if is_vehicle_thumbnail:
            rfactortools.resize_to_fit_img_file_with_target(source_file, target_file, 252, 64)
        elif is_track_thumbnail:
            if not self.cfg.force_track_thumbnails:
                rfactortools.resize_to_fit_img_file_with_target(source_file, target_file, 252, 249)
        else:
            shutil.copy(source_file, target_file)

    def convert_gamedata(self, source_directory, target_directory):
        for fname in os.listdir(source_directory):
            path = os.path.join(source_directory, fname)

            if os.path.isdir(path):
                self.convert_toplevel_subdir(source_directory, target_directory, fname)
            elif os.path.isfile(path):
                self.convert_file(source_directory, target_directory, fname)
            else:
                logging.error("%s: ignoring unknown file" % path)

    def convert_toplevel_subdir(self, source_directory, target_directory, dname):
        """Convert ``Vehicles``, ``Locations``, etc."""

        source = os.path.join(source_directory, dname)

        for fname in os.listdir(source):
            path = os.path.join(source, fname)

            if os.path.isdir(path):
                self.convert_mod_subdir(source_directory, target_directory, os.path.join(dname, fname), fname)
            elif os.path.isfile(path):
                self.convert_file(source_directory, target_directory, os.path.join(dname, fname), None)
            else:
                logging.error("%s: ignoring unknown file" % path)

    def convert_mod_subdir(self, source_directory, target_directory, dname, modname):
        """Convert ``Vehicles/some_mod/``, ``Locations/some_mod``, etc."""

        source = os.path.join(source_directory, dname)

        for path, dirs, files in os.walk(source):
            relpath = os.path.relpath(path, source)

            for fname in files:
                self.convert_file(source_directory, target_directory,
                                  os.path.normpath(os.path.join(dname, relpath, fname)), modname)

    def convert_file(self, source_directory, target_directory, filename, modname):
        logging.info("processing '%s' of mod '%s'" % (filename, modname))

        if filename.lower() in exclude_files:
            pass
        else:
            source_file = os.path.join(source_directory, filename)
            target_file = os.path.join(target_directory, filename)

            try:
                ext = os.path.splitext(filename)[1].lower()
                if ext == ".gdb":
                    self.convert_gdb(source_file, target_file)
                elif ext == ".veh":
                    self.convert_veh(source_file, target_file, modname)
                elif ext == ".aiw":
                    self.convert_aiw(source_file, target_file)
                elif ext == ".gmt":
                    self.convert_gmt(source_file, target_file)
                elif ext == ".tdf":
                    self.convert_tdf(source_file, target_file)
                elif ext == ".mas":
                    self.convert_mas(source_file, target_file)
                elif ext == ".sfx":
                    self.convert_sfx(source_file, target_file, modname)
                elif ext == ".tga":
                    self.convert_tga(source_file, target_file)
                else:
                    shutil.copy(source_file, target_file)

            except Exception:
                logging.exception("%s: %s: rfactortools.convert_file failed" % (source_file, target_file))

    def convert_all(self, target_directory):
        target_directory = os.path.normpath(target_directory)

        # convert GamaData/ directories
        for d in self.source_gamedata_directories:
            self.source_gamedata_directory = os.path.normpath(d)

            if self.cfg.single_gamadata:
                target_gamedata_directory = os.path.join(target_directory, "GameData")
            else:
                target_gamedata_directory = os.path.join(target_directory,
                                                         os.path.relpath(self.source_gamedata_directory,
                                                                         self.source_directory))

            logging.info("converting GameData %s to %s" % (self.source_gamedata_directory, target_gamedata_directory))

            self.copy_directory_hierachy(self.source_gamedata_directory, target_gamedata_directory)
            self.convert_gamedata(self.source_gamedata_directory, target_gamedata_directory)

            try:
                rfactortools.process_gen_directory(target_gamedata_directory, True)
            except Exception:
                logging.exception("rfactortools.process_gen_directory")

        # convert tracks that don't have a toplevel GameData/ directory
        for d, prefix in self.source_track_directories:
            logging.debug("track: prefix:\"%s\" - directory:\"%s\"" % (prefix, d))
            source_directory = os.path.normpath(d)

            if self.cfg.single_gamadata:
                target_gamedata_directory = os.path.join(target_directory, "GameData")
            else:
                target_gamedata_directory = os.path.join(target_directory,
                                                         os.path.relpath(source_directory,
                                                                         self.source_directory),
                                                         "GameData")

            logging.info("converting track %s to %s" % (source_directory, target_gamedata_directory))
            # create directory hierachy
            modname = os.path.basename(d)
            if prefix:
                self.copy_directory_hierachy(source_directory,
                                             os.path.join(target_gamedata_directory, "Locations", prefix, modname))
            else:
                self.copy_directory_hierachy(source_directory,
                                             os.path.join(target_gamedata_directory, "Locations", modname))

            logging.debug("modname: %s" % modname)
            if prefix:
                target_d = os.path.join(target_gamedata_directory, "Locations", prefix)
            else:
                target_d = os.path.join(target_gamedata_directory, "Locations")
            logging.debug("track: source_directory: %s" % source_directory)
            logging.debug("track: target_directory: %s" % target_d)
            self.convert_mod_subdir(os.path.dirname(source_directory), target_d, modname, modname)


# EOF #
