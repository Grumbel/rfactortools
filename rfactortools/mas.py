##  rFactor .gen file manipulation tool
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

from concurrent.futures import ThreadPoolExecutor
import struct
import os
import zlib

mas_type0 = b"GMOTORMAS10\0\0\0\0\0"
mas_type1 = b"\xC8\xCF\xD2\xD8\xCE\xD8\xE6\xC9\xCA\xDD\xD8\xBE\xBB\xA6\xBF\x90"
mas_type3 = b"CUBEMAS4.10\0\0\0\0\0"

class MASFileType:
    UNKNOWN = 0
    MISC = 16 # BIK, GFX, PSD, PSH, RTF, TXT, VSH
    GMT  = 17
    BMP  = 18
    SCN  = 19
    TGA  = 20
    PNG  = 21
    JPG  = 22
    # DDS  = 23 # FIXME: might need to inspect file content to tell the difference
    DDS  = 55

class MASFileEntry:
    def __init__(self, *args):
        self.type, self.flags, self.name, self.offset, self.size, self.zsize = args

def get_mas_type(signature):
    if signature == mas_type0:
        return 0
    elif signature == mas_type1:
        return 1
    elif signature == mas_type3:
        return 3
    else:
        raise RuntimeError("unknown mas type: %s" % signature)

def get_file_type(filename):
    ext = os.path.splitext(filename)[1].lower()

    if ext == ".bik" or  ext == ".gfx" or ext == ".psd"  or ext == ".rtf" or ext == ".txt" or ext == ".vsh":
        return MASFileType.MISC
    elif ext == ".gmt":
        return MASFileType.GMT
    elif ext == ".bmp":
        return MASFileType.BMP
    elif ext == ".scn":
        return MASFileType.SCN
    elif ext == ".tga":
        return MASFileType.TGA
    elif ext == ".png":
        return MASFileType.PNG
    elif ext == ".jpg":
        return MASFileType.JPG
    elif ext == ".dds":
        return MASFileType.DDS # FIXME: need to handle the other DDS
    else:
        print("%s: warning: unknown file type" % filename)
        return MASFileType.UNKNOWN

### MAS file packing
def mas_pack_from_data(files, masfile, mas_type = 1):
    """files is a (filename, data) tuple"""
    with open(masfile, "wb") as fout:
        if mas_type == 0:
            fout.write(mas_type0)
        elif mas_type == 1:
            fout.write(mas_type1)
        elif mas_type == 2:
            fout.write(mas_type2)
        elif mas_type == 3:
            fout.write(mas_type3)
        else:
            raise RuntimeError("invalid mas_type: %s" % mas_type)

        if mas_type == 1:
            base_offset = 28 + len(files) * 256
        else:
            base_offset = 24 + len(files) * 256

        file_table = []
        offset = 0
        fout.seek(base_offset)

        compressed_files = []
        with ThreadPoolExecutor(max_workers=8) as executor:
            for name, data in files:
                print("compressing %s" % name)
                compressed_files.append((name, data, executor.submit(zlib.compress, data)))
        compressed_files = [ (name, data, deflated_data.result()) for name, data, deflated_data in compressed_files ]

        for name, data, deflated_data in compressed_files:
            print("packing %s" % name)
            file_type = get_file_type(name)
            flags = 0

            # deflated_data = zlib.compress(data)

            file_table.append((file_type, flags, name, offset, len(data), len(deflated_data)))
            fout.write(deflated_data)
            offset += len(deflated_data)

        data_size = offset
        if mas_type == 1:
            fout.seek(20)
        else:
            fout.seek(16)
        fout.write(struct.pack("<ll", len(file_table), data_size))

        # write file table to the start of the file
        if mas_type == 1:
            fout.seek(28)
        else:
            fout.seek(24)

        for file_type, flags, name, offset, size, zsize in file_table:
            print("%8d %8d %8d %s" % (offset, size, zsize, name))

            name_bytes = name.encode("latin-1", "replace")

            if mas_type == 0:
                fout.write(struct.pack("<4xlll240s", offset, size, zsize, name_bytes))
            elif mas_type == 1:
                fout.write(struct.pack("<BBxx236slll4x", file_type, flags, name_bytes, offset, size, zsize))
            elif mas_type == 2:
                fout.write(struct.pack("<4x16slll4x", name_bytes, offset, size, zsize))
            elif mas_type == 3:
                fout.write(struct.pack("<4xlll4x236s", name_bytes, offset, size, zsize))
            else:
                raise RuntimeError("invalid map_type")


def mas_pack(files, masfile, mas_type):
    files_with_data = []

    for filename in files:
        print("reading %s" % filename)
        name = os.path.basename(filename)

        with open(filename, "rb") as fin:
            data = fin.read()
            files_with_data.append((name, data))

    mas_pack_from_data(files_with_data, masfile, mas_type)

### MAS file unpacking and listing
def mas_list(masfile, verbose=False, with_filename=False):
    with open(masfile, "rb") as fin:
        file_table = mas_unpack_file_table(fin)

    if verbose:
        print("%6s %6s %-8s %-8s %-8s %-8s" % ("flags:", "type:", "offset:", "size:", "zsize:", "name:"))
        for entry in file_table:
            print("%6x %6d %8d %8d %8d %s" % (entry.flags, entry.type, entry.offset, entry.size, entry.zsize, entry.name))

        print()
        print("number of files:       %12d" % len(file_table))
        # print("header file_count:     %12d" % file_count)
        print()
        print("total extracted size:  %12d" % sum([e.size for e in file_table]))
        print()
        print("total compressed size: %12d" % sum([e.zsize for e in file_table]))
        # print("header data_size:      %12d" % data_size)
    else:
        for entry in file_table:
            if with_filename:
                print("%s: %s" %  (masfile, entry.name))
            else:
                print(entry.name)

def mas_unpack(masfile, outdir, verbose=False):
    with open(masfile, "rb") as fin:
        file_table = mas_unpack_file_table(fin)

        os.mkdir(outdir)
        for entry in file_table:
            fin.seek(entry.offset)
            data = fin.read(entry.zsize)

            outfile = os.path.join(outdir, entry.name)
            print("%8d %8d %8d %s" % (entry.offset, entry.size, entry.zsize, outfile))
            with open(outfile, "wb") as fout:
                if entry.size != entry.zsize:
                    inflated_data = zlib.decompress(data)
                else:
                    inflated_data = data

                if len(inflated_data) != entry.size:
                    raise RuntimeError("invalid inflated size %d for %s should be %d" % (len(inflated_data), entry.name, entry.size))
                fout.write(inflated_data)

def mas_unpack_to_data(masfile):
    with open(masfile, "rb") as fin:
        file_table = mas_unpack_file_table(fin)

        results = []

        for entry in file_table:
            fin.seek(entry.offset)
            data = fin.read(entry.zsize)

            print("%8d %8d %8d %s" % (entry.offset, entry.size, entry.zsize, entry.name))

            if entry.size != entry.zsize:
                inflated_data = zlib.decompress(data)
            else:
                inflated_data = data
            
            if len(inflated_data) != entry.size:
                raise RuntimeError("invalid inflated size %d for %s should be %d" % (len(inflated_data), entry.name, entry.size))
            
            results.append((entry.name, inflated_data))

        return results

def mas_unpack_file_table(fin):
    signature = fin.read(16)

    mas_type = get_mas_type(signature)

    if mas_type == 1:
        file_count, data_size = struct.unpack("<4xll", fin.read(12))
    else:
        file_count, data_size = struct.unpack("<ll", fin.read(8))

    file_table = []
    for i in range(0, file_count):
        if mas_type == 0:
            offset, size, zsize, name = struct.unpack("<4xlll240s", fin.read(256))
        elif mas_type == 1:
            entry = MASFileEntry(*struct.unpack("<BBxx236slll4x", fin.read(256)))
        elif mas_type == 2:
            name, offset, size, zsize = struct.unpack("<4x16slll4x", fin.read(256))
        elif mas_type == 3:
            name, offset, size, zsize = struct.unpack("<4xlll4x236s", fin.read(256))
        else:
            raise RuntimeError("invalid map_type")

        # No support for ASCIZ strings struct.unpack, thus ugly hackery
        entry.name = entry.name.split(b'\0', 1)[0].decode('latin-1')

        file_table.append(entry)

    # base_offset = 28 + file_count * 256
    base_offset = fin.tell()

    for entry in file_table:
        entry.offset += base_offset

    return file_table

# EOF #
