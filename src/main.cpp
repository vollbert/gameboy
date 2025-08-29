#include <GLFW/glfw3.h>
#include <bitset>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <vector>

class CPU {
  // Registers
  uint8_t A, B, C, D, E, H, L;
  uint16_t SP, PC;
  // Flags
  uint8_t F;

public:
};

class MMU {};

class ROMHandler {};

class PPU {};

class APU {};

class Joypad {};

std::vector<uint8_t> memory;

class Reader {

public:
  uint8_t readUnsigned8Bit(std::ifstream *fontFile) {
    char readBuffer;
    uint16_t returnValue;
    fontFile->read(&readBuffer, sizeof(uint8_t));
    return static_cast<uint8_t>(readBuffer);
  }

  uint16_t readUnsigned16Bit(std::ifstream *fontFile) {
    char readBuffer[2];
    uint16_t returnValue;
    fontFile->read(&readBuffer[0], sizeof(uint16_t));
    return ((static_cast<uint16_t>(static_cast<uint8_t>(readBuffer[0])) << 8) |
            static_cast<uint16_t>(static_cast<uint8_t>(readBuffer[1])));
  }

  int16_t readSigned16Bit(std::ifstream *fontFile) {
    char readBuffer[2];
    uint16_t returnValue;
    fontFile->read(&readBuffer[0], sizeof(uint16_t));
    return ((static_cast<int16_t>(static_cast<uint8_t>(readBuffer[0])) << 8) |
            static_cast<int16_t>(static_cast<uint8_t>(readBuffer[1])));
  }

  int16_t readFWord(std::ifstream *fontFile) {
    return readSigned16Bit(fontFile);
  }

  uint32_t readUnsigned32Bit(std::ifstream *fontFile) {
    char readBuffer[4];
    uint16_t returnValue;
    fontFile->read(&readBuffer[0], sizeof(uint32_t));
    return ((static_cast<uint32_t>(static_cast<uint8_t>(readBuffer[0])) << 24) |
            (static_cast<uint32_t>(static_cast<uint8_t>(readBuffer[1])) << 16) |
            (static_cast<uint32_t>(static_cast<uint8_t>(readBuffer[2])) << 8) |
            (static_cast<uint32_t>(static_cast<uint8_t>(readBuffer[3]))));
  }

  uint16_t readPrintUnsigned16Bit(std::ifstream *fontFile) {
    uint16_t returnValue = readUnsigned16Bit(fontFile);
    std::cout << returnValue << std::endl;
    return returnValue;
  }

  uint32_t readPrintASCII32Bit(std::ifstream *fontFile) {
    uint32_t returnValue = readUnsigned32Bit(fontFile);
    std::cout << std::hex << std::setw(8) << std::setfill('0')
              << (static_cast<char>((returnValue >> 24) & 0xFF))
              << (static_cast<char>((returnValue >> 16) & 0xFF))
              << (static_cast<char>((returnValue >> 8) & 0xFF))
              << (static_cast<char>((returnValue) & 0xFF)) << std::endl;
    return returnValue;
  }
};

class Glyph {
private:
  uint8_t raw_flags;

public:
};

struct tableDirectoryEntries {
  std::string tag;
  uint32_t checkSum;
  uint32_t offset;
  uint32_t length;
};

struct glyfDesctiption {
  int16_t numberOfContours;
  int16_t xMin;
  int16_t yMin;
  int16_t xMax;
  int16_t yMax;
};

class TTFReader {
private:
  uint32_t scalerType;
  uint16_t numTables;
  uint16_t searchRange;
  uint16_t entrySelector;
  uint16_t rangeShift;

  int glyfTableNumber;

  std::vector<tableDirectoryEntries> tableDirectories;

  std::vector<glyfDesctiption> glyfDescriptions;

  std::ifstream fontFile;

  Reader reader;

public:
  TTFReader(const std::string &fontFilePath) {
    fontFile.open(fontFilePath);
    if (fontFile.is_open()) {
      std::cout << "Opened font file." << std::endl;
    } else {
      std::cout << "Couldn't open font file";
    }
    readOffsetSubtable();
    // printOffsetSubtables();

    for (int i = 0; i < numTables; i++) {
      readTableDirectories();
      printTableDirectoryEntry(i);
    }

    // std::cout << "Glyf table number: " << findTable("glyf") << std::endl;
    glyfTableNumber = findTable("glyf");
    std::cout << "glyfTableNumber: " << glyfTableNumber << std::endl;

    fontFile.seekg(tableDirectories[glyfTableNumber].offset, std::ios::beg);

    readGlyfDescription();
    printGlyfDescription(0);
  }

  void readOffsetSubtable() {
    scalerType = reader.readUnsigned32Bit(&fontFile);
    numTables = reader.readUnsigned16Bit(&fontFile);
    searchRange = reader.readUnsigned16Bit(&fontFile);
    entrySelector = reader.readUnsigned16Bit(&fontFile);
    rangeShift = reader.readUnsigned16Bit(&fontFile);
  }

  std::string convertTagUint32ToString(uint32_t tagValue) {
    char chars[4];
    chars[0] = static_cast<char>((tagValue >> 24) & 0xFF);
    chars[1] = static_cast<char>((tagValue >> 16) & 0xFF);
    chars[2] = static_cast<char>((tagValue >> 8) & 0xFF);
    chars[3] = static_cast<char>(tagValue & 0xFF);
    return std::string(chars, 4);
  }

  void readTableDirectory() {
    tableDirectoryEntries tableDirectory;
    tableDirectory.tag =
        convertTagUint32ToString(reader.readUnsigned32Bit(&fontFile));
    tableDirectory.checkSum = reader.readUnsigned32Bit(&fontFile);
    tableDirectory.offset = reader.readUnsigned32Bit(&fontFile);
    tableDirectory.length = reader.readUnsigned32Bit(&fontFile);
    tableDirectories.push_back(tableDirectory);
  }

  void readTableDirectories() {
    for (int i = 0; i < numTables; i++) {
      readTableDirectory();
    }
  }

  void printTableDirectoryEntry(int index) {
    std::cout << "Directory: " << std::dec << index << std::endl;
    std::cout << "Tag: " << std::hex << tableDirectories[index].tag
              << std::endl;
    std::cout << "Check Sum: " << std::hex << tableDirectories[index].checkSum
              << std::endl;
    std::cout << "Offset: " << std::dec << tableDirectories[index].offset
              << std::endl;
    std::cout << "length: " << std::dec << tableDirectories[index].length
              << std::endl;
    std::cout << std::endl;
  }

  void printOffsetSubtables() {
    std::cout << std::hex << "scalerType: " << scalerType << std::endl;
    std::cout << std::dec << "numTables: " << numTables << std::endl;
    std::cout << "searchRange: " << searchRange << std::endl;
    std::cout << "entrySelector: " << entrySelector << std::endl;
    std::cout << "rangeShift: " << rangeShift << std::endl;
  }

  int findTable(std::string searchedTag) {
    for (int i = 0; i < numTables; i++) {
      if (tableDirectories[i].tag == searchedTag) {
        return i;
      }
    }
    std::cout << "Couldn't find the required tag in the table. " << std::endl;
    return -1;
  }

  void readGlyfDescription() {
    glyfDesctiption bufferDescription;
    bufferDescription.numberOfContours = reader.readSigned16Bit(&fontFile);
    bufferDescription.xMin = reader.readSigned16Bit(&fontFile);
    bufferDescription.yMin = reader.readSigned16Bit(&fontFile);
    bufferDescription.xMax = reader.readSigned16Bit(&fontFile);
    bufferDescription.yMax = reader.readSigned16Bit(&fontFile);
    glyfDescriptions.push_back(bufferDescription);
  }

  void printGlyfDescription(int index) {
    std::cout << "Glyf Description: " << index << std::endl;
    std::cout << "numberContours: " << std::dec
              << glyfDescriptions[index].numberOfContours << std::endl;
    std::cout << "xMin: " << glyfDescriptions[index].xMin << std::endl;
    std::cout << "yMin: " << glyfDescriptions[index].yMin << std::endl;
    std::cout << "xMax: " << glyfDescriptions[index].xMax << std::endl;
    std::cout << "yMax: " << glyfDescriptions[index].yMax << std::endl;
  }
};

int main(void) {
  std::vector<uint8_t> x(1024);
  const std::string fontFilePath =
      "/home/fabianolbert/Documents/Projects/gameboy/fonts/"
      "JetBrainsMonoNerd.ttf";

  TTFReader ttfReader(fontFilePath);

  ////
  // for (int i = 0; i < numTables; i++) {
  //   std::cout << std::hex << std::endl;
  //   std::cout << "tag: ";
  //   reader.readPrintASCII32Bit(&fontFile);
  //   std::cout << "checkSum: " << reader.readUnsigned32Bit(&fontFile)
  //             << std::endl;
  //   std::cout << "offset: " << reader.readUnsigned32Bit(&fontFile) <<
  //   std::endl; std::cout << "length: " << std::setfill('0')
  //             << static_cast<uint32_t>(reader.readUnsigned32Bit(&fontFile))
  //             << std::endl;
  // }
  //
  // fontFile.seekg(0xb308, std::ios::beg);
  //
  // int16_t numContours = reader.readSigned16Bit(&fontFile);
  // std::cout << std::endl << "numContours: " << numContours << std::endl;
  // std::cout << std::dec << "xMin: " << reader.readFWord(&fontFile) <<
  // std::endl; std::cout << std::dec << "yMin: " << reader.readFWord(&fontFile)
  // << std::endl; std::cout << std::dec << "xMax: " <<
  // reader.readFWord(&fontFile) << std::endl; std::cout << std::dec << "yMax: "
  // << reader.readFWord(&fontFile) << std::endl;
  // // read endPtsOfContours[n] n is the number of contours
  //
  // uint16_t endPtsOfContours[numContours];
  // for (int i = 0; i < numContours; i++) {
  //   endPtsOfContours[i] = reader.readUnsigned16Bit(&fontFile);
  //   std::cout << "endPtsOfContours[" << i << "]: " << endPtsOfContours[i]
  //             << std::endl;
  // }
  //
  // uint16_t instructionLength = reader.readUnsigned16Bit(&fontFile);
  // std::cout << std::dec << "instructionLength: " << instructionLength
  //           << std::endl;
  //
  // uint8_t instructions[instructionLength];
  // for (int i = 0; i < instructionLength; i++) {
  //   instructions[i] = reader.readUnsigned8Bit(&fontFile);
  //   std::cout << std::hex << "instructions[" << i << "]" << std::setw(2)
  //             << std::setfill('0') << static_cast<int>(instructions[i])
  //             << std::endl;
  // }
  // uint8_t flag[instructionLength];
  // for (int i = 0; i < instructionLength; i++) {
  //   flag[i] = reader.readUnsigned8Bit(&fontFile);
  //   // std::bitset<8> x(flag[i]);
  //   std::cout << std::hex << static_cast<uint16_t>(flag[i]) << std::endl;
  // }
  // std::cout << "x_coordinate:"
  //           << static_cast<uint16_t>(reader.readUnsigned8Bit(&fontFile))
  //           << std::endl;
  //
  // bool onCurve = ((flag[0] & 0b10000000) >> 7);
  // bool xShortVector = ((flag[0] & 0b01000000) >> 6);
  // bool yShortVector = ((flag[0] & 0b00100000) >> 5);
  // bool Repeat = ((flag[0] & 0b00010000) >> 4);
  // bool xSame = ((flag[0] & 0b00001000) >> 3);
  // bool ySame = ((flag[0] & 0b00000100) >> 2);
  // std::cout << "On Curve: " << onCurve << std::endl;
  // std::cout << "x-ShortVector: " << xShortVector << std::endl;
  // std::cout << "y-ShortVector: " << yShortVector << std::endl;
  // std::cout << "Repeat: " << Repeat << std::endl;
  // std::cout << "xSame: " << xSame << std::endl;
  // std::cout << "ySame: " << ySame << std::endl;

  // std::cout << std::setw(2) << static_cast<int>(msb) << " ";
  // std::cout << std::setw(2) << static_cast<int>(lsb) << " ";
  //
  // std::cout << (isprint(static_cast<char>(msb)) ? static_cast<char>(msb)
  //                                               : '.');
  // std::cout << (isprint(static_cast<char>(lsb)) ? static_cast<char>(lsb)
  //                                               : '.');
  // std::cout << std::endl;
  // for (int j = 0; j < 32; j++) {
  //   for (int i = 0; i < 4; i++) {
  //     char c = static_cast<int>(x[i + 4 * j]);
  //     std::cout << std::hex << (isprint(c) ? c : '.') << " ";
  //   }
  //   std::cout << std::endl;
  // }
  // }
  // else {
  //   std::cout << "couldn't open file" << std::endl;
  // }

  // opengl stuff
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "GameboyEmulator", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
