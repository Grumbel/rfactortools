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
import logging
import os
import re
import shutil

import imgtool
import rfactortools


def find_gamedata_directory(directory):
    """Returns the ``GameData/`` directory inside of ``directory``, throws
    exception when more then one ``GamaData/`` is found, return
    ``None``, if none is found (not an error, as tracks don't contain
    a ``GameData/``)
    """

    basedir = os.path.basename(directory)
    if basedir.lower() == "gamedata":
        return directory
    else:
        gamedata = None
        for path, dirs, files in os.walk(directory):
            for d in dirs:
                if d.lower() == "gamedata":
                    if gamedata is not None:
                        raise Exception("multiple \"GamaData/\" directories found in \"%s\", only one allowed"
                                        % directory)
                    else:
                        gamedata = os.path.join(path, d)
        return gamedata


class rFactorToGSC2013Config:

    def __init__(self):
        self.unique_team_names = True
        self.force_track_thumbnails = False
        self.reiza_class = "reiza5"
        self.vehicle_category = None
        self.track_category = None


class rFactorToGSC2013:

    """
    Converter for rFactor vehicles and tracks to Game Stock Car 2013
    """

    def __init__(self, source_directory, cfg):
        self.source_directory = os.path.normpath(source_directory)
        self.cfg = cfg or rFactorToGSC2013Config()
        self.source_gamedata_directory = find_gamedata_directory(self.source_directory)

        if self.source_gamedata_directory is None:
            raise Exception("couldn't locate 'GameData/' directory")
        else:
            # gather files and directories, dir_tree is relative to ``gamedata_directory``
            self.dir_tree = []
            self.files_by_type = defaultdict(list)

            for path, dirs, files in os.walk(self.source_gamedata_directory):
                relpath = os.path.relpath(path, self.source_gamedata_directory)

                for d in dirs:
                    self.dir_tree.append(os.path.normpath(os.path.join(relpath, d)))

                for fname in files:
                    filename = os.path.normpath(os.path.join(relpath, fname))

                    ext = os.path.splitext(filename)[1].lower()
                    self.files_by_type[ext].append(filename)

            self.dir_tree.sort()
            for k, v in self.files_by_type.items():
                v.sort()

    def print_info(self):
        vehicle_count = len(self.files_by_type['.veh'])
        track_count = len(self.files_by_type['.gdb'])
        print("Vehicles: %d" % vehicle_count)
        print("  Tracks: %d" % track_count)
        print("GameData: \"%s\"" % self.source_gamedata_directory)

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

        logging.info("generating track thumbnail: %s" % target_mini_file)
        if not rfactortools.lookup_path_icase(source_mini_file) or self.cfg.force_track_thumbnails:
            aiw = rfactortools.parse_aiwfile(source_file)
            img = rfactortools.render_aiw(aiw, 252, 249)
            img.save(target_mini_file)

    def convert_veh(self, source_file, target_file, mod_name):
        with open(source_file, "rt", encoding="latin-1") as fin:
            lines = fin.readlines()

        if self.cfg.unique_team_names:
            team_suffix = " %s" % mod_name
        else:
            team_suffix = ""

        with open(target_file, "wt", newline='\r\n', encoding="latin-1", errors="replace") as fout:
            for line in lines:
                # reiza5 (Mini Challenge) is needed for the cars to
                # show up in the car list
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

    def convert_all(self, target_directory):
        target_gamedata_directory = os.path.join(os.path.normpath(target_directory), "GameData")
        logging.info("converting %s to %s" % (self.source_gamedata_directory, target_gamedata_directory))

        # create target directory hierachy
        if not os.path.isdir(target_gamedata_directory):
            os.makedirs(os.path.normpath(target_gamedata_directory))
        for d in self.dir_tree:
            t = os.path.join(target_gamedata_directory, d)
            logging.info("creating %s" % t)
            if not os.path.isdir(t):
                os.mkdir(t)

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

        # convert and copy files
        for ext, files in self.files_by_type.items():
            # ``files`` are in os.path.normpath() syntax and relative to GamaData/
            for i, filename in enumerate(files):
                if filename.lower() in exclude_files:
                    pass
                else:
                    source_file = os.path.join(self.source_gamedata_directory, filename)
                    target_file = os.path.join(target_gamedata_directory, filename)

                    logging.info("processing '%s' file %d/%d: %s" % (ext, i + 1, len(files), filename))
                    try:
                        if ext == ".gdb":
                            self.convert_gdb(source_file, target_file)
                        elif ext == ".veh":
                            self.convert_veh(source_file, target_file, "placeholder")
                        elif ext == ".aiw":
                            self.convert_aiw(source_file, target_file)
                        elif ext == ".gmt":
                            self.convert_gmt(source_file, target_file)
                        elif ext == ".tdf":
                            self.convert_tdf(source_file, target_file)
                        elif ext == ".mas":
                            self.convert_mas(source_file, target_file)
                        else:
                            shutil.copy(source_file, target_file)
                    except Exception:
                        logging.exception("rfactortools.process_gen_directory")

        try:
            rfactortools.process_gen_directory(target_gamedata_directory, True)
        except Exception:
            logging.exception("rfactortools.process_gen_directory")

        try:
            imgtool.process_directory(target_gamedata_directory)
        except Exception:
            logging.exception("imgtool error")


# EOF #
