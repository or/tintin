#include <stdio.h>

#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Bitmap.h"

using std::pair;
using std::set;
using std::vector;
using std::shared_ptr;

typedef struct {
  int x, y;
  shared_ptr<Bitmap> bitmap;
} Tile;

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("need <master file> <file list>\n");
    return -1;
  }

  srand(2107);

  int param = 1;
  Bitmap inputBitmap(argv[param]);
  if (!inputBitmap.isValid()) {
    printf("couldn't open '%s'...\n", argv[param]);
    return -1;
  }
  ++param;

  vector< shared_ptr<Bitmap> > bitmapList;
  std::ifstream fileList(argv[param], std::ios::in);
  if (!fileList.is_open()) {
    printf("couldn't open '%s'...\n", argv[param]);
    return -1;
  }

  int factor = 50;
  int count = -1;
  while (fileList.good()) {
    std::string line;
    getline(fileList, line);
    if (line.size() == 0) {
      continue;
    }
    ++count;
    //printf("opening '%s'...\n", line.c_str());
    Bitmap bmp(line.c_str());
    if (bmp.getWidth() < factor || bmp.getHeight() < factor) {
      continue;
    }
    shared_ptr<Bitmap> newBitmap(bmp.reduce(factor));
    if (!newBitmap->isValid()) {
      printf("couldn't open '%s'...\n", line.c_str());
      continue;
    }
    //printf("%dx%d\n", newBitmap->getWidth(), newBitmap->getHeight());
    bitmapList.push_back(newBitmap);
  }
  fileList.close();
  printf("got %d tile images\n", static_cast<unsigned int>(bitmapList.size()));

  set< pair<int, int> > openSpots;
  for (int y = -3; y < inputBitmap.getHeight(); ++y) {
    for (int x = -3; x < inputBitmap.getWidth(); ++x) {
      if (x < 0 || y < 0) {
        if (rand() % 5 != 0) {
          continue;
        }
      }
      openSpots.insert(pair<int, int>(x, y));
    }
  }
  unsigned int maxOpenSpots = openSpots.size();

  unsigned int variance = 20;
  vector<int> usedTilesList(bitmapList.size(), 0);
  unsigned int uniqueTiles = 0;

  vector<Tile> tileList;
  // first strategy
  unsigned int maxHits = 70000;
  unsigned int hits = 0;
  for (hits = 0; hits < maxHits; ++hits) {
    int x = (int)((double)rand() * inputBitmap.getWidth() / RAND_MAX);
    int y = (int)((double)rand() * inputBitmap.getHeight() / RAND_MAX);
    if (openSpots.size() > 0) {
      int ri = (int)((double)rand() * openSpots.size() / RAND_MAX);
      set< pair<int, int> >::iterator it = openSpots.begin();
      while (ri > 0) {
        ++it;
        --ri;
      }
      x = it->first;
      y = it->second;
    }

    vector<unsigned int > fitList;
    vector<unsigned int> errorList;
    for (unsigned int i = 0; i < bitmapList.size(); ++i) {
      unsigned int error = 0;
      unsigned int samples = 0;
      for (int py = 0; py < bitmapList[i]->getHeight(); ++py) {
        if (y + py < 0 || y + py >= inputBitmap.getHeight()) {
          break;
        }

        for (int px = 0; px < bitmapList[i]->getWidth(); ++px) {
          if (x + px < 0 || x + px >= inputBitmap.getWidth()) {
            break;
          }
          unsigned char *smallPtr = bitmapList[i]->getBitmapData() +
                                    (py * bitmapList[i]->getWidth() + px) * 3;
          unsigned char *bigPtr = inputBitmap.getBitmapData() +
                                  ((y + py) * inputBitmap.getWidth() + x + px) * 3;
          error += abs((int)smallPtr[0] - (int)bigPtr[0]);
          error += abs((int)smallPtr[1] - (int)bigPtr[1]);
          error += abs((int)smallPtr[2] - (int)bigPtr[2]);
          ++samples;
        }
      }

      if (samples != 0) {
        error /= samples;
      }
      error += 5 * usedTilesList[i];

      unsigned int j = 0;
      while (j < errorList.size()) {
        if (errorList[j] > error) {
          errorList.insert(errorList.begin() + j, error);
          fitList.insert(fitList.begin() + j, i);
          break;
        }
        ++j;
      }
      if (j >= errorList.size() && errorList.size() < variance) {
        errorList.push_back(error);
        fitList.push_back(i);
      }
      while (errorList.size() > variance) {
        errorList.pop_back();
        fitList.pop_back();
      }
    }

    unsigned int winnerInd = fitList[rand() % 10];
    if (usedTilesList[winnerInd] == 0) {
      ++uniqueTiles;
    }
    ++usedTilesList[winnerInd];
    shared_ptr<Bitmap> winner = bitmapList[winnerInd];
    Tile tile = {x, y, winner};
    tileList.push_back(tile);

    for (int py = 0; py < winner->getHeight(); ++py) {
      for (int px = 0; px < winner->getWidth(); ++px) {
        set< pair<int, int> >::iterator it = openSpots.find(pair<int, int>(x + px, y + py));
        if (it != openSpots.end()) {
          openSpots.erase(it);
        }
      }
    }

    if (openSpots.size() == 0) {
      break;
    }

    if (hits % 100 == 0) {
      printf("\r%.01f%%", 100.0 - 100.0 * openSpots.size() / maxOpenSpots);
      fflush(stdout);
    }
  }
  printf("\rdone.    \n");
  printf("had %d hits, used %d/%d unique tiles\n",
         hits, uniqueTiles, static_cast<unsigned int>(bitmapList.size()));

  printf("starting preview...\n");
  double inAlpha = 0.1;
  int outputFactor = 1;
  Bitmap previewBitmap(outputFactor * inputBitmap.getWidth(),
                       outputFactor * inputBitmap.getHeight());
  for (unsigned int i = 0; i < tileList.size(); ++i) {
    Tile tile = tileList[i];
    for (int py = 0; py < tile.bitmap->getHeight(); ++py) {
      if (tile.y + py < 0 || tile.y + py >= previewBitmap.getHeight()) {
        break;
      }

      for (int px = 0; px < tile.bitmap->getWidth(); ++px) {
        if (tile.x + px < 0 || tile.x + px >= previewBitmap.getWidth()) {
          break;
        }
        unsigned char *smallPtr = tile.bitmap->getBitmapData() +
                                  (py * tile.bitmap->getWidth() + px) * 3;
        unsigned char *outPtr = previewBitmap.getBitmapData() +
                                ((tile.y + py) * previewBitmap.getWidth() + tile.x + px) * 3;
        unsigned char *inPtr = inputBitmap.getBitmapData() +
                               ((tile.y + py) * inputBitmap.getWidth() +
                                (tile.x + px)) * 3;
        outPtr[0] = (unsigned char)((1.0 - inAlpha) * smallPtr[0] + inAlpha * inPtr[0]);
        outPtr[1] = (unsigned char)((1.0 - inAlpha) * smallPtr[1] + inAlpha * inPtr[1]);
        outPtr[2] = (unsigned char)((1.0 - inAlpha) * smallPtr[2] + inAlpha * inPtr[2]);
      }
    }
  }
  previewBitmap.save("mosaic_preview.bmp");

  printf("done.\n");
  printf("starting output...\n");
  inAlpha = 0;
  outputFactor = 50;
  int edge = 300;
  int tileEdge = 3;
  Bitmap outputBitmap(outputFactor * inputBitmap.getWidth() + 2 * edge,
                      outputFactor * inputBitmap.getHeight() + 2 * edge);
  unsigned char black[3] = {0, 0, 0};
  for (unsigned int i = 0; i < tileList.size(); ++i) {
    Tile tile = tileList[i];
    Bitmap bitmap(tile.bitmap->getFileName());
    if (!bitmap.isValid()) {
      printf("couldn't open '%s'...\n", tile.bitmap->getFileName());
      exit(-1);
    }
    int xWiggle = (bitmap.getWidth() - tileEdge * 2) % outputFactor;
    int yWiggle = (bitmap.getHeight() - tileEdge * 2) % outputFactor;
    int posX = edge + tile.x * outputFactor - tileEdge;
    int posY = edge + tile.y * outputFactor - tileEdge;
    if (xWiggle != 0) {
       posX += rand() % xWiggle - xWiggle / 2;
    }
    if (yWiggle != 0) {
       posY += rand() % yWiggle - yWiggle / 2;
    }

    for (int py = tileEdge; py < bitmap.getHeight() - tileEdge; ++py) {
      if (posY + py < 0 || posY + py >= outputBitmap.getHeight()) {
        break;
      }

      for (int px = tileEdge; px < bitmap.getWidth() - tileEdge; ++px) {
        if (posX + px < 0 || posX + px >= outputBitmap.getWidth()) {
          break;
        }
        unsigned char *smallPtr = bitmap.getBitmapData() +
                                  (py * bitmap.getWidth() + px) * 3;
        unsigned char *outPtr = outputBitmap.getBitmapData() +
                                ((posY + py) * outputBitmap.getWidth() + posX + px) * 3;

        int inX = (posX + px) / outputFactor;
        int inY = (posY + py) / outputFactor;
        unsigned char *inPtr = black;
        if (inX >= 0 &&
            inY >= 0 &&
            inX < inputBitmap.getWidth() &&
            inY < inputBitmap.getHeight()) {
          inPtr = inputBitmap.getBitmapData() + (inY * inputBitmap.getWidth() + inX) * 3;
        }
        outPtr[0] = (unsigned char)((1.0 - inAlpha) * smallPtr[0] + inAlpha * inPtr[0]);
        outPtr[1] = (unsigned char)((1.0 - inAlpha) * smallPtr[1] + inAlpha * inPtr[1]);
        outPtr[2] = (unsigned char)((1.0 - inAlpha) * smallPtr[2] + inAlpha * inPtr[2]);
      }
    }

    if (i % 50 == 0) {
      printf("\r%.01f%%", 100.0 * i / tileList.size());
      fflush(stdout);
    }
  }
  outputBitmap.save("mosaic_result.bmp");
  printf("\rdone.     \n");

  return 0;
}
