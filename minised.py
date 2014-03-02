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

import argparse
import sys
import re


def minised(lines, pattern, replacement, ignore_case):
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

            replacement = m.expand(args.replace)

            print("- %s%s%s%s%s" % (left, "→{", middle, "}←", right))
            print("+ %s%s%s%s%s" % (left, "→{", replacement, "}←", right))
            print()

            result.append(left + replacement + right)
        else:
            result.append(line)

    return result

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="A minimal sed-like tool")
    parser.add_argument('FILE', action='store', nargs='?',
                        help="files to process")
    parser.add_argument("-i", "--in-place", action='store_true', default=False,
                        help="modify files in place")
    parser.add_argument("-o", "--output", metavar="FILE", type=str, default=None,
                        help="write output to FILE")
    parser.add_argument("-n", "--dry-run", action='store_true', default=False,
                        help="only show modifications, without actually modifying anything")
    parser.add_argument("-p", "--pattern", metavar="PAT", type=str, required=True,
                        help="the search pattern expression")
    parser.add_argument("-r", "--replace", metavar="REPL", type=str,
                        help="the replacement  expression")
    parser.add_argument("-I", "--ignore-case", action='store_true', default=False,
                        help="ignore case")
    parser.add_argument("-v", "--verbose", action='store_true', default=False,
                        help="display the replacements are performed")
    args = parser.parse_args()

    if args.FILE:
        with open(args.FILE, 'rt', encoding='latin-1') as fin:
            lines = fin.read().splitlines()
    else:
        lines = sys.stdin.read().splitlines()

    output = minised(lines, args.pattern, args.replace, args.ignore_case)

    if not args.dry_run:
        if args.output:
            outfile = args.output
        elif args.in_place:
            outfile = args.FILE
        else:
            outfile = None

        if outfile:
            with open(outfile, 'wt', newline='\r\n', encoding='latin-1', errors="replace") as fout:
                for line in output:
                    fout.write(line)
                    fout.write("\n")
        else:
            for line in output:
                print(line)

# EOF #
