#pragma once
#include <cstdint>
#include <string>
#include <vector>

typedef int16_t shortFrac;
typedef int32_t Fixed; // a datatype where the upper and lower 16 bits are
                       // seperated by floating pointc
typedef int16_t FWord;
typedef uint16_t uFWord;
typedef int16_t F2Dot14;
typedef int64_t longDateTime;

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
  std::vector<int16_t> xCoordinates;
  std::vector<int16_t> yCoordinates;
};

struct compoundGlyf_t {};

struct Glyf_t {
  glyfDescription_t glyfDescription;
  SimpleGlyf_t simpleGlyf;
  compoundGlyf_t compoundGlyf;
  bool isSimple() { return glyfDescription.numberOfContours >= 0; }
  bool isCompound() { return glyfDescription.numberOfContours < 0; }
};

struct headTable_t {
  Fixed version;
  Fixed fontRevision;
  uint32_t checkSumAdjustment;
  uint32_t magicNumber;
  uint16_t flags;
  uint16_t unitsPerEm;
  longDateTime created;
  longDateTime modified;
  FWord xMin;
  FWord yMin;
  FWord xMax;
  FWord yMax;
  uint16_t macStyle;
  uint16_t lowestRecPPEM;
  int16_t fontDirectionHint;
  int16_t indexToLocFormat;
  int16_t glyphDataFormat;
};

struct maxpTable_t {
  Fixed version;
  uint16_t numGlyphs;
  uint16_t maxPoints;
  uint16_t maxContours;
  uint16_t maxComponentPoints;
  uint16_t maxComponentContours;
  uint16_t maxZones;
  uint16_t maxTwilightPoints;
  uint16_t maxStorage;
  uint16_t maxFunctionsDefs;
  uint16_t maxInstructionDefs;
  uint16_t maxStackElements;
  uint16_t maxSizeOfInstructions;
  uint16_t maxComponentElements;
  uint16_t maxComponentDepth;
};
