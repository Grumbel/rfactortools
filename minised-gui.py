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


from tkinter import \
    Button, Checkbutton, Entry, Frame, Label, LabelFrame, Scrollbar, Text, \
    N, S, W, E, X, CENTER, BOTH, TOP, BOTTOM, LEFT, END, NORMAL, DISABLED, \
    StringVar, BooleanVar
import argparse
import fnmatch
import os
import re
import tkinter.filedialog
import tkinter.messagebox


def do_ask_directory(directory):
    d = tkinter.filedialog.askdirectory(initialdir=directory.get())
    if d:
        directory.set(d)


def minised_on_lines(lines, pattern, replacement, ignore_case):
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
            result.append(line)

    return result


class MiniSedConfig:
    def __init__(self):
        self.directory = StringVar()
        self.glob = StringVar()

        self.search = StringVar()
        self.replace = StringVar()

        self.ignore_case = BooleanVar(value=True)
        self.show_full_content = BooleanVar(value=False)
        self.show_original = BooleanVar(value=True)
        self.create_backups = BooleanVar(value=False)


class MiniSedGUI(Frame):

    def __init__(self, master=None):
        Frame.__init__(self, master)

        self.cfg = MiniSedConfig()

        self.source_directory = None
        self.target_directory = None

        self.createWidgets()
        self.pack(anchor=CENTER, fill=BOTH, expand=1)

        self.files_with_content = []

        self.cfg.directory.trace('w', self.disable_apply)
        self.cfg.glob.trace('w', self.disable_apply)
        self.cfg.search.trace('w', self.disable_apply)
        self.cfg.replace.trace('w', self.disable_apply)
        self.cfg.ignore_case.trace('w', self.disable_apply)

    def disable_apply(self, *args):
        self.run_btn["state"] = 'disabled'

    def createWidgets(self):

        self.directory_frame = Frame(self)
        self.directory_frame.pack(side=TOP, fill=X, expand=0, padx=4, pady=4)
        self.directory_frame.grid_columnconfigure(1, weight=1)

        self.directory_label = Label(self.directory_frame, text="Directory:")
        self.directory_label.grid(column=0, row=0)

        self.directory_entry = Entry(self.directory_frame, textvariable=self.cfg.directory)
        self.directory_entry.grid(column=1, row=0, sticky=W + E)
        self.directory_entry.bind('<Return>', lambda arg: self.do_preview())

        self.directory_button = Button(self.directory_frame, text="Browse")
        self.directory_button["command"] = lambda: do_ask_directory(self.cfg.directory)
        self.directory_button.grid(column=2, row=0)

        self.glob_label = Label(self.directory_frame, text="Glob:")
        self.glob_label.grid(column=0, row=1, stick=E)

        self.glob_entry = Entry(self.directory_frame, textvariable=self.cfg.glob)
        self.glob_entry.bind('<Return>', lambda arg: self.do_preview())
        self.glob_entry.grid(column=1, row=1, sticky=N + S + W)

        self.search_replace_frame = Frame(self)
        self.search_replace_frame.grid_columnconfigure(1, weight=1)
        self.search_replace_frame.pack(anchor=N, side=TOP, fill=X, expand=0, padx=4, pady=4)

        self.search_label = Label(self.search_replace_frame, text="Search:")
        self.search_label.grid(column=0, row=0, sticky=E)
        self.search_entry = Entry(self.search_replace_frame, textvariable=self.cfg.search)
        self.search_entry.grid(column=1, row=0, sticky=N + S + W + E)
        self.search_entry.bind('<Return>', lambda arg: self.do_preview())

        self.replace_label = Label(self.search_replace_frame, text="Replace:")
        self.replace_label.grid(column=0, row=1, sticky=E)
        self.replace_entry = Entry(self.search_replace_frame, textvariable=self.cfg.replace)
        self.replace_entry.grid(column=1, row=1, sticky=N + S + W + E)
        self.replace_entry.bind('<Return>', lambda arg: self.do_preview())

        self.option_frame = Frame(self)
        self.option_frame.pack(side=TOP, fill=X, expand=0, pady=4)

        self.work_frame = LabelFrame(self.option_frame, text="Options")
        self.work_frame.pack(side=LEFT, padx=4, pady=4)

        self.ignore_case_checkbutton = Checkbutton(
            self.work_frame, text="ignore case", variable=self.cfg.ignore_case)
        self.ignore_case_checkbutton.pack(side=TOP, anchor=W, expand=0)

        self.create_backups_checkbutton = Checkbutton(
            self.work_frame, text="create backups", variable=self.cfg.create_backups)
        self.create_backups_checkbutton.pack(side=TOP, anchor=W, expand=0)

        self.preview_frame = LabelFrame(self.option_frame, text="Preview")
        self.preview_frame.pack(side=LEFT, padx=4, pady=4)
        self.show_full_content_checkbutton = Checkbutton(
            self.preview_frame, text="show full content", variable=self.cfg.show_full_content)
        self.show_full_content_checkbutton.pack(side=TOP, anchor=W, expand=0)

        self.show_original_checkbutton = Checkbutton(
            self.preview_frame, text="show original", variable=self.cfg.show_original)
        self.show_original_checkbutton.pack(side=TOP, anchor=W, expand=0)

        self.text_frame = Frame(self)
        self.text_frame.pack(side=TOP, fill=BOTH, expand=1, pady=4)

        self.text_frame.grid_columnconfigure(0, weight=1)
        self.text_frame.grid_rowconfigure(0, weight=1)

        self.scrollbar = Scrollbar(self.text_frame)
        self.scrollbar.grid(column=1, row=0, sticky=N + S)

        self.text = Text(self.text_frame, yscrollcommand=self.scrollbar.set, width=120, height=20)

        self.text.tag_config("file", background="lightgray", foreground="black")
        self.text.tag_config("search", background="lightblue", foreground="black")
        self.text.tag_config("replace", background="orange", foreground="black")
        self.text.tag_config("hollow", foreground="gray")
        self.text.tag_config("highlight", background="lightyellow")

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

    def do_preview(self):
        directory = self.cfg.directory.get()

        self.text.config(state=NORMAL)
        self.text.delete("1.0", END)
        self.text.config(state=DISABLED)

        self.files_with_content = []
        for path, dirs, files in os.walk(directory):
            for fname in files:
                filename = os.path.join(path, fname)

                if not self.cfg.glob.get() or fnmatch.fnmatch(fname.lower(), self.cfg.glob.get().lower()):
                    with open(filename, 'rt', encoding='latin-1') as fin:
                        lines = fin.read().splitlines()
                    lines = minised_on_lines(lines, self.cfg.search.get(), self.cfg.replace.get(),
                                             self.cfg.ignore_case.get())
                    self.files_with_content.append((filename, lines))

                    self.text.config(state=NORMAL)
                    self.text.insert(END, "%s:\n" % filename, "file")
                    for line in lines:
                        if isinstance(line, tuple):
                            if self.cfg.show_original.get():
                                self.text.insert(END, "%s" % line[0], "hollow")
                                self.text.insert(END, "%s" % line[1], "search")
                                self.text.insert(END, "%s\n" % line[3], "hollow")

                            self.text.insert(END, "%s" % line[0], "highlight")
                            self.text.insert(END, "%s" % line[2], "replace")
                            self.text.insert(END, "%s" % line[3], "highlight")
                            self.text.insert(END, "\n")
                        elif self.cfg.show_full_content.get():
                            self.text.insert(END, "%s\n" % line)
                    self.text.insert(END, "\n")
                    self.text.config(state=DISABLED)

                    self.master.update()
        self.run_btn["state"] = 'normal'

    def do_execute(self):
        for filename, lines in self.files_with_content:
            if self.cfg.create_backups.get():
                os.rename(filename, filename + "~")

            with open(filename, 'wt', encoding='latin-1', newline='\r\n') as fout:
                for line in lines:
                    if isinstance(line, tuple):
                        fout.write(line[0])
                        fout.write(line[2])
                        fout.write(line[3])
                    else:
                        fout.write(line)
                    fout.write("\n")

        self.run_btn["state"] = 'disabled'
        tkinter.messagebox.showinfo("Replacement successful",
                                    "Replacements have been performed successfully on the files")


def main():
    parser = argparse.ArgumentParser(description="A minimal sed-like tool")
    parser.add_argument('DIRECTORY', action='store', nargs='?',
                        help="directory to process")
    parser.add_argument("-i", "--in-place", action='store_true', default=False,
                        help="modify files in place")
    parser.add_argument("-o", "--output", metavar="FILE", type=str, default=None,
                        help="write output to FILE")
    parser.add_argument("-n", "--dry-run", action='store_true', default=False,
                        help="only show modifications, without actually modifying anything")
    parser.add_argument("-s", "--search", metavar="PAT", type=str,
                        help="the search pattern expression")
    parser.add_argument("-r", "--replace", metavar="REPL", type=str,
                        help="the replacement  expression, if not given just print the match")
    parser.add_argument("-R", "--recursive", metavar="GLOB", type=str,
                        help="interprets the FILE argument as perform replacement in all files matching GLOB")
    parser.add_argument("-I", "--ignore-case", action='store_true', default=False,
                        help="ignore case")
    parser.add_argument("-v", "--verbose", action='store_true', default=False,
                        help="display the replacements are performed")
    args = parser.parse_args()

    root = tkinter.Tk()
    root.wm_title("minised")
    root.minsize(600, 400)
    # root.geometry("800x500+0+0")

    app = MiniSedGUI(master=root)

    # apply command line parameters
    if args.DIRECTORY is not None:
        app.cfg.directory.set(args.DIRECTORY)
    else:
        app.cfg.directory.set(os.getcwd())

    if args.recursive is not None:
        app.cfg.glob.set(args.recursive)
    else:
        app.cfg.glob.set("*.txt")

    if args.search is not None:
        app.cfg.search.set(args.search)
    if args.replace is not None:
        app.cfg.replace.set(args.replace)

    if args.ignore_case:
        app.cfg.ignore_case.set(args.ignore_case)

    # run the thing
    app.mainloop()
    # root.destroy()


if __name__ == "__main__":
    main()


# EOF #
