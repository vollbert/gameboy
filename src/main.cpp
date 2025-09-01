#include <cmath>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdint>
#include <fstream>
#include <glad/glad.h>
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

    std::cout << std::endl;
    std::vector<uint16_t> endPtsOfContours;
    for (int i = 0; i < glyfDescriptions[0].numberOfContours; i++) {
      endPtsOfContours.push_back(reader.readUnsigned16Bit(&fontFile));
      std::cout << "endPtsOfContours: " << i << ": " << endPtsOfContours[i]
                << std::endl;
    }

    uint16_t instructionLength = reader.readUnsigned16Bit(&fontFile);
    std::cout << "instructionLength: " << instructionLength << std::endl;

    std::vector<uint8_t> instructions;
    for (int i = 0; i < instructionLength; i++) {
      instructions.push_back(reader.readUnsigned8Bit(&fontFile));
      std::cout << "instructions: " << i << ": " << std::hex
                << std::setfill('0') << std::setw(2)
                << static_cast<uint16_t>(instructions[i]) << std::endl;
    }

    int numberOfPoints = endPtsOfContours[endPtsOfContours.size() - 1] + 1;
    std::cout << std::endl
              << "endPtsOfContours: " << numberOfPoints << std::endl;
    std::vector<outlineFlags_t> OutlineFlagsVector;

    int i = 0;
    while (OutlineFlagsVector.size() < numberOfPoints) {
      uint8_t raw_bytes = reader.readUnsigned8Bit(&fontFile);
      std::cout << "test: " << static_cast<int>(raw_bytes) << std::endl;
      outlineFlags_t currentOutlineFlag(raw_bytes);
      OutlineFlagsVector.push_back(currentOutlineFlag);
      printOutlineFlags(currentOutlineFlag);
      if (currentOutlineFlag.repeat) {
        uint8_t repeatTimes = reader.readUnsigned8Bit(&fontFile);
        for (int j = 0; j < repeatTimes; j++) {
          OutlineFlagsVector.push_back(currentOutlineFlag);
          printOutlineFlags(currentOutlineFlag);
        }
      }
      i++;
    }

    // read x points
    std::vector<int16_t> xCoordinates;
    uint16_t previousPoint;
    // for (int i = 0; i < numberOfPoints; i++) {
    //   newContour.push_back(0);
    // }
    // for (int i = 0; i < endPtsOfContours.size(); i++) {
    //   newContour[endPtsOfContours[i]] = endPtsOfContours[i];
    // }
    for (int i = 0; i < numberOfPoints; i++) {
      if (OutlineFlagsVector[i].xShortVector) {
        //  x-Short Vector bit is set, this bit describes the sign of the value,
        //  with a value of 1 equalling positive and a zero value negative.
        // convert uint8 to uint16 so that we only need one vector
        uint16_t xCoordinate =
            static_cast<int16_t>(reader.readUnsigned8Bit(&fontFile));
        std::cout << "xCoordinate: " << std::dec << xCoordinate << std::endl;
        if (i == 0) {
          previousPoint = 0;
        } else {

          previousPoint = xCoordinates[i - 1];
        }
        xCoordinate =
            previousPoint +
            (OutlineFlagsVector[i].xSame ? xCoordinate : -xCoordinate);
        xCoordinates.push_back(xCoordinate);
      } else {
        if (OutlineFlagsVector[i].xSame) {
          // If the x-short Vector bit is not set, and this bit is set, then the
          // current x-coordinate is the same as the previous x-coordinate.
          //
          // std::cout << "xCoordinates[i-1]: " << xCoordinates[i - 1]
          //           << std::endl;
          xCoordinates.push_back(xCoordinates[i - 1]);

        } else {
          // If the x-short Vector bit is not set, and this bit is not set, the
          // current x-coordinate is a signed 16-bit delta vector. In this case,
          // the delta vector is the change in x
          if (i == 0) {
            xCoordinates.push_back(reader.readSigned16Bit(&fontFile));

          } else {
            xCoordinates.push_back(xCoordinates[i - 1] +
                                   reader.readSigned16Bit(&fontFile));
            // std::cout << "xCoordinates[i-1]: " << xCoordinates[i - 1]
            //           << std::endl;
          }
        }
      }
    }

    // read y points
    std::vector<int16_t> yCoordinates;
    for (int i = 0; i < numberOfPoints; i++) {
      if (OutlineFlagsVector[i].yShortVector) {
        //  y-Short Vector bit is set, this bit describes the sign of the
        //  value, with a value of 1 equalling positive and a zero value
        //  negative.
        // convert uint8 to uint16 so that we only need one vector
        uint16_t yCoordinate =
            static_cast<int16_t>(reader.readUnsigned8Bit(&fontFile));
        // std::cout << "yCoordinate: " << yCoordinate << std::endl;
        yCoordinates.push_back(OutlineFlagsVector[i].ySame ? yCoordinate
                                                           : -yCoordinate);
      } else {
        if (OutlineFlagsVector[i].ySame) {
          // If the y-short Vector bit is not set, and this bit is set, then
          // the current y-coordinate is the same as the previous
          // y-coordinate.
          //
          if (i == 0) {

            yCoordinates.push_back(0);
          } else {
            // std::cout << "yCoordinates[i-1]: " << yCoordinates[i - 1]
            // << std::endl;
            yCoordinates.push_back(yCoordinates[i - 1]);
          }

        } else {
          // If the y-short Vector bit is not set, and this bit is not set,
          // the current y-coordinate is a signed 16-bit delta vector. In this
          // case, the delta vector is the change in y
          if (i == 0) {
            yCoordinates.push_back(reader.readSigned16Bit(&fontFile));
          } else {
            // std::cout << "yCoordinates[i-1]: " << yCoordinates[i - 1]
            // << std::endl;
            yCoordinates.push_back(yCoordinates[i - 1] +
                                   reader.readSigned16Bit(&fontFile));
          }
          std::cout << "yCoordinate: " << yCoordinates[i] << std::endl;
        }
      }
    }

    for (int i = 0; i < numberOfPoints; i++) {
      std::cout << std::dec << "Coordinates " << i << ": {" << xCoordinates[i]
                << ", " << yCoordinates[i] << "}" << std::endl;
    }
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
    tableDirectoryEntries_t tableDirectory;
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
    glyfDescription_t bufferDescription;
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

  void printOutlineFlags(outlineFlags_t outlineFlags) {
    std::cout << "onCurve: " << outlineFlags.onCurve << std::endl;
    std::cout << "xShortVector: " << outlineFlags.xShortVector << std::endl;
    std::cout << "yShortVector: " << outlineFlags.yShortVector << std::endl;
    std::cout << "repeat: " << outlineFlags.repeat << std::endl;
    std::cout << "xSame: " << outlineFlags.xSame << std::endl;
    std::cout << "ySame: " << outlineFlags.ySame << "\n\n";
    return;
  }
};

// float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f,
// 0.0f};

struct vertex {
  float x, y, z;
};

class openGLRendererClass {
private:
public:
  std::vector<vertex> generateCircleVertices(int nVertices, float radius,
                                             float offsetX, float offsetY) {
    std::vector<vertex> vertices;

    vertex bufferVertex;
    float steps = 3.141f * 2.0f / static_cast<float>(nVertices);
    for (int i = 0; i < nVertices; i++) {
      bufferVertex.x = (radius * std::cos(steps * i) + offsetX);
      bufferVertex.y = radius * std::sin(steps * i) + offsetY;
      bufferVertex.z = 0.0f;
      vertices.push_back(bufferVertex);
    }
    return vertices;
  }
};

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\0";

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
  //             <<
  //             static_cast<uint32_t>(reader.readUnsigned32Bit(&fontFile))
  //             << std::endl;
  // }
  //
  // fontFile.seekg(0xb308, std::ios::beg);
  //
  // int16_t numContours = reader.readSigned16Bit(&fontFile);
  // std::cout << std::endl << "numContours: " << numContours <<
  // std::endl; std::cout << std::dec << "xMin: " <<
  // reader.readFWord(&fontFile) << std::endl; std::cout << std::dec <<
  // "yMin: " << reader.readFWord(&fontFile)
  // << std::endl; std::cout << std::dec << "xMax: " <<
  // reader.readFWord(&fontFile) << std::endl; std::cout << std::dec <<
  // "yMax:
  // "
  // << reader.readFWord(&fontFile) << std::endl;
  // // read endPtsOfContours[n] n is the number of contours
  //
  // uint16_t endPtsOfContours[numContours];
  // for (int i = 0; i < numContours; i++) {
  //   endPtsOfContours[i] = reader.readUnsigned16Bit(&fontFile);
  //   std::cout << "endPtsOfContours[" << i << "]: " <<
  //   endPtsOfContours[i]
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
  //   std::cout << std::hex << "instructions[" << i << "]" <<
  //   std::setw(2)
  //             << std::setfill('0') <<
  //             static_cast<int>(instructions[i])
  //             << std::endl;
  // }
  // uint8_t flag[instructionLength];
  // for (int i = 0; i < instructionLength; i++) {
  //   flag[i] = reader.readUnsigned8Bit(&fontFile);
  //   // std::bitset<8> x(flag[i]);
  //   std::cout << std::hex << static_cast<uint16_t>(flag[i]) <<
  //   std::endl;
  // }
  // std::cout << "x_coordinate:"
  //           <<
  //           static_cast<uint16_t>(reader.readUnsigned8Bit(&fontFile))
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
  // std::cout << (isprint(static_cast<char>(msb)) ?
  // static_cast<char>(msb)
  //                                               : '.');
  // std::cout << (isprint(static_cast<char>(lsb)) ?
  // static_cast<char>(lsb)
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
  int windowWidth = 640;
  int windowHeight = 480;
  window = glfwCreateWindow(windowWidth, windowHeight, "GameboyEmulator", NULL,
                            NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }

  // unsigned int VBO;
  // glGenBuffers(1, &VBO);
  // glBindBuffer(GL_ARRAY_BUFFER, VBO);

  openGLRendererClass openGLRenderer;
  std::vector<vertex> vertices;
  // openGLRenderer.generateCircleVertices(20, 0.5f, 0.0f, 0.0f);

  // vertices.push_back({-0.5f, -0.5f, 0.0f});
  // vertices.push_back({0.5f, -0.5f, 0.0f});
  // vertices.push_back({0.0f, 0.5f, 0.0f});
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(vertex),
  //              vertices.data(), GL_DYNAMIC_DRAW);

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void
  // *)0);
  // glEnableVertexAttribArray(0);

  std::vector<vertex> vertices1;
  std::vector<vertex> vertices2;
  std::vector<vertex> vertices3;

  // Coordinates 0: {90, 0}
  // Coordinates 1: {90, 730}
  // Coordinates 2: {510, 730}
  // Coordinates 3: {510, 0}
  // Coordinates 4: {140, 109}
  // Coordinates 5: {440, 680}
  // Coordinates 6: {140, 680}
  // Coordinates 7: {20, 50}
  // Coordinates 8: {320, 50}
  // Coordinates 9: {320, 621}
  vertices1.push_back({90.0 / 1000.0f, 0, 0});
  vertices1.push_back({90.0 / 1000.0f, 730 / 1000.0f, 0});
  vertices1.push_back({510.0 / 1000.0f, 730 / 1000.0f, 0});
  vertices1.push_back({510.0 / 1000.0f, 0 / 1000.0f, 0});

  vertices2.push_back({140.0 / 1000.0f, 109 / 1000.0f, 0});
  vertices2.push_back({440.0 / 1000.0f, 680 / 1000.0f, 0});
  vertices2.push_back({140.0 / 1000.0f, 680 / 1000.0f, 0});

  vertices3.push_back({160.0 / 1000.0f, 50 / 1000.0f, 0});
  vertices3.push_back({460.0 / 1000.0f, 50 / 1000.0f, 0});
  vertices3.push_back({460.0 / 1000.0f, 621 / 1000.0f, 0});

  unsigned int VAO1, VBO1;
  unsigned int VAO2, VBO2;
  unsigned int VAO3, VBO3;
  glGenVertexArrays(1, &VAO1);
  glBindVertexArray(VAO1);
  glBindBuffer(GL_ARRAY_BUFFER, VBO1);
  glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(vertex),
               vertices1.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0); //

  glGenVertexArrays(1, &VAO2);
  glBindVertexArray(VAO2);
  glGenBuffers(1, &VBO2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(vertex),
               vertices2.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  //
  glEnableVertexAttribArray(0); //

  glGenVertexArrays(1, &VAO3);
  glBindVertexArray(VAO3);
  glGenBuffers(1, &VBO3);
  glBindBuffer(GL_ARRAY_BUFFER, VBO3);
  glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(vertex),
               vertices3.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0); //

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    // --- CLEAR SCREEN ---
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Clear to a teal-ish color
    glClear(GL_COLOR_BUFFER_BIT);
    // --- END CLEAR SCREEN ---
    glUseProgram(shaderProgram);
    glPointSize(5.0f);
    // glLineWidth(2.0f);
    // --- DRAW OBJECT 1 ---
    glBindVertexArray(VAO1);
    glDrawArrays(GL_POINTS, 0, vertices1.size());
    glDrawArrays(GL_LINE_LOOP, 0, vertices1.size());

    // --- Draw Object 2 (Triangle 1) ---
    glBindVertexArray(VAO2); // Correct VAO
    glDrawArrays(GL_POINTS, 0, vertices2.size());
    glDrawArrays(GL_LINE_LOOP, 0,
                 vertices2.size()); // Draw the triangle outline

    // --- Draw Object 3 (Triangle 2) ---
    glBindVertexArray(VAO3);
    glDrawArrays(GL_POINTS, 0, vertices3.size());
    glDrawArrays(GL_LINE_LOOP, 0,
                 vertices3.size()); // Draw the triangle outline

    // --- DRAWING COMMANDS ---
    // --- END DRAWING COMMANDS ---
    glBindVertexArray(0);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
