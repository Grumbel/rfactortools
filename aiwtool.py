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
import re
import cairo
import math

# [Waypoint]
# trackstate=4543
# drivinglines=1
# autogengrid=(-1,9)
# autogenaltgrid=(1,0)
# teleportwp=(438)
# pitlanepaths=(11,11)
# times=(67.7990,340282346638528860000000000000000000000.0000)
# multilinetimes=(139.2194,340282346638528860000000000000000000000.0000)
# number_waypoints=3672
# lap_length=18169.6
# sector_1_length=17893.38
# sector_2_length=17937.1
# LeftHandedPits=1
# FuelUse=37171.0
# AIBrakingStiffness=(1.0000,1.0000,0.9000)
# SpeedOffset=0.000000
# DelayPitCrewLoad=0
# OutsideAdjustment=-0.500000
# InsideAdjustment=-1.000000
# GrooveWidth=0
# GrooveHeightOffset=0.01
# PaceCarReleaseDist=(0.8000, 0.8700)
# GrooveWidthWet=4.000000
# IntermediateFogLevel=(0.1250)        // Combined wetness factor where intermediate values are used (sunny fog values are in SCN file!)
# IntermediateFogPlanes=(175.0,675.0)  // Fog planes at intermediate wetness
# RainyFogPlanes=(0.0,150.0)           // Fog planes at full monsoon
# IntermediateFogColor=(191.3,191.3,191.3)  // Fog color at intermediate wetness (RGB, 0-255 each)
# RainyFogColor=(127.5,127.5,127.5)    // Fog color at full monsoon (RGB, 0-255 each)
# FogDensity=(0.00125,0.01000)         // Fog density only works on some video cards (intermediate, monsoon)
# RainyDarkness=(0.50000,0.00000)      // Overcast light value, monsoon light value
# WorstTime=(0.0000)
# MidTime=(0.0000)
# BestTime=(0.0000)
# WorstAdjust=(0.8000)
# MidAdjust=(1.0000)
# BestAdjust=(1.2000)
# CheatDelta=(0.0000,0.0000,0.0000)
# AIRange=(0.1000)

keyvalue_regex = re.compile(r'^\s*([^=]+)\s*=\s*(.*)\s*')
vec3_regex = re.compile(r'\((-?\d*(\.\d*)?),\s*(-?\d*(\.\d*)?),\s*(-?\d*(\.\d*)?)\)')

def vec3(str):
    m = vec3_regex.match(str)
    if m:
        x = float(m.group(1))
        y = float(m.group(3))
        z = float(m.group(5))
        return (x, y, z)
    else:
        raise Exception("not a vec3: \"%s\"" % str)

class AIW:
    def __init__(self):
        self.waypoints = []

    def get_bounding_box(self):
        x, y, z = self.waypoints[0].pos
        x1 = x
        y1 = z
        x2 = x
        y2 = z
        for w in self.waypoints[1:]:
            x, y, z = w.pos
            x1 = min(x1, x)
            y1 = min(y1, z)
            x2 = max(x2, x)
            y2 = max(y2, z)

        return x1, y1, x2, y2

class Waypoint:
    # wp_pos=(1615.28,115.8291,1235.497)
    # wp_perp=(-0.7424909,0,0.669856)
    # wp_normal=(0.003270742,0.999988,0.003625401)
    # wp_vect=(-2.192505,0.01757813,-2.429077)
    # wp_width=(12.5964,13.3416,25.1928,26.6832)
    # wp_dwidth=(25.1928,26.6832,0,0)
    # wp_path=(0,0.0)
    # wp_lockedAlpha=(0)
    # wp_galpha=(0)
    # wp_groove_lat=(0.000000)
    # wp_test_speed=(-1.0)
    # wp_score=(2,17900.64)
    # wp_cheat=(-1.0)
    # wp_pathabstractionspeed=(0.0000)
    # wp_pathabstraction=(0,-1)
    # wp_wpse=(0,0)
    # wp_branchID=(0)
    # wp_bitfields=(0)
    # wp_lockedLats=(1)
    # wp_multipathlat=(0.0, 0.0)
    # wp_translat=(0.0000, 0.0000)
    # wp_pitlane=(1)
    # WP_PTRS=(3632,1,-1,0)

    def __init__(self):
        self.pos = None
        self.bitfields = None
        self.branch_id = None

def parse_aiw(filename):
    aiw = AIW()

    with open(filename, "rt", encoding="latin-1", errors="replace") as fout:
        waypoints = []
        parse_waypoints = False

        waypoint = Waypoint()
        for line in fout.read().splitlines():
            if line == "[Waypoint]":
                parse_waypoints = True

            if parse_waypoints:
                m = keyvalue_regex.match(line)
                if m:
                    key, value = m.group(1), m.group(2)
                    if key == "wp_pos":
                        waypoint = Waypoint()
                        aiw.waypoints.append(waypoint)
                        waypoint.pos = vec3(value)
                    elif key == "wp_bitfields":
                        waypoint.bitfields = value
                    elif key == "wp_branchID":
                        waypoint.branch_id = value
                    else:
                        pass # print("unhandled: \"%s\"" % key)

    aiw.waypoints = list(filter(lambda w: w.branch_id == "(0)", aiw.waypoints))

    return aiw

def point_distance(p1, p2):
    return math.sqrt((p2[0] - p1[0])**2 + (p2[1] - p1[1])**2)

def print_aiw(aiw):
    width, height = 512, 512
    surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
    cr = cairo.Context(surface)

    x1, y1, x2, y2 = aiw.get_bounding_box()

    cx = (x2 + x1) / 2
    cy = (y2 + y1) / 2

    w = (x2 - x1)
    h = (y2 - y1)

    sx = width / w
    sy = height / h

    scale = min(sx, sy) * 0.9

    cr.save()
    cr.translate(width/2, height/2)

    cr.translate((-x1 - w/2)*scale,
                 (-y1 - h/2)*scale)

    x, y, z = aiw.waypoints[0].pos
    cr.move_to(x * scale, z * scale)
    for w in aiw.waypoints[1:]:
        x, y, z = w.pos
        cr.line_to(x * scale, z * scale)

    # fudge factor to find hillclimb tracks that don't close
    d = point_distance(aiw.waypoints[0].pos, aiw.waypoints[-1].pos)
    if d < 200:
        cr.close_path()

    cr.set_line_width(12.0)
    cr.set_source_rgb(0, 0, 0)
    cr.stroke_preserve()

    cr.set_source_rgb(255, 255, 255)
    cr.set_line_width(4.0)
    cr.stroke()

    cr.restore()

    surface.write_to_png("/tmp/test.png")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='rFactor Tools .aiw processor')
    parser.add_argument('FILE', action='store', type=str,
                        help='.aiw file to process')
    args = parser.parse_args()

    aiw = parse_aiw(args.FILE)
    print_aiw(aiw)

# EOF #
