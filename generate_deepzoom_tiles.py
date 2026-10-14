#!/usr/bin/env python2
# -*- coding: utf-8 -*-
import sys
import deepzoom

SOURCE = sys.argv[1]
DESTINATION = "tintin.dzi"

creator = deepzoom.ImageCreator(
    tile_size=128, tile_overlap=2, tile_format="jpg",
    image_quality=0.95, resize_filter="bicubic")

creator.create(SOURCE, DESTINATION)
