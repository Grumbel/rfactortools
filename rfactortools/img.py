# rFactor image file manipulation functions
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


from PIL import Image
import logging


def resize_to_fit(img, w, h):
    iw, ih = img.size

    if iw <= w and ih <= h:
        return img
    else:
        w_ratio = w / iw
        h_ratio = h / ih

        if w_ratio > h_ratio:
            return img.resize((int(iw * h_ratio), h), Image.ANTIALIAS)
        else:
            return img.resize((w, int(ih * w_ratio)), Image.ANTIALIAS)


def resize_to_fit_img_file(filename, w, h):
    img = Image.open(filename)
    newimg = resize_to_fit(img, w, h)

    if newimg is not img:
        print("resizing %s %s to %s" % (filename, img.size, newimg.size))
        newimg.save(filename)
    else:
        print("ignoring %s %s" % (filename, img.size))


def resize_to_fit_img_file_with_target(source_file, target_file, w, h):
    img = Image.open(source_file)
    newimg = resize_to_fit(img, w, h)

    if img is not newimg:
        logging.debug("%s: resizing from %s to %s, saving to %s", source_file, img.size, newimg.size, target_file)
        newimg.save(target_file)
    else:
        logging.debug("%s: not resizing %s, saving to %s", source_file, newimg.size, target_file)
        newimg.save(target_file)


# EOF #
