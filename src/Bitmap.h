#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <string>
#include <fstream>

#include "SmartPointer.h"

using namespace std;

#pragma pack(1)

typedef struct {
  char magic[2];
  unsigned int fileSize;
  unsigned int reserved;
  unsigned int offset;
  unsigned int structSize;
  unsigned int width;
  unsigned int height;
  unsigned short numPlanes;
  unsigned short bitCount;
  unsigned int compression;
  unsigned int imageSize;
  unsigned int XPelsPerMeter;
  unsigned int YPelsPerMeter;
  unsigned int numColoursUsed;
  unsigned int numColoursImportant;
} BITMAPHEADER;

class Bitmap : public SmartPointerBase {
public:
  Bitmap(const char *fileName);
  Bitmap(int width, int height);
  ~Bitmap();

  bool isValid() const;
  int getWidth() const;
  int getHeight() const;
  unsigned char *getBitmapData();
  void setFileName(const char *fileName);
  const char *getFileName() const;
  bool save(const char *fileName);

  SmartPointer<Bitmap> reduce(int factor);

private:
  bool _isValid;
  string _fileName;
  int _width, _height;
  unsigned char *_bitmap;
};

#endif
