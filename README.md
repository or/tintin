Make a big mosaic of a Tintin image based on comic panels from all Tintin books.
It has these properties:
 * tiles of different sizes and aspect ratios
 * overlapping tiles
 * try to avoid reuse of tiles, especially in the same neighbourhood, so repetitions aren't too obvious
 * the colour distribution inside the tile is also matched to the original image, not just the average of the tile
 * the original image is not overlayed in any way, like it is done in some cheated mosaics ;)

The result is 15.600 x 16.400 pixels large, uses ca. 3.200 panels as tiles, each of which used ca. 2.07 times on average.
Demo: http://or.github.io/tintin

This was an old project for a friend of mine. Perhaps someone is interested in the algorithm or the result,
so I decided to upload it here. 

The code is old and some parts are pretty ugly. I only cleaned it up slightly before posting it here.

## Steps
 * `find_panel_boxes` gets the comic page scan images as inputs and outputs a file containing boxes for the panels it found:
```python
[...]
all/page_1.jpg: [[18, 20, 1079, 1075], [318, 1100, 525, 1433], [537, 1100, 772, 1432], [784, 1100, 1079, 1431], [18, 1101, 304, 1435], ]
all/page_2.jpg: [[267, 17, 621, 352], [26, 18, 254, 350], [633, 18, 1092, 351], [23, 378, 429, 712], [442, 378, 810, 711], [824, 378, 1089, 712], [723, 733, 1086, 1069], [426, 735, 710, 1070], [23, 73
[...]
```
 * `cut_panels.py` will take that file as input and cut out all the panels, store them in `./panels`
 * `filter_panels` looks at all panels in `./panels` and decides whether each panel is suitable as a tile for the mosaic,
   based on how much of it is white, therefore likely speech bubbles, all suitable panels are saved in `./good_panels`
 * `resize_panels.py` resizes all panels in `./good_panels` and stores the results in `./good_panels/small`, but nowadays this
   should be much easier and faster with https://github.com/ImageMagick/ImageMagick, for instance like this:
```bash
> (cd good_panels; ls -1 | grep '\.jpg' | perl -pe 's/(.*)/convert "\1" -resize 300x300 "small\/\1.bmp"/' | bash)
```
 * `make_mosaic` gets an input image (in BMP format) and a text file with one path per line to the input tile BMPs, the
   latter can be generated via `find good_panels/small -iname '*.bmp' > input_files.txt`

Finally for the demo display the script `generate_deepzoom_tiles.py` in the branch
https://github.com/or/tintin/tree/gh-pages generates the files for https://github.com/openseadragon/openseadragon to
display the result on http://or.github.io/tintin.

