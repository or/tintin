#!/usr/bin/env python3

import sys

from PIL import Image

PANELDIR = 'panels/'

for line in open(sys.argv[1], 'r'):
    [filename, box_str] = line.split(':', 1)
    boxes = eval(box_str)
    img = Image.open(filename)
    for i, box in enumerate(boxes):
        print(box)
        newimg = img.crop(box)
        filename_sans_path = filename.split('/')[-1]
        newimg.save(PANELDIR + "%s_%d.jpg" % (filename_sans_path, i), quality=95)

