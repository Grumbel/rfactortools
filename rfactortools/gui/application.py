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


import tkinter
import argparse
import datetime
import logging
import os
import sys


from .main_window import MainWindow


class Application:

    def on_close_window_request(self, *args):
        if self.app.gui_progress_window:
            pass  # ignore close requests while conversion is running
        else:
            self.root.destroy()

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
        args = parser.parse_args()

        self.root = tkinter.Tk()
        self.root.wm_title("rfactortools: rFactor to Game Stock Car 2013 Mod Converter V0.3.0")
        self.root.minsize(640, 400)
        self.root.protocol("WM_DELETE_WINDOW", self.on_close_window_request)

        self.app = MainWindow(master=self.root)

        if args.INPUTDIR is not None:
            self.app.source_directory.set(args.INPUTDIR)

        if args.OUTPUTDIR is not None:
            self.app.target_directory.set(args.OUTPUTDIR)

        self.app.mainloop()


# EOF #
