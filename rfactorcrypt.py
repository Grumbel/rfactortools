#!/usr/bin/env python3

import sys
import os
import argparse
import struct
import subprocess

import rfactortools
import rfactorcrypt

def rfactor_encrypt_legacy(filename, key):
    # not using .check_call() as rfactordec reports wrong exit codes
    subprocess.call(["./rfactordec", "-s", "4b1dca9f960524e8", "-e", "-o", filename, filename])

def rfactor_decrypt_legacy(filename):
    # not using .check_call() as rfactordec reports wrong exit codes
    subprocess.call(["./rfactordec", "-o", filename, filename])

def rfactor_encrypt(filename, key = 0):
    with open(filename, 'rb') as fin:
        encrypted_data = rfactorcrypt.encrypt(fin.read(), key, 0x4b1dca9f960524e8, rfactortools.get_skip(filename))
    with open(filename, 'wb') as fout:
        fout.write(encrypted_data)

def rfactor_decrypt(filename):
    with open(filename, 'rb') as fin:
        decrypted_data = rfactorcrypt.decrypt(fin.read(), rfactortools.get_skip(filename))
    with open(filename, 'wb') as fout:
        fout.write(decrypted_data)

class store_int_from_hex(argparse.Action):
    def __call__(self, parser, namespace, values, option_string = None):
        setattr(namespace, self.dest, int(values, 0))
        return

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
    parser.add_argument('-l', '--legacy', action='store_true', default=False,
                        help="use legacy encryption")
    parser.add_argument('-k', '--key', default=0, type=lambda s: int(s, 0),
                        help="use key for encryption")
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()

    if args.legacy:
        encrypt = rfactor_encrypt_legacy
        decrypt = rfactor_decrypt_legacy
    else:
        encrypt = rfactor_encrypt
        decrypt = rfactor_decrypt

    for filename in args.FILE:
        sign, key = rfactortools.crypt_info_from_file(filename)
        print("sign:%016x key:%016x '%s' '%s'" % (sign, key, rfactortools.games.get(sign), filename))

        if args.encrypt:
            # decrypt file completely before reencryption
            while rfactortools.games.get(sign):
                decrypt(filename)
                sign, key = rfactortools.crypt_info_from_file(filename)
            encrypt(filename, args.key)
        elif args.decrypt:
            # decrypt file completely
            while rfactortools.games.get(sign):
                decrypt(filename)
                sign, key = rfactortools.crypt_info_from_file(filename)

# EOF #
