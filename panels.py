#!/usr/bin/python

import sys
import Image

def pixel_matches(pixel_colour, colour, tolerance):
    diff = (pixel_colour[0] - colour[0],
            pixel_colour[1] - colour[1],
            pixel_colour[2] - colour[2])
    if abs(diff[0]) + abs(diff[1]) + abs(diff[2]) > 3 * tolerance:
        return False
    return True



def walk_boundary(pixels, x, y, colour, tolerance, width, height, aura):
    (minx, maxx) = (x, x)
    (miny, maxy) = (y, y)
    seen = {(x, y): True}
    #area = {(x, y): True}
    loose_ends = {(x, y): True}
    while loose_ends:
        new_loose_ends = {}
        for (x, y) in loose_ends.keys():
            for i in range(-aura, aura + 1):
                for j in range(-aura, aura + 1):
                    # our loose end is already known
                    if i == 0 and j == 0:
                        continue
                    """# also only consider well connected areas
                    if i != 0 and j != 0:
                        continue"""
                    (px, py) = (x + i, y + j)
                    # is it outside the range?
                    if px < 0 or py < 0 or px >= width or py >= height:
                        continue
                    # is it a point we already saw?
                    if (px, py) in seen:
                        continue
                    # else we've seen it now
                    seen[(px, py)] = True
                    # is it outside the colour range we want?
                    if not pixel_matches(pixels[px, py], colour, tolerance):
                        continue
                    # else it is
                    #area[(px, py)] = True
                    minx = min(minx, px)
                    maxx = max(maxx, px)
                    miny = min(miny, py)
                    maxy = max(maxy, py)
                    new_loose_ends[(px, py)] = True

        loose_ends = new_loose_ends
    return None, (minx, miny, maxx, maxy)

def scan_for_panels(img, pixels):
    panel_boxes = []
    (width, height) = img.size
    border_colour = (0, 0, 0)
    tolerance = 150
    panel_scan_points = [((50, 0), (0, 1))]
    while panel_scan_points:
        new_panel_scan_points = []
        for ((x, y), (dx, dy)) in panel_scan_points:
            print 'scanning in %s direction from %s' % ((dx, dy), (x, y))
            for i in range(0, 1000):
                (px, py) = (x + i * dx, y + i * dy)
                if px >= width or py >= height:
                    break

                if not pixel_matches(pixels[px, py], border_colour, tolerance):
                    continue

                already_covered = False
                for (minx, miny, maxx, maxy) in panel_boxes:
                    if px >= minx and px <= maxx and \
                       py >= miny and py <= maxy:
                        already_covered = True
                        break
                # already had this in another panel?
                if already_covered:
                    continue

                print "think %s is a new border pixel" % ((px, py),)
                stuff = walk_boundary(pixels, px, py, border_colour, tolerance, width, height, 3)
                (minx, miny, maxx, maxy) = stuff[1]
                if maxx - minx < 20 or maxy - miny < 20:
                    print "nope, wasn't"
                    continue

                print "yep, got box %s" % ((minx, miny, maxx, maxy),)

                panel_boxes.append((minx, miny, maxx, maxy))
                # add bottom and right edges as new scan points
                new_panel_scan_points.append(((maxx + 1, miny + 3), (1, 0)))
                new_panel_scan_points.append(((minx + 3, maxy + 1), (0, 1)))
                break

        panel_scan_points = new_panel_scan_points

    return panel_boxes

PANELDIR = 'panels/'
filenames = sys.argv[1:]
for fn, filename in enumerate(filenames[:2]):
    img = Image.open(filename)
    print 'starting scan for %s...' % (filename,)
    pixels = img.load()
    panels = scan_for_panels(img, pixels)
    for i, panel in enumerate(panels):
        pimg = img.crop(panel)
        filename_sans_path = filename.split('/')[-1]
        pimg.save(PANELDIR + '%s_%d.jpg' % (filename_sans_path, i), quality=90)
    print 'done. %.3f%%' % (100.0 * fn / len(filenames),)
    print len(panels), panels
