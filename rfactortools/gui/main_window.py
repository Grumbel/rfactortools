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


from tkinter import N, S, W, E
import PIL.Image
import PIL.ImageTk
import logging
import tkinter as tk
import tkinter.filedialog
import tkinter.messagebox
import io

import rfactortools


try:
    with open("HOWTO.txt", "r") as fin:
        welcome_text = fin.read()
except Exception:
    welcome_text = "error: couldn't locate HOWTO.txt"


def createDirectoryEntry(frame, name, row):
    directory_label = tk.Label(frame, text=name)
    directory_label.grid(column=0, row=row, sticky=N+S+E, pady=4)

    directory = tk.StringVar()
    directory_entry = tk.Entry(frame, textvariable=directory)
    directory_entry.grid(column=1, row=row, sticky=N+S+W+E, pady=4)

    directory_button = tk.Button(frame)
    directory_button["text"] = "Browse"
    directory_button.grid(column=2, row=row, sticky=N+S, pady=4)
    directory_button["command"] = lambda: do_ask_directory(directory)

    return directory


def do_ask_directory(directory):
    d = tkinter.filedialog.askdirectory()
    if d:
        directory.set(d)


class MainWindow(tk.Tk):

    def __init__(self, app):
        super().__init__()

        self.app = app

        self.wm_title("rfactortools: rFactor to Game Stock Car 2013 Mod Converter V0.3.1")
        self.minsize(640, 400)
        self.protocol("WM_DELETE_WINDOW", self.on_close_window_request)

        self.source_directory = None
        self.target_directory = None

        self.createWidgets()

    def on_close_window_request(self, *args):
        if self.app.converter_thread:
            pass  # ignore close requests while conversion is running
        else:
            self.destroy()

    def createWidgets(self):
        self.grid_columnconfigure(1, weight=1)
        self.grid_columnconfigure(0, weight=0)
        self.grid_rowconfigure(0, weight=1)

        self.photo = PIL.ImageTk.PhotoImage(PIL.Image.open("logo.png"))
        self.photo_label = tk.Label(self, image=self.photo, anchor=N, width=256, height=256)
        self.photo_label["bg"] = "black"
        self.photo_label.grid(column=0, row=0, sticky=N+S+W+E)
        self.photo_label.winfo_height = 256

        self.scrollbar = tk.Scrollbar(self)
        self.scrollbar.grid(column=2, row=0, sticky=N+S)

        self.text = tk.Text(self, yscrollcommand=self.scrollbar.set)
        self.text.insert(tk.END, welcome_text)
        self.text.config(state=tk.DISABLED)
        self.text.grid(column=1, row=0, sticky=N+S+W+E)
        self.scrollbar.config(command=self.text.yview)

        self.directory_frame = tk.Frame(self)
        self.directory_frame.grid_columnconfigure(1, weight=1)
        self.source_directory = createDirectoryEntry(self.directory_frame, "Input:", 0)
        self.target_directory = createDirectoryEntry(self.directory_frame, "Output:", 1)
        self.target_directory.set("build/")
        self.directory_frame.grid(column=0, row=1, columnspan=3, sticky=W+E, padx=8, pady=4)

        # Options
        self.option_frame = tk.LabelFrame(self, text="Options", padx=8, pady=4)
        self.option_frame.grid(column=0, row=2, columnspan=3, sticky=N+S+W+E, padx=8, pady=4)
        self.option_frame.grid_columnconfigure(1, minsize=200)

        defaults = rfactortools.rFactorToGSC2013Config()

        self.unique_team_names = tk.BooleanVar(value=defaults.unique_team_names)
        self.unique_team_names_checkbox = tk.Checkbutton(self.option_frame, text="Unique Team Names",
                                                         variable=self.unique_team_names)
        self.unique_team_names_checkbox.grid(column=0, row=0, columnspan=2, sticky=W)

        self.force_track_thumb = tk.BooleanVar(value=defaults.force_track_thumbnails)
        self.force_track_thumb_checkbox = tk.Checkbutton(self.option_frame, text="Force Track Thumbnail",
                                                         variable=self.force_track_thumb)
        self.force_track_thumb_checkbox.grid(column=0, row=1, columnspan=2, sticky=W)

        self.clear_classes = tk.BooleanVar(value=defaults.clear_classes)
        self.clear_classes_checkbox = tk.Checkbutton(self.option_frame, text="Clear Vehicle Classes",
                                                     variable=self.clear_classes)
        self.clear_classes_checkbox.grid(column=0, row=2, columnspan=2, sticky=W)

        self.single_gamedata = tk.BooleanVar(value=defaults.single_gamedata)
        self.single_gamedata_checkbox = tk.Checkbutton(self.option_frame, text="Single GameData/ Output",
                                                       variable=self.single_gamedata)
        self.single_gamedata_checkbox.grid(column=0, row=3, columnspan=2, sticky=W)

        self.fix_light_intensity = tk.BooleanVar(value=defaults.fix_light_intensity)
        self.fix_light_intensity_checkbox = tk.Checkbutton(self.option_frame, text="Fix Light Intensity",
                                                           variable=self.fix_light_intensity)
        self.fix_light_intensity_checkbox.grid(column=0, row=4, columnspan=2, sticky=W)

        self.copy_missing_textures = tk.BooleanVar(value=defaults.copy_missing_textures)
        self.copy_missing_textures_checkbox = tk.Checkbutton(self.option_frame, text="Copy Missing Textures",
                                                             variable=self.copy_missing_textures)
        self.copy_missing_textures_checkbox.grid(column=0, row=5, columnspan=2, sticky=W)

        self.vehicle_category = tk.StringVar(value=defaults.vehicle_category)
        self.vehicle_category_label = tk.Label(self.option_frame, text="Vehicle Category:")
        self.vehicle_category_label.grid(column=2, row=0, sticky=E)
        self.vehicle_category_entry = tk.Entry(self.option_frame, textvariable=self.vehicle_category)
        self.vehicle_category_entry.grid(column=3, row=0, sticky=W)

        self.track_category = tk.StringVar(value=defaults.track_category)
        self.track_category_label = tk.Label(self.option_frame, text="Track Category:")
        self.track_category_label.grid(column=2, row=1, sticky=E)
        self.track_category_entry = tk.Entry(self.option_frame, textvariable=self.track_category)
        self.track_category_entry.grid(column=3, row=1, sticky=W)

        self.reiza_class = tk.StringVar(value=defaults.reiza_class)
        self.reiza_class_label = tk.Label(self.option_frame, text="Reiza Class:")
        self.reiza_class_label.grid(column=2, row=2, sticky=E)
        self.reiza_class_entry = tk.Entry(self.option_frame, textvariable=self.reiza_class)
        self.reiza_class_entry.grid(column=3, row=2, sticky=W)

        self.track_filter_properties = tk.StringVar(value=defaults.track_filter_properties)
        self.track_filter_properties_label = tk.Label(self.option_frame, text="Track Filter Properties:")
        self.track_filter_properties_label.grid(column=2, row=3, sticky=E)
        self.track_filter_properties_entry = tk.Entry(self.option_frame, textvariable=self.track_filter_properties)
        self.track_filter_properties_entry.grid(column=3, row=3, sticky=W)

        # Buttons
        self.button_frame = tk.Frame(self)
        self.button_frame.grid(column=0, row=3, columnspan=3, sticky=W+E+N+S)

        self.confirm_button_frame = tk.Frame(self.button_frame)
        self.confirm_button_frame.pack(side=tk.RIGHT)

        self.tool_button_frame = tk.Frame(self.button_frame)
        self.tool_button_frame.pack(side=tk.LEFT)

        self.vehtree_btn = tk.Button(self.tool_button_frame)
        self.vehtree_btn["text"] = ".veh tree"
        self.vehtree_btn["command"] = self.do_veh_tree
        self.vehtree_btn.grid(column=0, row=0, sticky=S, pady=8, padx=8)

        self.veh_btn = tk.Button(self.tool_button_frame)
        self.veh_btn["text"] = ".veh check"
        self.veh_btn["command"] = self.do_veh_check
        self.veh_btn.grid(column=1, row=0, sticky=S, pady=8, padx=8)

        self.gen_btn = tk.Button(self.tool_button_frame)
        self.gen_btn["text"] = ".gen check"
        self.gen_btn["command"] = self.do_gen_check
        self.gen_btn.grid(column=2, row=0, sticky=S, pady=8, padx=8)

        self.cancel_btn = tk.Button(self.confirm_button_frame)
        self.cancel_btn["text"] = "Quit"
        self.cancel_btn["command"] = self.quit
        self.cancel_btn.grid(column=3, row=0, sticky=S, pady=8, padx=8)

        self.convert_btn = tk.Button(self.confirm_button_frame)
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
            cfg.fix_light_intensity = self.fix_light_intensity.get()
            cfg.copy_missing_textures = self.copy_missing_textures.get()

            if self.vehicle_category.get().strip():
                cfg.vehicle_category = self.vehicle_category.get().strip()

            if self.track_category.get().strip():
                cfg.track_category = self.track_category.get().strip()

            cfg.reiza_class = self.reiza_class.get().strip()

            cfg.track_filter_properties = self.track_filter_properties.get().strip()

            self.app.start_conversion(self.source_directory.get(), self.target_directory.get(), cfg)

    def do_veh_tree(self):
        path = self.target_directory.get()
        files = rfactortools.find_files(path, ".veh")
        vehs = [rfactortools.parse_vehfile(filename) for filename in files]

        sout = io.StringIO()
        rfactortools.print_veh_tree(vehs, sout)

        self.app.show_text_window("rfactortools: .veh tree", sout.getvalue())

    def do_veh_check(self):
        path = self.target_directory.get()
        files = rfactortools.find_files(path, ".veh")
        vehs = [rfactortools.parse_vehfile(filename) for filename in files]

        sout = io.StringIO()
        rfactortools.print_veh_info(vehs, sout)

        self.app.show_text_window("rfactortools: .veh check", sout.getvalue())

    def do_gen_check(self):
        path = self.target_directory.get()

        sout = io.StringIO()
        rfactortools.process_gen_directory(path, False, sout)

        self.app.show_text_window("rfactortools: .gen check", sout.getvalue())

# EOF #
