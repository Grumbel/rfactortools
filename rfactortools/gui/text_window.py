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
import tkinter as tk


class TextWindow(tk.Toplevel):

    def __init__(self, parent):
        super().__init__(parent)

        self.text = None
        self.createWidgets()
        self.minsize(720, 480)

    def createWidgets(self):
        textarea = tk.Frame(self)
        textarea.pack(expand=1, fill=tk.BOTH)

        textarea.grid_rowconfigure(0, weight=1)
        textarea.grid_columnconfigure(0, weight=1)

        scrollbar = tk.Scrollbar(textarea)
        scrollbar.grid(column=1, row=0, sticky=N+S)

        text = tk.Text(textarea, yscrollcommand=scrollbar.set, height=3)
        text.config(state=tk.DISABLED)
        text.grid(column=0, row=0, sticky=N+S+W+E)
        scrollbar.config(command=text.yview)

        frame = tk.Frame(self)
        frame.pack(anchor=S+W, expand=0, fill=tk.X)

        confirm_button_frame = tk.Frame(frame)
        confirm_button_frame.pack(side=tk.RIGHT)

        close_btn = tk.Button(confirm_button_frame)
        close_btn["text"] = "Close"
        close_btn["command"] = self.on_close
        close_btn.grid(column=3, row=0, sticky=S, pady=8, padx=8)

        self.text = text

    def on_close(self):
        self.destroy()

    def set_text(self, text):
        self.text.config(state=tk.NORMAL)
        self.text.insert(tk.END, text)
        self.text.config(state=tk.DISABLED)


# EOF #
