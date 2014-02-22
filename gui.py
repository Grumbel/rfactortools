#!/usr/bin/env python3

##  rFactor Tools GUI
##  Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
##
##  This program is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with this program.  If not, see <http://www.gnu.org/licenses/>.

from PyQt5 import QtGui, QtWidgets, QtCore
import sys
 
def on_button_click(*args):
    print(args)    
    app.quit()

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)

    window = QtWidgets.QMainWindow()
    window.setWindowTitle("rfactortool V0.1")

    groupbox = QtWidgets.QGroupBox()
    hbox = QtWidgets.QHBoxLayout()

    quit_button = QtWidgets.QPushButton("Quit")
    quit_button.clicked.connect(on_button_click)

    ok_button = QtWidgets.QPushButton("Ok")
    ok_button.clicked.connect(on_button_click)
    
    hbox.addWidget(quit_button)
    hbox.addWidget(ok_button)

    groupbox.setLayout(hbox)

    window.setCentralWidget(groupbox)
    window.show()

    sys.exit(app.exec_())

# EOF #
