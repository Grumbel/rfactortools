##  Utility functions
##  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
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

import os

def find_files(directory, ext = None):
    results = []
    for path, dirs, files in os.walk(directory):
        for fname in files:
            if ext and os.path.splitext(fname)[1].lower() == ext:
                results.append(os.path.join(path, fname))
            elif ext == None:
                results.append(os.path.join(path, fname))

    return results

# EOF #
