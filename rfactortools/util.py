# Utility functions
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


import os
import pathlib
import ntpath
import posixpath


def nt2posixpath(path):
    return path.replace(ntpath.sep, posixpath.sep)


def find_files(directory, ext=None):
    results = []
    for path, dirs, files in os.walk(directory):
        for fname in files:
            if ext and os.path.splitext(fname)[1].lower() == ext:
                results.append(os.path.join(path, fname))
            elif ext is None:
                results.append(os.path.join(path, fname))

    return results


def _lookup_path_icase(root, rest):
    """
    Given a root directory and a list of directory parts, find all
    path that match root + rest, while ignoring the case of the filenames.
    """
    if not rest:
        return [os.path.normpath(root)]
    else:
        results = []
        if os.path.isdir(root):
            for f in os.listdir(root):
                if f.lower() == rest[0].lower():
                    p = os.path.join(root, f)
                    results += _lookup_path_icase(p, rest[1:])
        return results


def lookup_path_icase(filename):
    """Returns list of files matching filename in a case insensitive manner"""

    assert filename, "filename must not be empty: %s" % filename

    filename = os.path.normpath(filename)
    path = pathlib.Path(filename)
    parts = path.parts

    if not path.is_absolute():
        return _lookup_path_icase(os.curdir, parts)
    else:
        return _lookup_path_icase(parts[0], parts[1:])


# EOF #
