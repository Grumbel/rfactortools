# rFactor .sfx file manipulation tool
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
import ntpath
import os
import re

import rfactortools


comment_regex = re.compile(r'(.*?)(\s+//.*)')
wav1_regex = re.compile(r'^\s*([^=]+)\s*=\s*([0-9]+\.[0-9]+|[0-9]+),(.*\.wav)\s*(.*)', re.IGNORECASE)
wav2_regex = re.compile(r'^\s*([^=]+)\s*=\s*(.*\.wav)\s*(.*)', re.IGNORECASE)


class SFX:

    def __init__(self):
        self.wavs = []

    def check(self, gamedata_directory, modname):
        for wav in self.wavs:
            p = os.path.join(gamedata_directory, "Sounds", wav)
            if rfactortools.file_exists(p):
                print("%s: ok" % wav)
            else:
                p = os.path.join(gamedata_directory, "Sounds", modname, wav)
                if rfactortools.file_exists(p):
                    print("%s: ok with fix" % wav)
                else:
                    print("%s: failure" % wav)


def try_fix_wav_path(gamedata, modname, wav_file):
    """Return either ``None`` to change nothing or a new wav path"""
    p = os.path.join(gamedata, "Sounds", wav_file)
    if rfactortools.file_exists(p):
        logging.debug("%s: file ok", wav_file)
        return None
    else:
        p = os.path.join(gamedata, "Sounds", modname, wav_file)
        if rfactortools.file_exists(p):
            r = os.path.join(modname, wav_file)
            logging.debug("%s: file ok", r)
            return r
        else:
            logging.error("%s: couldn't locate file", wav_file)
            return None


def modify_sfxfile(fout, filename, on_wav_file):
    with rfactortools.open_read(filename) as fin:
        lines = fin.read().splitlines()

    for orig_line in lines:
        m = comment_regex.match(orig_line)
        if m:
            comment = m.group(2)
            line = m.group(1)
        else:
            comment = ""
            line = orig_line

        suffix = comment

        m = wav1_regex.match(line)
        if m:
            wav = on_wav_file(rfactortools.nt2posixpath(m.group(3)))
            if wav is not None:
                wav = ntpath.normpath(wav)
            else:
                wav = m.group(3)
            fout.write("%s=%s,%s%s\n" % (m.group(1), m.group(2), wav, suffix))
        else:
            m = wav2_regex.match(line)
            if m:
                wav = on_wav_file(rfactortools.nt2posixpath(m.group(2)))
                if wav is not None:
                    wav = ntpath.normpath(wav)
                else:
                    wav = m.group(2)
                fout.write("%s=%s%s\n" % (m.group(1), wav, suffix))
            else:
                fout.write(orig_line)
                fout.write("\n")


def parse_sfxfile(filename):
    sfx = SFX()

    with rfactortools.open_read(filename) as fin:
        for orig_line in fin.read().splitlines():
            m = comment_regex.match(orig_line)
            if m:
                # comment = m.group(2)
                line = m.group(1)
            else:
                # comment = None
                line = orig_line

            m = wav1_regex.match(line)
            if m:
                sfx.wavs.append(rfactortools.nt2posixpath(m.group(3)))
            else:
                m = wav2_regex.match(line)
                if m:
                    sfx.wavs.append(rfactortools.nt2posixpath(m.group(2)))
    return sfx


# EOF #
