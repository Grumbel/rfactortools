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


import re
import rfactortools


comment_regex = re.compile(r'(.*?)(//.*)')
wav1_regex = re.compile(r'^\s*([^=]+)\s*=\s*([0-9]+\.[0-9]+|[0-9]+),(.*\.wav)\s*(.*)', re.IGNORECASE)
wav2_regex = re.compile(r'^\s*([^=]+)\s*=\s*(.*\.wav)\s*(.*)', re.IGNORECASE)


def modify_sfxfile(fout, filename, prefix):
    with open(filename, 'rt', encoding='latin-1') as fin:
        for orig_line in fin.read().splitlines():
            m = comment_regex.match(orig_line)
            if m:
                comment = m.group(2)
                line = m.group(1)
            else:
                comment = None
                line = orig_line

            suffix = (" " + comment) if comment else ""

            m = wav1_regex.match(line)
            if m:
                fout.write("%s=%s,%s%s%s\n" % (m.group(1), prefix, m.group(2), m.group(3), suffix))
            else: 
                m = wav2_regex.match(line)
                if m:
                    fout.write("%s=%s%s%s\n" % (m.group(1), prefix, m.group(2), suffix))
                else:
                    fout.write(orig_line))
                    fout.write("\n")


def parse_sfxfile(filename):
    wavs = []
    with open(filename, 'rt', encoding='latin-1') as fin:
        for orig_line in fin.read().splitlines():
            m = comment_regex.match(orig_line)
            if m:
                comment = m.group(2)
                line = m.group(1)
            else:
                comment = None
                line = orig_line


            m = wav1_regex.match(line)
            if m:
                wavs.append(rfactortools.nt2posixpath(m.group(3)))
            else:
                m = wav2_regex.match(line)
                if m:
                    wavs.append(rfactortools.nt2posixpath(m.group(2)))
    return wavs


# EOF #
