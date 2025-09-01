
#include "reader.h"

uint8_t Reader::readUnsigned8Bit(std::ifstream *fontFile) {
  char readBuffer;
  uint16_t returnValue;
  fontFile->read(&readBuffer, sizeof(uint8_t));
  return static_cast<uint8_t>(readBuffer);
}

uint16_t Reader::readUnsigned16Bit(std::ifstream *fontFile) {
  char readBuffer[2];
  uint16_t returnValue;
  fontFile->read(&readBuffer[0], sizeof(uint16_t));
  return ((static_cast<uint16_t>(static_cast<uint8_t>(readBuffer[0])) << 8) |
          static_cast<uint16_t>(static_cast<uint8_t>(readBuffer[1])));
}

int16_t Reader::readSigned16Bit(std::ifstream *fontFile) {
  char readBuffer[2];
  uint16_t returnValue;
  fontFile->read(&readBuffer[0], sizeof(uint16_t));
  return ((static_cast<int16_t>(static_cast<uint8_t>(readBuffer[0])) << 8) |
          static_cast<int16_t>(static_cast<uint8_t>(readBuffer[1])));
}

int16_t Reader::readFWord(std::ifstream *fontFile) {
  return readSigned16Bit(fontFile);
}

uint32_t Reader::readUnsigned32Bit(std::ifstream *fontFile) {
  char readBuffer[4];
  uint16_t returnValue;
  fontFile->read(&readBuffer[0], sizeof(uint32_t));
  return ((static_cast<uint32_t>(static_cast<uint8_t>(readBuffer[0])) << 24) |
          (static_cast<uint32_t>(static_cast<uint8_t>(readBuffer[1])) << 16) |
          (static_cast<uint32_t>(static_cast<uint8_t>(readBuffer[2])) << 8) |
          (static_cast<uint32_t>(static_cast<uint8_t>(readBuffer[3]))));
}

uint16_t Reader::readPrintUnsigned16Bit(std::ifstream *fontFile) {
  uint16_t returnValue = readUnsigned16Bit(fontFile);
  std::cout << returnValue << std::endl;
  return returnValue;
}

uint32_t Reader::readPrintASCII32Bit(std::ifstream *fontFile) {
  uint32_t returnValue = readUnsigned32Bit(fontFile);
  std::cout << std::hex << std::setw(8) << std::setfill('0')
            << (static_cast<char>((returnValue >> 24) & 0xFF))
            << (static_cast<char>((returnValue >> 16) & 0xFF))
            << (static_cast<char>((returnValue >> 8) & 0xFF))
            << (static_cast<char>((returnValue) & 0xFF)) << std::endl;
  return returnValue;
}
