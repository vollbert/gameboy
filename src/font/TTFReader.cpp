#include "TTFReader.h"
#include "TTFTypes.h"
#include <bitset>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>

TTFReader::TTFReader(const std::string &fontFilePath) {
  openFontFile(fontFilePath);
  readOffsetSubtable();
  // printOffsetSubtables();

  readTableDirectories();
  printTableDirectoryEntries();
  readHeadTable(headTable);
  // printHeadTable(headTable);
  //
  // read maxp table
  readMaxpTable(maxpTable);
  // printMaxpTable(maxpTable);

  // read loca description
  readLocaTable(glyphOffsets);

  readGlyf(glyf, 0);
  printSimpleGlyph(glyf.simpleGlyf);
}

void TTFReader::openFontFile(const std::string &fontFilePath) {
  fontFile.open(fontFilePath);
  if (fontFile.is_open()) {
    std::cout << "Opened font file." << std::endl;
  } else {
    std::cout << "Couldn't open font file";
  }
}

void TTFReader::readOffsetSubtable() {
  scalerType = reader.readUnsigned32Bit(&fontFile);
  numTables = reader.readUnsigned16Bit(&fontFile);
  searchRange = reader.readUnsigned16Bit(&fontFile);
  entrySelector = reader.readUnsigned16Bit(&fontFile);
  rangeShift = reader.readUnsigned16Bit(&fontFile);
}

std::string TTFReader::convertTagUint32ToString(uint32_t tagValue) {
  char chars[4];
  chars[0] = static_cast<char>((tagValue >> 24) & 0xFF);
  chars[1] = static_cast<char>((tagValue >> 16) & 0xFF);
  chars[2] = static_cast<char>((tagValue >> 8) & 0xFF);
  chars[3] = static_cast<char>(tagValue & 0xFF);
  return std::string(chars, 4);
}

void TTFReader::readTableDirectory() {
  tableDirectoryEntries_t tableDirectory;
  tableDirectory.tag =
      convertTagUint32ToString(reader.readUnsigned32Bit(&fontFile));
  tableDirectory.checkSum = reader.readUnsigned32Bit(&fontFile);
  tableDirectory.offset = reader.readUnsigned32Bit(&fontFile);
  tableDirectory.length = reader.readUnsigned32Bit(&fontFile);
  tableDirectories.push_back(tableDirectory);
}

void TTFReader::readTableDirectories() {
  for (int i = 0; i < numTables; i++) {
    readTableDirectory();
  }
}

void TTFReader::readGlyf(Glyf_t &glyf, int index) {
  glyfTableNumber = findTable("glyf");
  std::cout << "glyf table number: " << glyfTableNumber;
  fontFile.seekg(tableDirectories[glyfTableNumber].offset + glyphOffsets[index],
                 std::ios::beg);
  readGlyfDescription(glyf.glyfDescription);
  printGlyfDescription(glyf.glyfDescription);
  // to do: check if simple glyf
  if (glyf.glyfDescription.numberOfContours >= 0) {
    // read simple glyf
    readSimpleGlyph(glyf.simpleGlyf, glyf.glyfDescription);
  } else {
    // glyph is compound glyph to do
    std::cout << "compound glyph: to do" << std::endl;
  }
}

void TTFReader::readSimpleGlyph(SimpleGlyf_t &simpleGlyf,
                                glyfDescription_t &glyfDescription) {

  // std::cout << std::endl;
  for (int i = 0; i < glyfDescription.numberOfContours; i++) {
    simpleGlyf.endPtsOfContours.push_back(reader.readUnsigned16Bit(&fontFile));
    // std::cout << "endPtsOfContours: " << i << ": "
    //           << simpleGlyf.endPtsOfContours[i] << std::endl;
  }

  uint16_t instructionLength = reader.readUnsigned16Bit(&fontFile);
  // std::cout << "instructionLength: " << instructionLength << std::endl;

  for (int i = 0; i < instructionLength; i++) {
    simpleGlyf.instructions.push_back(reader.readUnsigned8Bit(&fontFile));
    // std::cout << "instructions: " << i << ": " << std::hex <<
    // std::setfill('0')
    //           << std::setw(2) << static_cast<uint16_t>(instructions[i])
    //           << std::endl;
  }

  int numberOfPoints =
      simpleGlyf.endPtsOfContours[simpleGlyf.endPtsOfContours.size() - 1] + 1;
  // std::cout << std::endl << "endPtsOfContours: " << numberOfPoints <<
  // std::endl;

  int i = 0;
  while (simpleGlyf.flags.size() < numberOfPoints) {
    uint8_t raw_bytes = reader.readUnsigned8Bit(&fontFile);
    // std::cout << "test: " << static_cast<int>(raw_bytes) << std::endl;
    outlineFlags_t currentOutlineFlag(raw_bytes);
    simpleGlyf.flags.push_back(currentOutlineFlag);
    // printOutlineFlags(currentOutlineFlag);
    if (currentOutlineFlag.repeat) {
      uint8_t repeatTimes = reader.readUnsigned8Bit(&fontFile);
      for (int j = 0; j < repeatTimes; j++) {
        simpleGlyf.flags.push_back(currentOutlineFlag);
        // printOutlineFlags(currentOutlineFlag);
      }
    }
    i++;
  }

  // read x points
  uint16_t previousPoint;
  // for (int i = 0; i < numberOfPoints; i++) {
  //   newContour.push_back(0);
  // }
  // for (int i = 0; i < endPtsOfContours.size(); i++) {
  //   newContour[endPtsOfContours[i]] = endPtsOfContours[i];
  // }

  // read x
  readXYPoints(simpleGlyf, 1);
  // read y points
  readXYPoints(simpleGlyf, 0);
}

void TTFReader::printSimpleGlyph(SimpleGlyf_t &simpleGlyf) {
  // print endPtsOfContours
  std::cout << "End Points: {";
  for (int i = 0; i < simpleGlyf.endPtsOfContours.size(); i++) {
    std::cout << simpleGlyf.endPtsOfContours[i];
    if (!(i == simpleGlyf.endPtsOfContours.size() - 1)) {
      std::cout << " ";
    }
  }
  std::cout << "}" << std::endl;

  // print instructionLength
  std::cout << "instructionLength: " << simpleGlyf.instructionLength
            << std::endl;

  // print instructions
  std::cout << "Instructions: {";
  for (int i = 0; i < simpleGlyf.instructions.size(); i++) {
    std::cout << std::setfill('0') << std::setw(2) << std::hex
              << static_cast<uint16_t>(simpleGlyf.instructions[i]);
    if (!(i == simpleGlyf.instructions.size() - 1)) {
      std::cout << " ";
    }
  }
  std::cout << "}" << std::endl;

  // flags
  //  print instructions
  std::cout << "Flags: {";
  for (int i = 0; i < simpleGlyf.flags.size(); i++) {
    std::bitset<8> x(static_cast<uint16_t>(simpleGlyf.instructions[i]));
    std::cout << x;
    if (!(i == simpleGlyf.flags.size() - 1)) {
      std::cout << " ";
    }
  }
  std::cout << "}" << std::endl;

  //  print xCoordinates
  std::cout << "xCoordinates: {";
  for (int i = 0; i < simpleGlyf.xCoordinates.size(); i++) {
    std::cout << std::dec << simpleGlyf.xCoordinates[i];
    if (!(i == simpleGlyf.xCoordinates.size() - 1)) {
      std::cout << " ";
    }
  }
  std::cout << "}" << std::endl;

  //  print yCoordinates
  std::cout << "yCoordinates: {";
  for (int i = 0; i < simpleGlyf.yCoordinates.size(); i++) {
    std::cout << std::dec << simpleGlyf.yCoordinates[i];
    if (!(i == simpleGlyf.yCoordinates.size() - 1)) {
      std::cout << " ";
    }
  }
  std::cout << "}" << std::endl;
}

void TTFReader::printCompoundGlyph(compoundGlyf_t &compoundGlyf) {
  std::cout << "need to implement printCompoundGlyph" << std::endl;
}

void TTFReader::printGlyph(Glyf_t &glyf) {
  glyf.isSimple() ? printSimpleGlyph(glyf.simpleGlyf)
                  : printCompoundGlyph(glyf.compoundGlyf);
}

void TTFReader::printTableDirectoryEntry(int index) {
  std::cout << "Directory: " << std::dec << index << std::endl;
  std::cout << "Tag: " << std::hex << tableDirectories[index].tag << std::endl;
  std::cout << "Check Sum: " << std::hex << tableDirectories[index].checkSum
            << std::endl;
  std::cout << "Offset: " << std::dec << tableDirectories[index].offset
            << std::endl;
  std::cout << "length: " << std::dec << tableDirectories[index].length
            << std::endl;
  std::cout << std::endl;
}

void TTFReader::printTableDirectoryEntries() {
  for (int i = 0; i < numTables; i++) {
    printTableDirectoryEntry(i);
  }
}

void TTFReader::printOffsetSubtables() {
  std::cout << std::hex << "scalerType: " << scalerType << std::endl;
  std::cout << std::dec << "numTables: " << numTables << std::endl;
  std::cout << "searchRange: " << searchRange << std::endl;
  std::cout << "entrySelector: " << entrySelector << std::endl;
  std::cout << "rangeShift: " << rangeShift << std::endl;
}

int TTFReader::findTable(std::string searchedTag) {
  for (int i = 0; i < numTables; i++) {
    if (tableDirectories[i].tag == searchedTag) {
      return i;
    }
  }
  std::cout << "Couldn't find the required tag in the table. " << std::endl;
  return -1;
}

void TTFReader::readGlyfDescription(glyfDescription_t &glyfDescription) {
  glyfDescription.numberOfContours = reader.readSigned16Bit(&fontFile);
  glyfDescription.xMin = reader.readSigned16Bit(&fontFile);
  glyfDescription.yMin = reader.readSigned16Bit(&fontFile);
  glyfDescription.xMax = reader.readSigned16Bit(&fontFile);
  glyfDescription.yMax = reader.readSigned16Bit(&fontFile);
}

void TTFReader::printGlyfDescription(glyfDescription_t &glyfDescription) {
  std::cout << "numberContours: " << std::dec
            << glyfDescription.numberOfContours << std::endl;
  std::cout << "xMin: " << glyfDescription.xMin << std::endl;
  std::cout << "yMin: " << glyfDescription.yMin << std::endl;
  std::cout << "xMax: " << glyfDescription.xMax << std::endl;
  std::cout << "yMax: " << glyfDescription.yMax << std::endl;
}

void TTFReader::printOutlineFlags(outlineFlags_t outlineFlags) {
  std::cout << "onCurve: " << outlineFlags.onCurve << std::endl;
  std::cout << "xShortVector: " << outlineFlags.xShortVector << std::endl;
  std::cout << "yShortVector: " << outlineFlags.yShortVector << std::endl;
  std::cout << "repeat: " << outlineFlags.repeat << std::endl;
  std::cout << "xSame: " << outlineFlags.xSame << std::endl;
  std::cout << "ySame: " << outlineFlags.ySame << "\n\n";
  return;
}

void TTFReader::readHeadTable(headTable_t &headTable) {
  int headTableIndex = findTable("head");
  // std::cout << "head table: " << headTableIndex << std::endl;
  fontFile.seekg(tableDirectories[headTableIndex].offset, std::ios::beg);
  headTable.version = reader.readFixed(&fontFile);
  headTable.fontRevision = reader.readFixed(&fontFile);
  headTable.checkSumAdjustment = reader.readUnsigned32Bit(&fontFile);
  headTable.magicNumber = reader.readUnsigned32Bit(&fontFile);
  headTable.flags = reader.readUnsigned16Bit(&fontFile);
  headTable.unitsPerEm = reader.readUnsigned16Bit(&fontFile);
  headTable.created = reader.readLongDateTime(&fontFile);
  headTable.modified = reader.readLongDateTime(&fontFile);
  headTable.xMin = reader.readFWord(&fontFile);
  headTable.yMin = reader.readFWord(&fontFile);
  headTable.xMax = reader.readFWord(&fontFile);
  headTable.yMax = reader.readFWord(&fontFile);
  headTable.macStyle = reader.readUnsigned16Bit(&fontFile);
  headTable.lowestRecPPEM = reader.readUnsigned16Bit(&fontFile);
  headTable.fontDirectionHint = reader.readSigned16Bit(&fontFile);
  headTable.indexToLocFormat = reader.readSigned16Bit(&fontFile);
  headTable.glyphDataFormat = reader.readSigned16Bit(&fontFile);
}

void TTFReader::printHeadTable(headTable_t &headTable) {
  std::cout << "head table: " << std::endl;
  std::cout << "version: " << std::hex << std::setprecision(1) << std::fixed
            << reader.fixedToDouble(headTable.version) << std::endl;

  std::cout << "fontRevision: " << std::hex << std::setprecision(1)
            << std::fixed << reader.fixedToDouble(headTable.fontRevision)
            << std::endl;

  std::cout << "checkSumAdjustment: " << headTable.checkSumAdjustment
            << std::endl;
  std::cout << "magicNumber: " << headTable.magicNumber << std::endl;
  std::cout << "flags: " << headTable.flags << std::endl;
  std::cout << "unitsPerEm: " << headTable.unitsPerEm << std::endl;
  std::cout << "created: " << headTable.created << std::endl;
  std::cout << "modified: " << headTable.modified << std::endl;
  std::cout << "xMin: " << headTable.xMin << std::endl;
  std::cout << "yMin: " << headTable.yMin << std::endl;
  std::cout << "xMax: " << headTable.xMax << std::endl;
  std::cout << "yMax: " << headTable.yMax << std::endl;
  std::cout << "lowestRecPPEM: " << headTable.lowestRecPPEM << std::endl;
  std::cout << "fontDirectionHint: " << headTable.fontDirectionHint
            << std::endl;
  std::cout << "indexToLocFormat: " << headTable.indexToLocFormat << std::endl;
  std::cout << "glyphDataFormat: " << headTable.glyphDataFormat << std::endl
            << std::endl
            << std::endl;
}

void TTFReader::readMaxpTable(maxpTable_t &maxpTable) {
  int maxpTableIndex = findTable("maxp");
  // std::cout << "maxp table index: " << maxpTableIndex << std::endl;
  fontFile.seekg(tableDirectories[maxpTableIndex].offset, std::ios::beg);
  maxpTable.version = reader.readFixed(&fontFile);
  maxpTable.numGlyphs = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxPoints = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxContours = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxComponentPoints = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxComponentContours = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxZones = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxTwilightPoints = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxStorage = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxFunctionsDefs = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxInstructionDefs = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxStackElements = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxSizeOfInstructions = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxComponentElements = reader.readUnsigned16Bit(&fontFile);
  maxpTable.maxComponentDepth = reader.readUnsigned16Bit(&fontFile);
}

void TTFReader::printMaxpTable(maxpTable_t &maxpTable) {
  std::cout << "maxp table:" << std::endl;
  std::cout << "version: " << maxpTable.version << std::endl;
  std::cout << "numGlyphs: " << maxpTable.numGlyphs << std::endl;
  std::cout << "maxPoints: " << maxpTable.maxPoints << std::endl;
  std::cout << "maxContours: " << maxpTable.maxContours << std::endl;
  std::cout << "maxComponentPoints: " << maxpTable.maxComponentPoints
            << std::endl;
  std::cout << "maxComponentContours: " << maxpTable.maxComponentContours
            << std::endl;
  std::cout << "maxZones: " << maxpTable.maxZones << std::endl;
  std::cout << "maxTwilightPoints: " << maxpTable.maxTwilightPoints
            << std::endl;
  std::cout << "maxStorage: " << maxpTable.maxStorage << std::endl;
  std::cout << "maxFunctionDefs: " << maxpTable.maxFunctionsDefs << std::endl;
  std::cout << "maxInstructionDefs: " << maxpTable.maxInstructionDefs
            << std::endl;
  std::cout << "maxStackElements: " << maxpTable.maxStackElements << std::endl;
  std::cout << "maxSizeOfInstructions: " << maxpTable.maxSizeOfInstructions
            << std::endl;
  std::cout << "maxComponentElements: " << maxpTable.maxComponentElements
            << std::endl;
  std::cout << "maxComponentDepth: " << maxpTable.maxComponentDepth << std::endl
            << std::endl;
}

void TTFReader::readLocaTable(std::vector<uint32_t> &glyphOffsets) {
  int locaTableIndex = findTable("loca");
  // std::cout << "loca table: " << locaTableIndex << std::endl;
  // check index to loc format: if 0 short offsets, if 1 long offsets

  fontFile.seekg(tableDirectories[locaTableIndex].offset, std::ios::beg);
  // std::cout << "index format: " << headTable.indexToLocFormat << std::endl;

  if (headTable.indexToLocFormat == 1) {
    // offset represented in long format
    for (int i = 0; i < maxpTable.numGlyphs; i++) {
      glyphOffsets.push_back(reader.readUnsigned32Bit(&fontFile));
    }

  } else {
    // offset represented in short format
  }
}

void TTFReader::readXYPoints(SimpleGlyf_t &simpleGlyf, bool useX) {
  int numberOfPoints =
      simpleGlyf.endPtsOfContours[simpleGlyf.endPtsOfContours.size() - 1] + 1;
  int16_t previousPoint = 0;
  int16_t difference = 0;

  for (int i = 0; i < numberOfPoints; i++) {
    bool shortV = useX ? simpleGlyf.flags[i].xShortVector
                       : simpleGlyf.flags[i].yShortVector;
    bool same = useX ? simpleGlyf.flags[i].xSame : simpleGlyf.flags[i].ySame;

    if (shortV) {
      difference = static_cast<int16_t>(reader.readUnsigned8Bit(&fontFile));
      difference = same ? difference : -difference;
    } else {
      if (same) {
        difference = 0;
      } else {
        difference = reader.readSigned16Bit(&fontFile);
      }
    }
    int16_t newCoordinate = previousPoint + difference;
    useX ? simpleGlyf.xCoordinates.push_back(newCoordinate)
         : simpleGlyf.yCoordinates.push_back(newCoordinate);
    previousPoint = newCoordinate;
  }
}
