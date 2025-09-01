#pragma once
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <istream>

class Reader {

public:
  uint8_t readUnsigned8Bit(std::ifstream *fontFile);

  uint16_t readUnsigned16Bit(std::ifstream *fontFile);

  int16_t readSigned16Bit(std::ifstream *fontFile);

  int16_t readFWord(std::ifstream *fontFile);

  uint32_t readUnsigned32Bit(std::ifstream *fontFile);

  uint16_t readPrintUnsigned16Bit(std::ifstream *fontFile);

  uint32_t readPrintASCII32Bit(std::ifstream *fontFile);
};
