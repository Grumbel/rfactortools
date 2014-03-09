#!/usr/bin/env python3

# Minimal sed replacement
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


from tkinter import *
import tkinter.tix
import tkinter.filedialog
import tkinter.messagebox
import tkinter.tix
import PIL.Image
import PIL.ImageTk
import os
import fnmatch


def do_ask_directory(directory):
    d = tkinter.filedialog.askdirectory()
    if d:
        directory.set(d)


def minised_on_lines(lines, pattern, replacement, ignore_case, only_replaced_lines=False):
    """Performes search&replace on a list of strings

    Returns list of lines, in cases where a replacement is done it
    returns a tuple (left, original, replacement, right) instead.
    """

    result = []

    flags = 0
    if ignore_case:
        flags |= re.I

    rx = re.compile(pattern, flags)
    for line in lines:
        m = rx.search(line)
        if m:
            left = line[:m.span()[0]]
            right = line[m.span()[1]:]
            middle = line[m.span()[0]:m.span()[1]]

            if replacement is not None:
                expanded = m.expand(replacement)

                result.append((left, middle, expanded, right))
            else:
                result.append((left, middle, middle, right))
        else:
            if not only_replaced_lines:
                result.append(line)

    return result


class Application(Frame):

    def __init__(self, master=None):
        Frame.__init__(self, master)

        self.source_directory = None
        self.target_directory = None

        self.createWidgets()
        self.pack(anchor=CENTER, fill=BOTH, expand=1)

    def createWidgets(self):

        self.directory_frame = Frame(self)
        self.directory_frame.pack(side=TOP, fill=X, expand=0, padx=4, pady=4)
        self.directory_frame.grid_columnconfigure(1, weight=1)

        self.directory_label = Label(self.directory_frame, text="Directory:")
        self.directory_label.grid(column=0, row=0)

        self.directory = StringVar(value="build/")
        self.directory_entry = Entry(self.directory_frame, textvariable=self.directory)
        self.directory_entry.grid(column=1, row=0, sticky=W + E)
        self.directory_entry.bind('<Return>', self.do_preview)

        self.directory_button = Button(self.directory_frame, text="Browse")
        self.directory_button["command"] = lambda: do_ask_directory(self.directory)
        self.directory_button.grid(column=2, row=0)

        self.glob_label = Label(self.directory_frame, text="Glob:")
        self.glob_label.grid(column=0, row=1, stick=E)

        self.glob = StringVar(value="*.veh")
        self.glob_entry = Entry(self.directory_frame, textvariable=self.glob)
        self.glob_entry.bind('<Return>', self.do_preview)
        self.glob_entry.grid(column=1, row=1, sticky=N + S + W)

        self.search_replace_frame = Frame(self)
        self.search_replace_frame.grid_columnconfigure(1, weight=1)
        self.search_replace_frame.pack(anchor=N, side=TOP, fill=X, expand=0, padx=4, pady=4)

        self.search = StringVar(value="Team=")
        self.search_label = Label(self.search_replace_frame, text="Search:")
        self.search_label.grid(column=0, row=0, sticky=E)
        self.search_entry = Entry(self.search_replace_frame, textvariable=self.search)
        self.search_entry.grid(column=1, row=0, sticky=N + S + W + E)
        self.search_entry.bind('<Return>', self.do_preview)

        self.replace = StringVar(value="Nothing")
        self.replace_label = Label(self.search_replace_frame, text="Replace:")
        self.replace_label.grid(column=0, row=1, sticky=E)
        self.replace_entry = Entry(self.search_replace_frame, textvariable=self.replace)
        self.replace_entry.grid(column=1, row=1, sticky=N + S + W + E)
        self.replace_entry.bind('<Return>', self.do_preview)

        self.ignore_case = BooleanVar(value=True)
        self.ignore_case_checkbutton = Checkbutton(self, text="ignore case", variable=self.ignore_case)
        self.ignore_case_checkbutton.pack(side=TOP, anchor=W, expand=0)

        self.show_full_content = BooleanVar(value=False)
        self.show_full_content_checkbutton = Checkbutton(
            self, text="show full content", variable=self.show_full_content)
        self.show_full_content_checkbutton.pack(side=TOP, anchor=W, expand=0)

        self.mark_replacements = BooleanVar(value=True)
        self.mark_replacements_checkbutton = Checkbutton(
            self, text="mark replacements", variable=self.mark_replacements)
        self.mark_replacements_checkbutton.pack(side=TOP, anchor=W, expand=0)

        self.text_frame = Frame(self)
        self.text_frame.pack(side=TOP, fill=BOTH, expand=1, pady=4)

        self.text_frame.grid_columnconfigure(0, weight=1)
        self.text_frame.grid_rowconfigure(0, weight=1)

        self.scrollbar = Scrollbar(self.text_frame)
        self.scrollbar.grid(column=1, row=0, sticky=N + S)

        self.text = Text(self.text_frame, yscrollcommand=self.scrollbar.set, width=1, height=1)

        self.text.tag_config("file", background="lightgrey", foreground="black")
        self.text.tag_config("search", background="lightblue", foreground="black")
        self.text.tag_config("replace", background="orange", foreground="black")

        self.text.insert(END, "place holder")
        self.text.config(state=DISABLED)
        self.text.grid(column=0, row=0, sticky=N + S + W + E)
        self.scrollbar.config(command=self.text.yview)

        self.confirm_button_frame = Frame(self)
        self.confirm_button_frame.pack(side=BOTTOM, anchor=E)

        self.cancel_btn = Button(self.confirm_button_frame)
        self.cancel_btn["text"] = "Quit"
        self.cancel_btn["command"] = self.quit
        self.cancel_btn.grid(column=1, row=0, sticky=S, pady=8, padx=8)

        self.preview_btn = Button(self.confirm_button_frame)
        self.preview_btn["text"] = "Preview"
        self.preview_btn["command"] = self.do_preview
        self.preview_btn.grid(column=2, row=0, sticky=S, pady=8, padx=8)

        self.run_btn = Button(self.confirm_button_frame)
        self.run_btn["text"] = "Apply"
        self.run_btn["command"] = self.do_execute
        self.run_btn.grid(column=3, row=0, sticky=S, pady=8, padx=8)
        self.run_btn["state"] = 'disabled'

        # Directory
        # File Glob
        # ignore case
    def do_preview(self, *args):
        print(args)
        directory = self.directory.get()

        self.text.config(state=NORMAL)
        self.text.delete("1.0", END)
        self.text.config(state=DISABLED)

        for path, dirs, files in os.walk(directory):
            for fname in files:
                filename = os.path.join(path, fname)

                if not self.glob.get() or fnmatch.fnmatch(fname.lower(), self.glob.get().lower()):
                    print("%s" % filename)

                    with open(filename, 'rt', encoding='latin-1') as fin:
                        lines = fin.read().splitlines()
                    lines = minised_on_lines(lines, self.search.get(), self.replace.get(),
                                             self.ignore_case.get(), True)

                    self.text.config(state=NORMAL)
                    self.text.insert(END, "%s:\n" % filename, "file")
                    for line in lines:
                        if isinstance(line, tuple):
                            self.text.insert(END, "%s" % line[0])
                            self.text.insert(END, "%s" % line[1], "search")
                            self.text.insert(END, "%s\n" % line[3])

                            self.text.insert(END, "%s" % line[0])
                            self.text.insert(END, "%s" % line[2], "replace")
                            self.text.insert(END, "%s\n" % line[3])
                        else:
                            self.text.insert(END, "  %s\n" % line)
                    self.text.insert(END, "\n")
                    self.text.config(state=DISABLED)

                    self.master.update()

    def do_execute(self):
        pass


def main():
    root = tkinter.tix.Tk()
    root.wm_title("minised")
    root.minsize(600, 400)
    # root.geometry("800x500+0+0")

    app = Application(master=root)
    app.mainloop()
    root.destroy()


if __name__ == "__main__":
    main()


# EOF #
