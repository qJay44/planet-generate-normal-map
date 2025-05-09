#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTex;

out vec2 texCoordUV;

layout(location = 0) uniform mat4 translateMat;
layout(location = 1) uniform mat4 scaleMat;

void main() {
  vec3 pos = vec3(translateMat * scaleMat * vec4(inPos, 1.f));
  texCoordUV = inTex;

  gl_Position = vec4(pos, 1.f);
}

