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
  // printTableDirectoryEntries();

  glyfTableNumber = findTable("glyf");
  // std::cout << "glyfTableNumber: " << glyfTableNumber << std::endl;

  // read one glyf
  // maybe seek specific glyf
  fontFile.seekg(tableDirectories[glyfTableNumber].offset, std::ios::beg);

  // read glyf description
  // to do: use variable instead
  glyfDescription_t glyfDescription;
  readGlyfDescription();
  // printGlyfDescription(0);

  // to do: check if simple glyf

  // read simple glyf
  SimpleGlyf_t simpleGlyf;
  readSimpleGlyph(simpleGlyf, glyfDescription);
  printSimpleGlyph(simpleGlyf);
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

void TTFReader::readSimpleGlyph(SimpleGlyf_t &simpleGlyf,
                                glyfDescription_t &glyfDescription) {

  // std::cout << std::endl;
  for (int i = 0; i < glyfDescriptions[0].numberOfContours; i++) {
    simpleGlyf.endPtsOfContours.push_back(reader.readUnsigned16Bit(&fontFile));
    // std::cout << "endPtsOfContours: " << i << ": " << endPtsOfContours[i]
    //           << std::endl;
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
        printOutlineFlags(currentOutlineFlag);
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
  for (int i = 0; i < numberOfPoints; i++) {
    if (simpleGlyf.flags[i].xShortVector) {
      //  x-Short Vector bit is set, this bit describes the sign of the value,
      //  with a value of 1 equalling positive and a zero value negative.
      // convert uint8 to uint16 so that we only need one vector
      uint16_t xCoordinate =
          static_cast<int16_t>(reader.readUnsigned8Bit(&fontFile));
      // std::cout << "xCoordinate: " << std::dec << xCoordinate << std::endl;
      if (i == 0) {
        previousPoint = 0;
      } else {

        previousPoint = simpleGlyf.xCoordinates[i - 1];
      }
      xCoordinate = previousPoint +
                    (simpleGlyf.flags[i].xSame ? xCoordinate : -xCoordinate);
      simpleGlyf.xCoordinates.push_back(xCoordinate);
    } else {
      if (simpleGlyf.flags[i].xSame) {
        // If the x-short Vector bit is not set, and this bit is set, then the
        // current x-coordinate is the same as the previous x-coordinate.
        //
        // std::cout << "xCoordinates[i-1]: " << xCoordinates[i - 1]
        //           << std::endl;
        simpleGlyf.xCoordinates.push_back(simpleGlyf.xCoordinates[i - 1]);

      } else {
        // If the x-short Vector bit is not set, and this bit is not set, the
        // current x-coordinate is a signed 16-bit delta vector. In this case,
        // the delta vector is the change in x
        if (i == 0) {
          simpleGlyf.xCoordinates.push_back(reader.readSigned16Bit(&fontFile));

        } else {
          simpleGlyf.xCoordinates.push_back(simpleGlyf.xCoordinates[i - 1] +
                                            reader.readSigned16Bit(&fontFile));
          // std::cout << "xCoordinates[i-1]: " << xCoordinates[i - 1]
          //           << std::endl;
        }
      }
    }
  }

  // read y points
  for (int i = 0; i < numberOfPoints; i++) {
    if (simpleGlyf.flags[i].yShortVector) {
      //  y-Short Vector bit is set, this bit describes the sign of the
      //  value, with a value of 1 equalling positive and a zero value
      //  negative.
      // convert uint8 to uint16 so that we only need one vector
      uint16_t yCoordinate =
          static_cast<int16_t>(reader.readUnsigned8Bit(&fontFile));
      // std::cout << "yCoordinate: " << yCoordinate << std::endl;
      simpleGlyf.yCoordinates.push_back(
          simpleGlyf.flags[i].ySame ? yCoordinate : -yCoordinate);
    } else {
      if (simpleGlyf.flags[i].ySame) {
        // If the y-short Vector bit is not set, and this bit is set, then
        // the current y-coordinate is the same as the previous
        // y-coordinate.
        //
        if (i == 0) {

          simpleGlyf.yCoordinates.push_back(0);
        } else {
          // std::cout << "yCoordinates[i-1]: " << yCoordinates[i - 1]
          // << std::endl;
          simpleGlyf.yCoordinates.push_back(simpleGlyf.yCoordinates[i - 1]);
        }

      } else {
        // If the y-short Vector bit is not set, and this bit is not set,
        // the current y-coordinate is a signed 16-bit delta vector. In this
        // case, the delta vector is the change in y
        if (i == 0) {
          simpleGlyf.yCoordinates.push_back(reader.readSigned16Bit(&fontFile));
        } else {
          // std::cout << "yCoordinates[i-1]: " << yCoordinates[i - 1]
          // << std::endl;
          simpleGlyf.yCoordinates.push_back(simpleGlyf.yCoordinates[i - 1] +
                                            reader.readSigned16Bit(&fontFile));
        }
        // std::cout << "yCoordinate: " << yCoordinates[i] << std::endl;
      }
    }
  }
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

void TTFReader::readGlyfDescription() {
  glyfDescription_t bufferDescription;
  bufferDescription.numberOfContours = reader.readSigned16Bit(&fontFile);
  bufferDescription.xMin = reader.readSigned16Bit(&fontFile);
  bufferDescription.yMin = reader.readSigned16Bit(&fontFile);
  bufferDescription.xMax = reader.readSigned16Bit(&fontFile);
  bufferDescription.yMax = reader.readSigned16Bit(&fontFile);
  glyfDescriptions.push_back(bufferDescription);
}

void TTFReader::printGlyfDescription(int index) {
  std::cout << "Glyf Description: " << index << std::endl;
  std::cout << "numberContours: " << std::dec
            << glyfDescriptions[index].numberOfContours << std::endl;
  std::cout << "xMin: " << glyfDescriptions[index].xMin << std::endl;
  std::cout << "yMin: " << glyfDescriptions[index].yMin << std::endl;
  std::cout << "xMax: " << glyfDescriptions[index].xMax << std::endl;
  std::cout << "yMax: " << glyfDescriptions[index].yMax << std::endl;
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
