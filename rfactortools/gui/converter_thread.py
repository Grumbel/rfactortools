# rFactorTools GUI
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
import queue
import threading

import rfactortools


class ConverterThread(threading.Thread):

    def __init__(self, source_directory, target_directory, cfg):
        super().__init__()
        self.msgbox = queue.Queue()
        self.quit = False
        self.cancel_ = False

        self.source_directory = source_directory
        self.target_directory = target_directory
        self.cfg = cfg
        self.progress_cb = lambda *args: None

    def run(self):
        self.convert(self.source_directory, self.target_directory, self.cfg)

    def convert(self, source_directory, target_directory, cfg):
        try:
            converter = rfactortools.rFactorToGSC2013(source_directory, cfg)
            converter.progress_cb = self.progress_callback
            converter.convert_all(target_directory)
            print("-- rfactor-to-gsc2013 conversion complete --")
        except Exception as e:
            logging.exception("conversion failed")
            self.progress_cb("error", e)

    def cancel(self):
        self.cancel_ = True

    def progress_callback(self, *args):
        if args:
            self.progress_cb(*args)

        if self.cancel_:
            raise RuntimeError("ConvertThread: cancel received")


# EOF #
