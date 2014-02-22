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

import struct

games = \
{
    0x38af5637e81bc9a0: 'rFactor',
    0x2eb8f5cc9b14ea3b: 'ARCA Sim Racing',
    0x6a9d37283a9f3d9f: 'Simulador Turismo Carretera',
    0xde4139f961fa2817: 'Top Race Simulador 2009',
    0x38af3150902cc55b: 'Superleague Formula',
    0x4b1dca9f960524e8: 'Game Stock Car',
    0x06a66ad328aeaed6: 'Simulador Turismo Carretera 2012',
    0x28b7856a3a5996da: 'Game Stock Car: Formula Truck'
}

def crypt_info(data):
    sign, key = struct.unpack("<QQ", data[0:16])
    sign ^= key;
    return sign, key

def crypt_info_from_file(filename):
    with open(filename, 'rb') as fin:
        return crypt_info(fin.read(16))

# EOF #
