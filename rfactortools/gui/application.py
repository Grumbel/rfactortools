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


import argparse
import datetime
import logging
import os
import sys

from .converter_thread import ConverterThread
from .main_window import MainWindow
from .progress_window import ProgressWindow
from .text_window import TextWindow


class Application:

    def __init__(self):
        self.converter_thread = None

    def start_conversion(self, source_directory, target_directory, cfg):
        assert self.converter_thread is None

        progress_window = ProgressWindow(self, self.gui_main_window)
        self.converter_thread = ConverterThread(source_directory, target_directory, cfg)
        self.converter_thread.progress_cb = progress_window.request

        self.converter_thread.start()
        progress_window.wait_for_conversion()
        self.cancel_conversion()
        progress_window = None
        self.converter_thread = None

    def cancel_conversion(self):
        if self.converter_thread is not None:
            self.converter_thread.cancel()
            self.converter_thread.join()

    def show_text_window(self, title, text):
        text_window = TextWindow(self.gui_main_window)
        text_window.title(title)
        text_window.set_text(text)

    def main(self):
        logger = logging.getLogger()
        logger.setLevel(logging.DEBUG)

        formatter = logging.Formatter("%(levelname)s: %(message)s")

        handler = logging.StreamHandler(sys.stderr)
        handler.setLevel(logging.INFO)
        handler.setFormatter(formatter)
        logger.addHandler(handler)

        time_str = datetime.datetime.now().strftime("%Y-%m-%dT%H%M%S")
        if not os.path.isdir("logs"):
            os.mkdir("logs")
        handler = logging.FileHandler("logs/rfactortools-gui-%s.log" % time_str, mode='w')
        handler.setLevel(logging.DEBUG)
        handler.setFormatter(formatter)
        logger.addHandler(handler)

        parser = argparse.ArgumentParser(description='rFactor to GSC2013 converter')
        parser.add_argument('INPUTDIR', action='store', type=str, nargs='?',
                            help='directory containing the mod')
        parser.add_argument('OUTPUTDIR', action='store', type=str, nargs='?',
                            help='directory where the conversion will be written')
        parser.add_argument('-s', '--start', action='store_true',
                            help='start conversion instantly')
        args = parser.parse_args()

        self.gui_main_window = MainWindow(self)

        if args.INPUTDIR is not None:
            self.gui_main_window.source_directory.set(args.INPUTDIR)

        if args.OUTPUTDIR is not None:
            self.gui_main_window.target_directory.set(args.OUTPUTDIR)

        if args.start:
            self.gui_main_window.do_conversion()

        self.gui_main_window.mainloop()


# EOF #
