#include <string.h>
#include "Bitmap.h"

Bitmap::Bitmap(const char *fileName)
       :_isValid(false),
        _fileName(fileName),
        _bitmap(NULL)
{
  ifstream file(fileName, ios::in | ios::binary);
  if (!file.is_open()) {
    printf("    Error: couldn't open file '%s'\n", fileName);
    return;
  }

  file.seekg(0, ios::end);
  int fsize = file.tellg();
  file.seekg(0, ios::beg);
  unsigned char *bBuf = new unsigned char[fsize];
  file.read((char*)bBuf, fsize);
  file.close();

  if (!(bBuf[0] == 'B' && bBuf[1] == 'M')) {
    delete[] bBuf;
    printf("    '%s' doesn't have a Bitmap header...\n", fileName);
    return;
  }

  int width = *((long*)&bBuf[18]);
  int height = *((long*)&bBuf[22]);
  short bitDepth = *((short*)&bBuf[28]);
  if (bitDepth != 24) {
    delete[] bBuf;
    printf("    '%s' has an unsupported bit depth of %d...\n", fileName, bitDepth);
    return;
  }

  int offset = 54;

  _width = width;
  _height = height;
  _bitmap = new unsigned char[3 * width * height];
  unsigned char *ptrIn = bBuf + offset, *ptrOut;

  if (bitDepth == 24) {
    int linePadding = (4 - ((3 * width) % 4)) % 4;
    ptrOut = _bitmap + (height - 1) * 3 * width;
    for (int y = 0; y < height; ++y) {
      memcpy(ptrOut, ptrIn, 3 * width);
      ptrIn += 3 * width + linePadding;
      ptrOut -= 3 * width;
    }
  }

  delete[] bBuf;

  _isValid = true;
}



Bitmap::Bitmap(int width, int height)
       :_isValid(false),
        _width(width),
        _height(height)
{
  _bitmap = new unsigned char[3 * width * height];
  memset(_bitmap, 0, 3 * width * height);
  _isValid = true;
}



Bitmap::~Bitmap()
{
  if (_bitmap != NULL) {
    delete[] _bitmap;
    _bitmap = NULL;
  }
}



bool Bitmap::isValid() const
{
  return _isValid;
}



int Bitmap::getWidth() const
{
  return _width;
}



int Bitmap::getHeight() const
{
  return _height;
}



unsigned char *Bitmap::getBitmapData()
{
  return _bitmap;
}



void Bitmap::setFileName(const char *fileName)
{
  _fileName = fileName;
}



const char *Bitmap::getFileName() const
{
  return _fileName.c_str();
}



SmartPointer<Bitmap> Bitmap::reduce(int factor)
{
  /*int reducedWidth = (_width + factor / 2) / factor;
  int reducedHeight = (_height + factor / 2) / factor;*/
  int reducedWidth = _width / factor;
  int reducedHeight = _height / factor;
  if (reducedWidth == 0) {
    reducedWidth = 1;
  }
  if (reducedHeight == 0) {
    reducedHeight = 1;
  }
  SmartPointer<Bitmap> reduced = new Bitmap(reducedWidth, reducedHeight);
  reduced->setFileName(_fileName.c_str());
  unsigned char *reducedBuf = reduced->getBitmapData();
  for (int y = 0; y < reducedHeight; ++y) {
    for (int x = 0; x < reducedWidth; ++x) {
      unsigned int R = 0, G = 0, B = 0;
      unsigned int values = 0;
      for (int j = y * factor; j < (y + 1) * factor && j < _height; ++j) {
        unsigned char *ptr = &_bitmap[(j * _width + x * factor) * 3];
        for (int i = x * factor; i < (x + 1) * factor && i < _width; ++i) {
          R += ptr[0];
          G += ptr[1];
          B += ptr[2];
          ++values;
          ptr += 3;
        }
      }
      R = (R + values / 2) / values;
      G = (G + values / 2) / values;
      B = (B + values / 2) / values;
      reducedBuf[0] = (unsigned char)R;
      reducedBuf[1] = (unsigned char)G;
      reducedBuf[2] = (unsigned char)B;
      //printf("%d %d %d\n", R, G, B);
      reducedBuf += 3;
    }
  }

  return reduced;
}



bool Bitmap::save(const char *fileName)
{
  ofstream outputFile(fileName, ios::out | ios::binary);
  if (!outputFile.is_open()) {
    printf("    Error: couldn't open file '%s'\n", fileName);
    return false;
  }

  BITMAPHEADER header;
  memset((char*)&header, 0, sizeof(BITMAPHEADER));
  header.magic[0] = 'B';
  header.magic[1] = 'M';
  header.fileSize = sizeof(BITMAPHEADER) + _height * ((3 * _width) % 4);
  header.offset = sizeof(BITMAPHEADER);
  header.structSize = 40; // 40 bytes left from this point on
  header.width = _width;
  header.height = _height;
  header.numPlanes = 1;
  header.bitCount = 24;
  header.imageSize = _height * ((3 * _width) % 4);
  outputFile.write((char*)&header, sizeof(BITMAPHEADER));

  char padding[4] = {0};
  for (int y = _height - 1; y >= 0; --y) {
    outputFile.write((char*)&_bitmap[y * _width * 3], _width * 3);
    if (4 - (_width * 3) % 4 != 4) {
      outputFile.write(padding, 4 - (_width * 3) % 4);
    }
  }

  outputFile.close();

  return true;
}
