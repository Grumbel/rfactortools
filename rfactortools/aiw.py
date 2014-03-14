# rFactor .aiw file processing tool
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

import PIL.Image
import PIL.ImageDraw
import logging
import math
import os
import re
import tempfile

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
int1_regex = re.compile(r'\((-?\d+)\)')


def vec3(str):
    m = vec3_regex.match(str)
    if m:
        x = float(m.group(1))
        y = float(m.group(3))
        z = float(m.group(5))
        return (x, y, z)
    else:
        raise Exception("not a vec3: \"%s\"" % str)


def int1(str):
    m = int1_regex.match(str)
    if m:
        return int(m.group(1))
    else:
        raise Exception("not a int1: \"%s\"" % str)


class AIW:

    def __init__(self):
        self.waypoints = []

    def get_waypoints(self, branch_id=0):
        return list(filter(lambda w: w.branch_id == branch_id, self.waypoints))

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


def parse_aiwfile(filename):
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
                        waypoint.bitfields = int1(value)
                    elif key == "wp_branchID":
                        waypoint.branch_id = int1(value)
                    else:
                        pass  # logging.info("unhandled: \"%s\"" % key)

    return aiw


def point_distance(p1, p2):
    return math.sqrt((p2[0] - p1[0]) ** 2 + (p2[1] - p1[1]) ** 2)


def draw_path_cairo(cr, scale, waypoints):
    x, y, z = waypoints[0].pos
    cr.move_to(x * scale, z * scale)
    for w in waypoints[1:]:
        x, y, z = w.pos
        cr.line_to(x * scale, z * scale)

    # fudge factor to find hillclimb tracks that don't close
    d = point_distance(waypoints[0].pos, waypoints[-1].pos)
    if d < 200:
        cr.close_path()


def draw_path(draw, scale, waypoints, ofx, ofy):
    # fudge factor to find hillclimb tracks that don't close
    d = point_distance(waypoints[0].pos, waypoints[-1].pos)
    if d < 200:
        waypoints.append(waypoints[0])

    x, y, z = waypoints[0].pos
    points = []
    for w in waypoints:
        x, y, z = w.pos
        points.append((x * scale + ofx, z * scale + ofy))

    draw.line(points, width=12*2, fill="#000000")
    draw.line(points, width=4*2, fill="#ffffff")


def render_aiw(aiw, width=512, height=512):
    x1, y1, x2, y2 = aiw.get_bounding_box()

    cx = (x2 + x1) / 2
    cy = (y2 + y1) / 2

    w = (x2 - x1)
    h = (y2 - y1)

    sx = width / w
    sy = height / h

    # make the image a little smaller then requested so the edges
    # aren't clipped
    scale = min(sx, sy) * 0.9

    try:
        import cairo

        surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
        cr = cairo.Context(surface)

        cr.save()
        cr.translate(width / 2, height / 2)

        cr.translate((-x1 - w / 2) * scale,
                     (-y1 - h / 2) * scale)

        for i in reversed(range(0, 1)):
            draw_path_cairo(cr, scale, aiw.get_waypoints(i))

            cr.set_line_width(12.0)
            cr.set_source_rgb(0, 0, 0)
            cr.stroke()

        for i in reversed(range(0, 1)):
            draw_path_cairo(cr, scale, aiw.get_waypoints(i))
            if i == 0:
                cr.set_source_rgb(1, 1, 1)
            else:
                cr.set_source_rgb(0.75, 0.75, 0.75)
            cr.set_line_width(4.0)
            cr.stroke()

        cr.restore()

        # TODO: img.get_data() is not implemented in pycairo, thus we save
        # to .png, load it and save as .tga again
        tmpfile = tempfile.mkstemp(suffix=".png")[1]
        surface.write_to_png(tmpfile)
        pil_img = PIL.Image.open(tmpfile)
        os.unlink(tmpfile)

        return pil_img

    except ImportError as err:
        logging.info("Cairo not found, rendering with lower quality PIL: %s" % err)

        # render at 2x scale to get a bit of anti-aliasing, rendering
        # quality is still much worse then cairo
        img = PIL.Image.new("RGBA", (width*2, height*2))

        ofx = width  / 2 + (-x1 - w / 2) * scale
        ofy = height / 2 + (-y1 - h / 2) * scale

        draw = PIL.ImageDraw.Draw(img)
        draw_path(draw, scale*2.0, aiw.get_waypoints(0), ofx*2.0, ofy*2.0)

        img = img.resize((width, height), PIL.Image.ANTIALIAS)

        return img


# EOF #
