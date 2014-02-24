#!/usr/bin/env python3

##  rFactor .scn/.gen file manipulation tool
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

import argparse
import os

from collections import defaultdict
import rfactortools

def process_vehfile(filename):
    veh = rfactortools.parse_vehfile(filename)
    return veh
   
class Tree(defaultdict):
    def __init__(self, value=None):
        super(Tree, self).__init__(Tree)
        self.value = value
 
def print_tree_rec(tree, indent = ""):
    for i, (k, v) in enumerate(sorted(tree.items())):
        if indent == "":
            sym = ""
        elif i < len(tree)-1:
            sym = "├ "
        else:
            sym = "└ "

        print("%s%s%s" % (indent, sym, k))
        print_tree_rec(v, indent + "  ")

def print_tree(vehs):
    tree = Tree()
    for veh in vehs:
        subtree = tree
        for cat in veh.category:
            subtree = subtree[cat]

    print_tree_rec(tree)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor .veh/.gen processor')
    parser.add_argument('FILE', action='store', type=str, nargs='+',
                        help='.veh file or directory containing .veh files')
    parser.add_argument('-t', '--tree', action='store_true', default=False,
                        help="print tree")
    args = parser.parse_args()

    files = []
    for path in args.FILE:
        if os.path.isdir(path):
            files += rfactortools.find_files(path, ".veh")
        else:
            files.append(path)
            
    vehs = [ process_vehfile(filename) for filename in files ]

    if args.tree:
        print_tree(vehs)
    else:
        for veh in vehs:
            print("    file:", veh.filename)
            print(" classes:", veh.classes)
            print("graphics:", veh.graphics_file)
            print("category:", veh.category)
            print()

# EOF #
