#include <cmath>
#define GLFW_INCLUDE_NONE
#include "font/TTFReader.h"
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

class Glyph {
private:
  uint8_t raw_flags;

public:
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

  std::vector<std::vector<vertex>> verticesBuffer;
  Glyf_t glyfBuffer;
  ttfReader.readGlyf(glyfBuffer, 22);
  ttfReader.printGlyph(glyfBuffer);

  int start = 0;
  for (int i = 0; i < glyfBuffer.simpleGlyf.endPtsOfContours.size(); i++) {
    int endPoint = glyfBuffer.simpleGlyf.endPtsOfContours[i];
    std::vector<vertex> vertexVector;
    for (int j = start; j <= endPoint; j++) {
      vertex bufferVertex;
      bufferVertex.x = glyfBuffer.simpleGlyf.xCoordinates[j];
      bufferVertex.y = glyfBuffer.simpleGlyf.yCoordinates[j];
      vertexVector.push_back(bufferVertex);
    }
    verticesBuffer.push_back(vertexVector);
    start = endPoint + 1;
  }

  double normalizationFactor = 1000;
  std::cout << "verticesBuffer x\n";
  for (int i = 0; i < static_cast<int>(verticesBuffer.size()); ++i) {
    for (int j = 0; j < static_cast<int>(verticesBuffer[i].size()); ++j) {
      verticesBuffer[i][j].x /= normalizationFactor;
      verticesBuffer[i][j].y /= normalizationFactor;
    }
    std::cout << "\n";
  }

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

  std::vector<unsigned int> VAOVector(verticesBuffer.size());
  std::vector<unsigned int> VBOVector(verticesBuffer.size());

  for (int i = 0; i < verticesBuffer.size(); i++) {
    glGenVertexArrays(1, &VAOVector[i]);
    glBindVertexArray(VAOVector[i]);
    glGenBuffers(1, &VBOVector[i]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOVector[i]);
    glBufferData(GL_ARRAY_BUFFER, verticesBuffer[i].size() * sizeof(vertex),
                 verticesBuffer[i].data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
    glEnableVertexAttribArray(0); //
  }

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
    for (int i = 0; i < verticesBuffer.size(); i++) {
      glBindVertexArray(VAOVector[i]);
      // glDrawArrays(GL_POINTS, 0, verticesBuffer[0].size());
      glDrawArrays(GL_LINE_LOOP, 0, verticesBuffer[i].size());
    }

    glBindVertexArray(0);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
