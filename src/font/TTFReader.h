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

  std::vector<tableDirectoryEntries_t> tableDirectories;

  std::vector<glyfDescription_t> glyfDescriptions;

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

  void readSimpleGlyph(SimpleGlyf_t &simpleGlyf,
                       glyfDescription_t &glyfDescription);

  void printSimpleGlyph(SimpleGlyf_t &simpleGlyf);

  void printOffsetSubtables();

  int findTable(std::string searchedTag);

  void readGlyfDescription();

  void printGlyfDescription(int index);

  void printOutlineFlags(outlineFlags_t outlineFlags);
};
