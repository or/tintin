#!/usr/bin/python

import os
import sys
import Image

max_width = 200
max_height = 200

in_dir = './good_panels'
out_dir = './good_panels/small'

for (where, bah, files) in os.walk(in_dir):
    if where != in_dir:
        continue
    for fn in files:
        if not fn.endswith('.jpg'):
            continue
        img = Image.open(in_dir + '/' + fn)
        (width, height) = img.size
        if width > max_width or height > max_height:
            continue
            if width >= height:
                new_width = max_width
                new_height = height * new_width / width
            else:
                new_height = max_height
                new_width = width * new_height / height
            new_img = img.resize((new_width, new_height), Image.ANTIALIAS)
        else:
            new_img = img

        out_fn = out_dir + '/' + fn.replace('.jpg', '') + '.bmp'
        new_img.save(out_fn)
        print out_fn
