#!/usr/bin/env python3

import sys
import os
import argparse
import struct

import rfactortools

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
    parser.add_argument('-k', '--key', default=1, type=lambda s: int(s, 0),
                        help="use key for encryption")
    parser.add_argument('-s', '--sign', default=0x4b1dca9f960524e8, type=lambda s: int(s, 0),
                        help="use signature for encryption")
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()

    for filename in args.FILE:
        sign, key = rfactortools.crypt_info_from_file(filename)
        print("sign:%016x key:%016x '%s' '%s'" % (sign, key, rfactortools.games.get(sign), filename))

        if args.encrypt:
            rfactortools.encrypt_file(filename, filename, args.key, args.sign)
        elif args.decrypt:
            rfactortools.decrypt_file(filename, filename)

# EOF #
