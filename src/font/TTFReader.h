#pragma once
#include "TTFTypes.h"
#include "reader.h"
#include <cstdint>
#include <fstream>
#include <string.h>
#include <vector>

class TTFReader {
private:
  uint32_t scalerType;
  uint16_t numTables;
  uint16_t searchRange;
  uint16_t entrySelector;
  uint16_t rangeShift;

  int glyfTableNumber;

  headTable_t headTable;
  maxpTable_t maxpTable;
  Glyf_t glyf;

  std::vector<tableDirectoryEntries_t> tableDirectories;

  std::vector<glyfDescription_t> glyfDescriptions;
  std::vector<uint32_t> glyphOffsets;

  std::ifstream fontFile;

  Reader reader;

public:
  TTFReader(const std::string &fontFilePath);

  void openFontFile(const std::string &fontFilePath);

  void readOffsetSubtable();
  std::string convertTagUint32ToString(uint32_t tagValue);
  void readTableDirectory();
  void readTableDirectories();
  void printTableDirectoryEntry(int index);
  void printTableDirectoryEntries();
  void readGlyf(Glyf_t &glyf, int index);
  void readSimpleGlyph(SimpleGlyf_t &simpleGlyf,
                       glyfDescription_t &glyfDescription);
  void printSimpleGlyph(SimpleGlyf_t &simpleGlyf);
  void printCompoundGlyph(compoundGlyf_t &compoundGlyf);
  void printGlyph(Glyf_t &glyf);
  void printOffsetSubtables();
  int findTable(std::string searchedTag);
  void readGlyfDescription(glyfDescription_t &glyfDescription);
  void printGlyfDescription(glyfDescription_t &glyfDescription);
  void printOutlineFlags(outlineFlags_t outlineFlags);
  void readHeadTable(headTable_t &headTable);
  void printHeadTable(headTable_t &headTable);
  void readMaxpTable(maxpTable_t &maxpTable);
  void printMaxpTable(maxpTable_t &maxpTable);
  void readLocaTable(std::vector<uint32_t> &glyphOffsets);
  void readXYPoints(SimpleGlyf_t &simpleGlyf, bool useX);
};
