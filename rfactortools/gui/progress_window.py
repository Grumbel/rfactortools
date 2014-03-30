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
    Button, Frame, Scrollbar, Text, Toplevel, \
    N, S, W, E, END, DISABLED
import queue
import tkinter

from .converter_thread import ConverterThread


class ProgressWindow(Toplevel):

    def __init__(self, parent, source_directory, target_directory, cfg):
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
        self.converter_thread = ConverterThread(self, source_directory, target_directory, cfg)
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

    def finish(self):
        self.converter_thread.cancel()
        self.converter_thread.join()
        self.destroy()

    def update_progress(self, *args):
        msg, *args = args

        if msg == "start":
            self.conversion_had_errors = False

        elif msg == "finished":
            self.cancel_btn["text"] = "Finish"
            self.cancel_btn["command"] = self.finish
            self.cancel_btn.config(state=tkinter.NORMAL)

            self.text.config(state=tkinter.NORMAL)
            self.text.insert(END, "_" * 100 + "\n\n")
            if not self.conversion_had_errors:
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

            txt = "%s: %-70s" % (modname, filename + "...")

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


# EOF #
