#!/usr/bin/env python3

import sys
import os
import argparse
import struct
import subprocess

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

def rfactor_crypt_info(filename):
    with open(filename, 'rb') as fin:
        sign, key = struct.unpack("<QQ", fin.read(16))
        sign ^= key;
        return sign, key

def rfactor_decrypt(filename):
    # not using .check_call() as rfactordec reports wrong exit codes
    subprocess.call(["./rfactordec", "-o", filename, filename])

def rfactor_encrypt(filename):
    # not using .check_call() as rfactordec reports wrong exit codes
    subprocess.call(["./rfactordec", "-s", "4b1dca9f960524e8", "-e", "-o", filename, filename])

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor MAS packer')
    parser.add_argument('FILE', action='store', type=str, nargs='+',
                        help='.mas file to unpack')
    parser.add_argument('-i', '--info', action='store_true',
                        help="show file info")
    parser.add_argument('-e', '--encrypt', action='store_true',
                        help="encrypt files")
    parser.add_argument('-d', '--decrypt', action='store_true',
                        help="decrypt files")
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()

    for filename in args.FILE:
        sign, key = rfactor_crypt_info(filename)
        print("%016x %016x '%s' '%s'" % (sign, key, games.get(sign), filename))

        if args.encrypt:
            # decrypt file completely before reencryption
            while games.get(sign):
                rfactor_decrypt(filename)
                sign, key = rfactor_crypt_info(filename)
            rfactor_encrypt(filename)
        elif args.decrypt:
            # decrypt file completely
            while games.get(sign):
                rfactor_decrypt(filename)
                sign, key = rfactor_crypt_info(filename)

# EOF #
