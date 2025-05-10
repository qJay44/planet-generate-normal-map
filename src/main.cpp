#include <algorithm>
#include <format>
#include <direct.h>

#include "GLFW/glfw3.h"
#include "Shader.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pch.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

constexpr vec2 scaleLimit{0.5f, 5.f};
constexpr float scaleAmount = 0.1f;
constexpr float panMoveScale = 0.3f;

static float dt;
static mat4 scaleMat = mat4(1.f);
static mat4 translateMat = mat4(1.f);
static bool imguiHovered = false;

bool isImguiHovered(const vec2& mouse) {
  auto& io = ImGui::GetIO();
  return io.WantCaptureMouse || io.WantCaptureKeyboard;
}

void resizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  static float prevScale = 1.f;
  float scale = prevScale + yoffset * scaleAmount;
  scale = std::clamp(scale, scaleLimit.x, scaleLimit.y);
  float scaleFactor = scale / prevScale;
  scaleMat = glm::scale(scaleMat, vec3(scaleFactor));
  prevScale = scale;
}

void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos) {
  static bool isHoldingButton = false;
  static vec3 prevPos = {0.f, 0.f, 0.f};

  vec3 currPos = {xpos, ypos, 0.f};

  if (!imguiHovered) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      vec3 toMove = (currPos - prevPos * glm::sign(currPos)) * dt * panMoveScale;
      toMove.y *= -1.f;
      translateMat = glm::translate(translateMat, toMove);
    }
  }

  prevPos = currPos;
}

int main() {
  // Assuming the executable is launching from its own directory
  _chdir("../../../src");

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Window init
  ivec2 winSize = {1600, 900};
  ivec2 winCenter = winSize / 2;
  GLFWwindow* window = glfwCreateWindow(winSize.x, winSize.y, "Sphere", NULL, NULL);

  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  // glfwSetCursorPos(window, winCenter.x, winSize.y * 0.5f);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetCursorPosCallback(window, mouseCursorCallback);

  // GLAD init
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, winSize.x, winSize.y);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  float seaLevel = 20.f;
  float heightMutliplier = 12.f;
  float worldRadius = 50.f;

  Shader mainShader("main.vert", "main.frag");
  mainShader.setUniformTexture(0, 0);

  int w, h, colorChannels;
  stbi_set_flip_vertically_on_load(true);
  byte* pixels = stbi_load("heightmap2560.png", &w, &h, &colorChannels, 0);

  u32 heightmapTexture;
  glGenTextures(1, &heightmapTexture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, heightmapTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
  stbi_image_free(pixels);

  float vertices[30] {
    -1.f, -1.f,  0.f,   0.f, 0.f,
    -1.f,  1.f,  0.f,   0.f, 1.f,
     1.f,  1.f,  0.f,   1.f, 1.f,
     1.f,  1.f,  0.f,   1.f, 1.f,
     1.f, -1.f,  0.f,   1.f, 0.f,
    -1.f, -1.f,  0.f,   0.f, 0.f
  };

  u32 vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  u32 vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static double titleTimer = glfwGetTime();
    static double prevTime = titleTimer;
    static double currTime = prevTime;

    constexpr double fpsLimit = 1. / 90.;
    currTime = glfwGetTime();
    dt = currTime - prevTime;

    // FPS cap
    if (dt < fpsLimit) continue;
    else prevTime = currTime;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    imguiHovered = isImguiHovered({mx, my});

    // Update window title every 0.3 seconds
    if (currTime - titleTimer >= 0.3) {
      u16 fps = static_cast<u16>(1.f / dt);
      glfwSetWindowTitle(window, std::format("FPS: {} / {:.5f} ms", fps, dt).c_str());
      titleTimer = currTime;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_TRUE);

    mainShader.setUniformMatrix4f(0, translateMat);
    mainShader.setUniformMatrix4f(1, scaleMat);
    mainShader.setUniform1f(2, seaLevel);
    mainShader.setUniform1f(3, heightMutliplier);
    mainShader.setUniform1f(4, worldRadius);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    ImGui::Begin("Settings");

    ImGui::SliderFloat("seaLevel", &seaLevel, -100.f, 100.f);
    ImGui::SliderFloat("heightMutliplier", &heightMutliplier, 0.f, 100.f);
    ImGui::SliderFloat("worldRadius", &worldRadius, 1.f, 100.f);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}

