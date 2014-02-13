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

from PIL import Image
import os
import argparse

def resize_to_fit(img, w, h):
    iw, ih = img.size

    if iw <= w and ih <= h:
        return img
    else:
        w_ratio = w / iw
        h_ratio = h / ih

        if w_ratio > h_ratio:
            return img.resize((int(w * h_ratio), h), Image.ANTIALIAS)
        else:
            return img.resize((w, int(h * w_ratio)), Image.ANTIALIAS)

def resize_to_fit_img_file(filename, w, h):
    img = Image.open(filename)
    newimg = resize_to_fit(img, w, h)
    if newimg != img:
        print("resizing %s %s to %s" % (filename, img.size, newimg.size))
        newimg.save(filename)
    else:
        print("ignoring %s %s" % (filename, img.size))

def process_directory(directory):
    for path, dirs, files in os.walk(directory):
        for fname in files:
            name, ext = os.path.splitext(fname)
            if ext.lower() == ".veh":
                img = os.path.join(path, name + "number.tga")
                if os.path.exists(img):
                    resize_to_fit_img_file(img, 252, 64)
                
            elif ext.lower() == ".gdb":
                img = os.path.join(path, name + "mini.tga")
                if os.path.exists(img):
                    resize_to_fit_img_file(img, 252, 249)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor thumbnail image resizer')
    parser.add_argument('DIRECTORY', action='store', type=str,
                        help='images in DIRECTORY will be resized to GSC2013 standards')
    args = parser.parse_args()
    
    process_directory(args.DIRECTORY)

# EOF #
