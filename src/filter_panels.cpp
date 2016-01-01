#include <stdio.h>

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "CImg.h"

#define WHITE_THRESHOLD 220

using cimg_library::CImg;
using cimg_library::CImgIOException;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("need <infile> [<infile> ...]\n");
    return -1;
  }
  int i = 1;
  while (i < argc) {
    try {
      CImg<unsigned char> image(argv[i]);
      unsigned int red = 0, green = 0, blue = 0;
      unsigned int white = 0, non_white = 0;
      unsigned int numPixels = image.width() * image.height();
      for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
          unsigned char *pixel = image._data + y * image.width() + x;
          //printf("(%d, %d, %d)\n", pixel[0], pixel[1], pixel[2]);
          //printf("(%d, %d, %d)\n", red, green, blue);
          if (pixel[0] >= WHITE_THRESHOLD &&
              pixel[1] >= WHITE_THRESHOLD &&
              pixel[2] >= WHITE_THRESHOLD) {
            ++white;
          } else {
            red += (int)pixel[0];
            green += (int)pixel[1];
            blue += (int)pixel[2];
            ++non_white;
          }
        }
      }
      if (100.0 * white / numPixels < 25) {
        system(((std::string)"cp '" + argv[i] + "' good_panels/").c_str());
      }
      printf("('%s', (%d, %d, %d), %.3f)\n",
             argv[i], red / non_white, green / non_white, blue / non_white,
             100.0 * white / numPixels);
    } catch (CImgIOException &e) {
      printf("    Error: %s\n", e.what());
    }

    ++i;
    if (argc - 1 != 1) {
      printf(" -- %.3f%%\n", 100.0 * (i - 1) / (argc - 1));
    }
  }

  return 0;
}
