#!/usr/bin/env python3

##  rFactor .gen file manipulation tool
##  Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
##
##  This program is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys
import os

class VFS:
    """Simple virtual file system that is case insensitive"""
    
    def __init__(self, directory):
        """
        Creates a VFS rooted in 'directory'
        """

        self._files = {}
        self._directories = {}

        for path, dirs, files in os.walk(directory):
            for d in dirs:
                fullpath = os.path.normpath(os.path.join(path, d))
                self._directories[fullpath.lower()] = fullpath

            for fname in files:
                fullpath = os.path.normpath(os.path.join(path, fname))
                self._files[fullpath.lower()] = fullpath

    def directory_exists(self, path):
        try:
            _ = self.lookup_directory(path)
            return True
        except:
            return False

    def file_exists(self, path):
        try:
            _ = self.lookup_file(path)
            return True
        except:
            return False

    def open_read(self, path, encoding=None):
        return open(self.lookup_file(path), 'r', encoding=encoding)

    def open_write(self, path, encoding=None):
        return open(self.lookup_file(path), 'w', encoding=encoding)

    def lookup_directory(self, path):
        try:
            return self._directories[os.path.normpath(path).lower()]
        except:
            raise Exception("%s: VFS couldn't locate directory" % path)

    def lookup_file(self, path):
        try:
            return self._files[os.path.normpath(path).lower()]
        except:
            raise Exception("%s: VFS couldn't locate file" % path)

    def files(self):
        return self._files.values()

if __name__ == "__main__":
    for p in sys.argv[1:]:
        vfs = VFS(p)
        for f in vfs.files():
            print(f)

# EOF #
