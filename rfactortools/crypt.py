# rFactor encryption and decryption tool
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

import struct
import os

import rfactorcrypt

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


def encrypt_data(data, key, sign, skip):
    data = decrypt_data(data, skip)
    return rfactorcrypt.encrypt(data, key, sign, skip)


def decrypt_data(data, skip):
    sign, key = crypt_info(data)
    while games.get(sign):
        data = rfactorcrypt.decrypt(data, skip)
        sign, key = crypt_info(data)
    return data


def encrypt_file(input, output, key=1, sign=0x4b1dca9f960524e8):
    skip = get_skip(input)
    with open(input, 'rb') as fin:
        data = fin.read()

    data = decrypt_data(data, skip)

    encrypted_data = encrypt_data(data, key, sign, skip)

    with open(output, 'wb') as fout:
        fout.write(encrypted_data)


def decrypt_file(input, output):
    skip = get_skip(input)
    with open(input, 'rb') as fin:
        data = fin.read()

    data = decrypt_data(data, skip)

    with open(output, 'wb') as fout:
        fout.write(data)


def crypt_info(data):
    if len(data) < 16:
        return 0, 0
    else:
        sign, key = struct.unpack("<QQ", data[0:16])
        sign ^= key
        return sign, key


def crypt_info_from_file(filename):
    with open(filename, 'rb') as fin:
        return crypt_info(fin.read(16))


def get_skip(filename):
    if os.path.splitext(filename)[1].lower() == ".gmt":
        return 4
    else:
        return 0

# EOF #
