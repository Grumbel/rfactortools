#!/usr/bin/env python3

import os
import argparse
import hashlib
import functools
import shutil
import filecmp

def hashfile(afile, hasher, blocksize=65536):
    buf = afile.read(blocksize)
    while len(buf) > 0:
        hasher.update(buf)
        buf = afile.read(blocksize)
        return hasher.digest()

@functools.total_ordering
class FileInfo:
    def __init__(self, md5sum, filename):
        self.md5sum   = md5sum
        self.filename = os.path.normpath(filename)

    def __eq__(self, other):
        # return (self.md5sum, self.filename) == (other.md5sum, other.filename)
        return self.filename == other.filename
        
    def __lt__(self, other):
        # return (self.md5sum, self.filename) < (other.md5sum, other.filename)
        return self.filename < other.filename

    def __hash__(self):
        return hash(self.filename)

    def __repr__(self):
        return "FileInfo(%s, %s)" % (repr(self.md5sum), repr(self.filename))

    def __str__(self):
        return self.filename

def fileinfo_from_path(path):
    if os.path.isdir(path):
        return fileinfo_from_directory(path)
    elif os.path.isfile(path):
        return fileinfo_from_md5sums(path)
    else:
        raise Exception("%s: error: unknown file type" % path)

def fileinfo_from_md5sums(filename):
    with open(filename, "r") as fin:
        return [FileInfo(*e.split(None, 1)) for e in fin.read().splitlines()]
        
def fileinfo_from_directory(directory):
    lst = []
    for path, dirs, files in os.walk(directory):
        for fname in files:
            lst.append(FileInfo(None, os.path.relpath(os.path.join(path, fname), directory)))
    return lst

def compare_directories(finfo1, finfo2):
    finfo1_set = set(finfo1)
    finfo2_set = set(finfo2)

    return (
        sorted(finfo1_set.difference(finfo2_set)), # removals
        sorted(finfo2_set.difference(finfo1_set)), # additions
        [] # changes
    )
        
def compare_command(path1, path2):
    finfo1 = fileinfo_from_path(path1)
    finfo2 = fileinfo_from_path(path2)

    removals, additions, changes = compare_directories(finfo1, finfo2)

    for f in removals:
        print("-%s" % f)

    for f in additions:
        print("+%s" % f)

    for f in changes:
        print("~%s" % f)

def extract_command(file1, file2, target):
    pass

class VFS:
    def remove_file(filename):
        pass

    def move_file(source, target):
        pass

class LoggingVFS:
    def remove_file(filename):
        print("removing %s" % filename)

    def move_file(source, target):
        print("moving %s to %s" % (source, target))

def merge_command(source, target, dry_run, force):
    if not os.path.isdir(source):
        raise Exception("%s: must be a directory" % source)
    if not os.path.isdir(target):
        raise Exception("%s: must be a directory" % target)

    dir_lst = []
    file_lst = []
    for path, dirs, files in os.walk(source):
        for fname in dirs:
            dir_lst.append(os.path.relpath(os.path.join(path, fname), source))
        for fname in files:
            file_lst.append(os.path.relpath(os.path.join(path, fname), source))

    # build directory hierachy
    for path in dir_lst:
        target_dir = os.path.join(target, path)
        if os.path.exists(target_dir):
            if not os.path.isdir(target_dir):
                raise Exception("%s: error: already exist and is not a directory" % target_dir)
        else:
            print("mkdir %s" % target_dir)
            if not dry_run:
                os.mkdir(target_dir)

    # move files
    for path in file_lst:
        source_file = os.path.join(source, path)
        target_file = os.path.join(target, path)

        if not force and os.path.exists(target_file):
            if filecmp.cmp(source_file, target_file, shallow=False):
                print("%s: removing, already exist in target" % source_file)
                if not dry_run:
                    os.remove(source_file)
            else:
                print("%s: file already exist, not touching it" % target_file)
        else:
            print("moving %s to %s" % (source_file, target_file))
            if not dry_run:
                shutil.move(source_file, target_file)

    # cleanup old hierachy
    for path in reversed(dir_lst):
        source_dir = os.path.join(source, path)
        print("rmdir %s" % source_dir)
        if not dry_run:
            os.rmdir(source_dir)
    os.rmdir(source)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='dirtool')
    parser.add_argument('COMMAND', action='store', type=str,
                        help='diff, extract, merge')
    parser.add_argument('FILE1', action='store', type=str,
                        help='A directory')
    parser.add_argument('FILE2', action='store', type=str,
                        help='Another directory')
    parser.add_argument('-c', '--checksum', action='store_true', 
                        help="Use checksum for comparism")
    parser.add_argument('-t', '--target', metavar="DIR", action='store', 
                        help="Target directory for extract")
    parser.add_argument('-f', '--force', action='store_true', 
                        help="Overwrite files in target directory")
    parser.add_argument('-n', '--dry-run', action='store_true',
                        help="don't act, just show actions")
    args = parser.parse_args() 

    if args.COMMAND == "diff":
        compare_command(args.FILE1, args.FILE2)
    elif args.COMMAND == "extract":
        extract_command(args.FILE1, args.FILE2, args.target)
    elif args.COMMAND == "merge":
        merge_command(args.FILE1, args.FILE2, args.dry_run, args.force)
    else:
        raise Exception("unknown command: %s" % command)

# EOF #
