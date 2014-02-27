#!/usr/bin/env python3

import argparse

import rfactortools

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor to GSC2013 converter')
    parser.add_argument('DIRECTORY', action='store', type=str, nargs='+',
                        help='directory containing the mod')
    parser.add_argument('-o', '--output', metavar='DIR', type=str, required=True,
                        help="output directory")
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help="be more verbose")
    args = parser.parse_args()

    for directory in args.DIRECTORY:
        rfactortools.rfactor_to_gsc2013(directory, args.output)

    print("-- rfactor-to-gsc2013 conversion complete --")

# EOF #
