#include <stdio.h>

#include <algorithm>
#include <fstream>
#include <map>
#include <vector>

#include "CImg.h"

#define MINDIMS 50
#define AURA 6

using cimg_library::CImg;
using cimg_library::CImgIOException;

typedef struct {
  int left, top, right, bottom;
} Box;

inline bool pixel_matches(unsigned char *source, unsigned char *colour, int tolerance) {
  int diff = abs(source[0] - colour[0]) + abs(source[1] - colour[1]) + abs(source[2] - colour[2]);
  return diff <= 3 * tolerance;
}


Box find_box(CImg<unsigned char> &img, unsigned char *map, int x, int y,
             unsigned char *border_colour, int tolerance, int aura,
             unsigned char *seenBuf) {
  Box box = { x, y, x, y };
  int width = img.width();
  int height = img.height();
  memset(seenBuf, 0, width * height);
  seenBuf[y * width + x] = 1;
  std::map<int, bool> looseEnds;
  looseEnds[y * width + x] = true;
  while (looseEnds.size() > 0) {
    int value = looseEnds.begin()->first;
    looseEnds.erase(looseEnds.begin());
    int pos_x = value % width;
    int pos_y = value / width;

    for (int j = -aura; j <= aura; ++j) {
      for (int i = -aura; i <= aura; ++i) {
        int px = pos_x + i;
        int py = pos_y + j;

        if (px < 0 || px >= width ||
            py < 0 || py >= height) {
          continue;
        }
        int pos = py * width + px;
        if (seenBuf[pos] != 0) {
          continue;
        }
        seenBuf[pos] = 1;
        if (map[pos] == 0) {
          continue;
        }

        box.left = std::min(px, box.left);
        box.right = std::max(px, box.right);
        box.top = std::min(py, box.top);
        box.bottom = std::max(py, box.bottom);

        bool isAtEdge = false;
        for (int m = -1; m <= 1 && !isAtEdge; ++m) {
          for (int n = -1; n <= 1 && !isAtEdge; ++n) {
            if (px + m < 0 || py + n < 0 ||
                px + m >= width || py + n >= height) {
              continue;
            }
            if (map[(py + n) * width + px + m] == 0) {
              isAtEdge = true;
            }
          }
        }
        if (!isAtEdge) {
          continue;
        }

        looseEnds[pos] = true;
      }
    }
  }
  return box;
}


std::vector<Box> scan_for_panels(CImg<unsigned char> &img) {
  std::vector<Box> boxList;
  int width = img.width();
  int height = img.height();
  unsigned char border_colour[3] = {0, 0, 0};
  int tolerance = 190;
  unsigned char *covered = new unsigned char[width * height];
  unsigned char *seenBuf = new unsigned char[width * height];
  unsigned char *map = new unsigned char[width * height];
  memset(covered, 0, width * height);
  memset(map, 0, width * height);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      unsigned char *pixel = img._data + y * width + x;
      if (pixel_matches(pixel, border_colour, tolerance)) {
        map[y * width + x] = 1;
      }
    }
  }

  for (int y = 0; y < height - MINDIMS + 2; ++y) {
    for (int x = 0; x < width - MINDIMS + 2; ++x) {
      if (map[y * width + x] == 0) {
        continue;
      }
      if (covered[y * width + x] != 0) {
        continue;
      }
      // got a possible border pixel
      Box box = find_box(img, map, x, y, border_colour, tolerance, AURA, seenBuf);
      if (box.right - box.left < MINDIMS || box.bottom - box.top < MINDIMS) {
        // too small
        continue;
      }

      for (int j = box.top; j <= box.bottom; ++j) {
        memset(covered + j * width + box.left, 1, box.right - box.left + 1);
      }
      boxList.push_back(box);
    }
  }

  delete[] covered;
  delete[] seenBuf;
  delete[] map;
  return boxList;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("need <outfile> <infile> [<infile> ...]\n");
    return -1;
  }
  std::ofstream outFile(argv[1], std::ios::out);
  if (!outFile.is_open()) {
    printf("couldn't open '%s'...\n", argv[1]);
    return -1;
  }
  int i = 2;
  while (i < argc) {
    std::vector<Box> boxList;
    printf("starting scan for '%s'...\n", argv[i]);
    try {
      CImg<unsigned char> image(argv[i]);
      boxList = scan_for_panels(image);

      outFile << argv[i] << ": [";
      for (unsigned int j = 0; j < boxList.size(); ++j) {
        outFile << "[" << boxList[j].left << ", " <<
                          boxList[j].top << ", " <<
                          boxList[j].right << ", " <<
                          boxList[j].bottom <<
                   "], ";
      }
      outFile << "]\n";
    }
    catch (CImgIOException &e) {
      printf("    Error: %s\n", e.what());
    }

    ++i;
    printf("    got %d. -- %.3f%%\n",
           static_cast<unsigned int>(boxList.size()),
           100.0 * (i - 1) / (argc - 1));
  }

  outFile.close();

  return 0;
}
