#!/usr/bin/env python3

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


from tkinter import \
    Button, Checkbutton, Entry, Frame, Label, LabelFrame, Scrollbar, Text, Toplevel, \
    N, S, W, E, CENTER, BOTH, LEFT, RIGHT, END, DISABLED, \
    StringVar, BooleanVar
import PIL.Image
import PIL.ImageTk
import argparse
import datetime
import logging
import os
import queue
import sys
import threading
import tkinter.filedialog
import tkinter.messagebox

import rfactortools


try:
    with open("HOWTO.txt", "r") as fin:
        welcome_text = fin.read()
except Exception:
    welcome_text = "error: couldn't locate HOWTO.txt"


def createDirectoryEntry(frame, name, row):
    directory_label = Label(frame, text=name)
    directory_label.grid(column=0, row=row, sticky=N+S+E, pady=4)

    directory = StringVar()
    directory_entry = Entry(frame, textvariable=directory)
    directory_entry.grid(column=1, row=row, sticky=N+S+W+E, pady=4)

    directory_button = Button(frame)
    directory_button["text"] = "Browse"
    directory_button.grid(column=2, row=row, sticky=N+S, pady=4)
    directory_button["command"] = lambda: do_ask_directory(directory)

    return directory


def do_ask_directory(directory):
    d = tkinter.filedialog.askdirectory()
    if d:
        directory.set(d)


class ProgressWindow(Toplevel):

    def __init__(self, parent):
        super().__init__(parent)
        self.title("rfactortools: Conversion Progress")
        self.transient(parent)

        self.minsize(730, 300)

        self.protocol("WM_DELETE_WINDOW", self.cancel)
        self.geometry("+%d+%d" % (parent.winfo_rootx()+50,
                                  parent.winfo_rooty()+30))

        self.conversion_had_errors = False
        # self.gui_progressbar = None
        self.text = None
        self.createWidgets()

        self.msgbox = queue.Queue()
        self.converter_thread = ConverterThread(self)
        self.converter_thread.start()

    def wait_for_conversion(self):
        self.grab_set()
        self.focus_set()
        self.check_msgbox()
        self.wait_window(self)

    def check_msgbox(self):
        try:
            args = self.msgbox.get_nowait()
            self.update_progress(*args)

        except queue.Empty:
            pass

        self.after(15, self.check_msgbox)

    def request(self, *args):
        self.msgbox.put(args)

    def createWidgets(self):
        # label = tkinter.Label(self, text="Converting:")
        # label.pack(anchor=tkinter.W, fill=tkinter.X, expand=0, pady=8, padx=8)

        # gui_progressbar = tkinter.ttk.Progressbar(self, mode='determinate', maximum=100)
        # gui_progressbar.pack(anchor=CENTER, fill=tkinter.X, expand=1, pady=4, padx=8)
        # self.gui_progressbar = gui_progressbar

        textarea = Frame(self)
        textarea.pack(expand=1, fill=tkinter.BOTH)

        textarea.grid_rowconfigure(0, weight=1)
        textarea.grid_columnconfigure(0, weight=1)

        scrollbar = Scrollbar(textarea)
        scrollbar.grid(column=1, row=0, sticky=N+S)

        text = Text(textarea, yscrollcommand=scrollbar.set, height=3)
        text.config(state=DISABLED)
        text.grid(column=0, row=0, sticky=N+S+W+E)
        scrollbar.config(command=text.yview)

        frame = Frame(self)
        frame.pack(anchor=S+W, expand=0, fill=tkinter.X)

        confirm_button_frame = tkinter.Frame(frame)
        confirm_button_frame.pack(side=tkinter.RIGHT)

        cancel_btn = Button(confirm_button_frame)
        cancel_btn["text"] = "Cancel"
        cancel_btn["command"] = self.cancel
        # cancel_btn.config(state=DISABLED)
        cancel_btn.grid(column=3, row=0, sticky=S, pady=8, padx=8)

        self.cancel_btn = cancel_btn
        self.text = text

    def cancel(self):
        self.converter_thread.cancel()
        self.converter_thread.join()
        tkinter.messagebox.showerror("rfactortools: Conversion canceled",
                                     "The conversion has been canceled",
                                     parent=self)
        self.destroy()

    def update_progress(self, *args):
        msg, *args = args

        if msg == "start":
            self.conversion_had_errors = False

        elif msg == "finished":
            self.cancel_btn["text"] = "Finish"
            self.cancel_btn.config(state=tkinter.NORMAL)

            self.text.config(state=tkinter.NORMAL)
            self.text.insert(END, "-------------------------------------------------------------------\n")
            if self.conversion_had_errors:
                self.text.insert(END, "Conversion finished")
            else:
                self.text.insert(END,
                                 "Conversion finished, but there have been errors, "
                                 "check the logs/ for more details")
            self.text.config(state=tkinter.DISABLED)

        elif msg == "error":
            self.cancel_btn.config(state=tkinter.NORMAL)

            self.text.config(state=tkinter.NORMAL)
            self.text.insert(END, "ERROR: Conversion failed: %s\n" % args[0])
            self.text.insert(END, "\nSee logs/ for more details.")
            self.text.config(state=tkinter.DISABLED)

        elif msg == "file_ignored":
            self.text.config(state=tkinter.NORMAL)
            self.text.insert(END, " ignored\n")
            self.text.config(state=tkinter.DISABLED)

        elif msg == "file_error":
            self.text.config(state=tkinter.NORMAL)
            self.text.insert(END, "  error\n")
            self.text.config(state=tkinter.DISABLED)
            self.conversion_had_errors = True

        elif msg == "file_done":
            self.text.config(state=tkinter.NORMAL)
            self.text.insert(END, "   done\n")
            self.text.config(state=tkinter.DISABLED)

        elif msg == "file":
            modname, filename = args

            txt = "%s: %-60s" % (modname, filename + "...")

            self.text.config(state=tkinter.NORMAL)
            self.text.insert(END, txt)
            self.text.config(state=tkinter.DISABLED)

        elif msg == "directory":
            directory, = args
            self.text.config(state=tkinter.NORMAL)
            self.text.insert(END, "Converting directory '%s':\n" % directory)
            self.text.config(state=tkinter.DISABLED)

        else:
            raise RuntimeError("ProgressWindow: unknown msg: %s %s" % (msg, args))
        self.text.yview(END)


class MainWindow(Frame):

    def __init__(self, master=None):
        super().__init__(master)

        self.source_directory = None
        self.target_directory = None

        self.createWidgets()
        self.pack(anchor=CENTER, fill=BOTH, expand=1)

    def do_close_window(self):
        print("----------------- close window")

    def createWidgets(self):
        self.grid_columnconfigure(1, weight=1)
        self.grid_columnconfigure(0, weight=0)
        self.grid_rowconfigure(0, weight=1)

        self.photo = PIL.ImageTk.PhotoImage(PIL.Image.open("logo.png"))
        self.photo_label = Label(self, image=self.photo, anchor=N, width=256, height=256)
        self.photo_label["bg"] = "black"
        self.photo_label.grid(column=0, row=0, sticky=N+S+W+E)
        self.photo_label.winfo_height = 256

        self.scrollbar = Scrollbar(self)
        self.scrollbar.grid(column=2, row=0, sticky=N+S)

        self.text = Text(self, yscrollcommand=self.scrollbar.set)
        self.text.insert(END, welcome_text)
        self.text.config(state=DISABLED)
        self.text.grid(column=1, row=0, sticky=N+S+W+E)
        self.scrollbar.config(command=self.text.yview)

        self.directory_frame = Frame(self)
        self.directory_frame.grid_columnconfigure(1, weight=1)
        self.source_directory = createDirectoryEntry(self.directory_frame, "Input:", 0)
        self.target_directory = createDirectoryEntry(self.directory_frame, "Output:", 1)
        self.target_directory.set("build/")
        self.directory_frame.grid(column=0, row=1, columnspan=3, sticky=W+E, padx=8, pady=4)

        # Options
        self.option_frame = LabelFrame(self, text="Options", padx=8, pady=4)
        self.option_frame.grid(column=0, row=2, columnspan=3, sticky=N+S+W+E, padx=8, pady=4)
        self.option_frame.grid_columnconfigure(1, minsize=200)

        defaults = rfactortools.rFactorToGSC2013Config()

        self.unique_team_names = BooleanVar(value=defaults.unique_team_names)
        self.unique_team_names_checkbox = Checkbutton(self.option_frame, text="Unique Team Names",
                                                      variable=self.unique_team_names)
        self.unique_team_names_checkbox.grid(column=0, row=0, columnspan=2, sticky=W)

        self.force_track_thumb = BooleanVar(value=defaults.force_track_thumbnails)
        self.force_track_thumb_checkbox = Checkbutton(self.option_frame, text="Force Track Thumbnail",
                                                      variable=self.force_track_thumb)
        self.force_track_thumb_checkbox.grid(column=0, row=1, columnspan=2, sticky=W)

        self.clear_classes = BooleanVar(value=defaults.clear_classes)
        self.clear_classes_checkbox = Checkbutton(self.option_frame, text="Clear Vehicle Classes",
                                                  variable=self.clear_classes)
        self.clear_classes_checkbox.grid(column=0, row=2, columnspan=2, sticky=W)

        self.single_gamedata = BooleanVar(value=defaults.single_gamedata)
        self.single_gamedata_checkbox = Checkbutton(self.option_frame, text="Single GameData/ Output",
                                                    variable=self.single_gamedata)
        self.single_gamedata_checkbox.grid(column=0, row=3, columnspan=2, sticky=W)

        self.vehicle_category = StringVar(value=defaults.vehicle_category)
        self.vehicle_category_label = Label(self.option_frame, text="Vehicle Category:")
        self.vehicle_category_label.grid(column=2, row=0, sticky=E)
        self.vehicle_category_entry = Entry(self.option_frame, textvariable=self.vehicle_category)
        self.vehicle_category_entry.grid(column=3, row=0, sticky=W)

        self.track_category = StringVar(value=defaults.track_category)
        self.track_category_label = Label(self.option_frame, text="Track Category:")
        self.track_category_label.grid(column=2, row=1, sticky=E)
        self.track_category_entry = Entry(self.option_frame, textvariable=self.track_category)
        self.track_category_entry.grid(column=3, row=1, sticky=W)

        self.reiza_class = StringVar(value=defaults.reiza_class)
        self.reiza_class_label = Label(self.option_frame, text="Reiza Class:")
        self.reiza_class_label.grid(column=2, row=2, sticky=E)
        self.reiza_class_entry = Entry(self.option_frame, textvariable=self.reiza_class)
        self.reiza_class_entry.grid(column=3, row=2, sticky=W)

        self.track_filter_properties = StringVar(value=defaults.track_filter_properties)
        self.track_filter_properties_label = Label(self.option_frame, text="Track Filter Properties:")
        self.track_filter_properties_label.grid(column=2, row=3, sticky=E)
        self.track_filter_properties_entry = Entry(self.option_frame, textvariable=self.track_filter_properties)
        self.track_filter_properties_entry.grid(column=3, row=3, sticky=W)

        # Buttons
        self.button_frame = Frame(self)
        self.button_frame.grid(column=0, row=3, columnspan=3, sticky=W+E+N+S)

        self.confirm_button_frame = Frame(self.button_frame)
        self.confirm_button_frame.pack(side=RIGHT)

        self.tool_button_frame = Frame(self.button_frame)
        self.tool_button_frame.pack(side=LEFT)

        self.vehtree_btn = Button(self.tool_button_frame)
        self.vehtree_btn["text"] = ".veh tree"
        self.vehtree_btn["command"] = self.do_veh_tree
        self.vehtree_btn.grid(column=0, row=0, sticky=S, pady=8, padx=8)

        self.veh_btn = Button(self.tool_button_frame)
        self.veh_btn["text"] = ".veh check"
        self.veh_btn["command"] = self.do_veh_check
        self.veh_btn.grid(column=1, row=0, sticky=S, pady=8, padx=8)

        self.gen_btn = Button(self.tool_button_frame)
        self.gen_btn["text"] = ".gen check"
        self.gen_btn["command"] = self.do_gen_check
        self.gen_btn.grid(column=2, row=0, sticky=S, pady=8, padx=8)

        self.cancel_btn = Button(self.confirm_button_frame)
        self.cancel_btn["text"] = "Quit"
        self.cancel_btn["command"] = self.quit
        self.cancel_btn.grid(column=3, row=0, sticky=S, pady=8, padx=8)

        self.convert_btn = Button(self.confirm_button_frame)
        self.convert_btn["text"] = "Convert"
        self.convert_btn["command"] = self.do_conversion
        self.convert_btn.grid(column=4, row=0, sticky=S, pady=8, padx=8)

    def do_conversion(self):
        if not self.source_directory.get():
            tkinter.messagebox.showerror("Input directory not selected",
                                         "Input directory not selected",
                                         parent=self)

        elif not self.target_directory.get():
            tkinter.messagebox.showerror("Error: Output directory not selected",
                                         "Output directory not selected",
                                         parent=self)

        else:
            logging.info("source-directory: %s", self.source_directory.get())
            logging.info("target-directory: %s", self.target_directory.get())

            cfg = rfactortools.rFactorToGSC2013Config()

            cfg.unique_team_names = self.unique_team_names.get()
            cfg.force_track_thumbnails = self.force_track_thumb.get()
            cfg.clear_classes = self.clear_classes.get()
            cfg.single_gamedata = self.single_gamedata.get()

            if self.vehicle_category.get().strip():
                cfg.vehicle_category = self.vehicle_category.get().strip()

            if self.track_category.get().strip():
                cfg.track_category = self.track_category.get().strip()

            cfg.reiza_class = self.reiza_class.get().strip()

            cfg.track_filter_properties = self.track_filter_properties.get().strip()

            self._do_conversion(cfg)

    def _do_conversion(self, cfg):
        self.gui_progress_window = ProgressWindow(self)

        self.gui_progress_window.converter_thread.request("convert",
                                                          self.source_directory.get(),
                                                          self.target_directory.get(),
                                                          cfg)
        self.gui_progress_window.wait_for_conversion()
        self.gui_progress_window = None

    def do_veh_tree(self):
        print("--- veh tree: start ---")
        path = self.target_directory.get()
        files = rfactortools.find_files(path, ".veh")
        vehs = [rfactortools.parse_vehfile(filename) for filename in files]
        rfactortools.print_veh_tree(vehs)
        print("--- veh tree: end ---")

    def do_veh_check(self):
        print("--- veh check: start ---")
        path = self.target_directory.get()
        files = rfactortools.find_files(path, ".veh")
        vehs = [rfactortools.parse_vehfile(filename) for filename in files]
        rfactortools.print_veh_info(vehs)
        print("--- veh check: end ---")

    def do_gen_check(self):
        print("--- gen check: start ---")
        path = self.target_directory.get()
        rfactortools.process_gen_directory(path, False)
        print("--- gen check: end ---")


class ConverterThread(threading.Thread):

    def __init__(self, parent):
        super().__init__()
        self.msgbox = queue.Queue()
        self.quit = False
        self.cancel_ = False
        self.parent = parent

    def run(self):
        self.check_msgbox()

    def check_msgbox(self):
        while not self.quit:
            msg, *args = self.msgbox.get()
            if msg == "convert":
                self.convert(*args)
            elif msg == "cancel":
                self.quit = True
            else:
                logging.error("unknown message: %s %s", msg, args)

    def request(self, *args):
        self.msgbox.put(args)

    def cancel(self):
        self.cancel_ = True
        self.request("cancel")

    def progress_callback(self, *args):
        if args:
            self.parent.request(*args)

        if self.cancel_:
            raise RuntimeError("ConvertThread: cancel received")

    def convert(self, source_directory, target_directory, cfg):
        print("Convert: %s %s %s" % (source_directory, target_directory, cfg))
        try:
            converter = rfactortools.rFactorToGSC2013(source_directory, cfg)
            converter.progress_cb = self.progress_callback
            converter.convert_all(target_directory)
            print("-- rfactor-to-gsc2013 conversion complete --")
        except Exception as e:
            logging.exception("conversation failed")
            self.parent.request("error", e)


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


if __name__ == "__main__":
    app = Application()
    app.main()

# EOF #
