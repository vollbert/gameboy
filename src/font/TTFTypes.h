#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct tableDirectoryEntries_t {
  std::string tag;
  uint32_t checkSum;
  uint32_t offset;
  uint32_t length;
};

struct glyfDescription_t {
  int16_t numberOfContours;
  int16_t xMin;
  int16_t yMin;
  int16_t xMax;
  int16_t yMax;
};

struct outlineFlags_t {
  bool onCurve;
  bool xShortVector;
  bool yShortVector;
  bool repeat;
  bool xSame; // look up meaning
  bool ySame;

  outlineFlags_t(uint8_t rawBytes) {
    onCurve = rawBytes & 0b00000001;
    xShortVector = (rawBytes >> 1) & 0b00000001;
    yShortVector = (rawBytes >> 2) & 0b00000001;
    repeat = (rawBytes >> 3) & 0b00000001;
    xSame = (rawBytes >> 4) & 0b00000001;
    ySame = (rawBytes >> 5) & 0b00000001;
  }
};

struct SimpleGlyf_t {
  std::vector<uint16_t> endPtsOfContours;
  uint16_t instructionLength;
  std::vector<uint8_t> instructions;
  std::vector<outlineFlags_t> flags;
  std::vector<uint16_t> xCoordinates;
  std::vector<uint16_t> yCoordinates;
};
