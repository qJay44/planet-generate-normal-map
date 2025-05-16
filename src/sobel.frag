#version 460 core

// https://stackoverflow.com/a/26357357/17694832

in vec2 texCoordUV;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D heightmap;
layout(location = 2) uniform float scale;

ivec2 mapSize = textureSize(heightmap, 0);

float getHeightmapValue(vec2 globalUV) {
  vec2 uv = globalUV / (vec2(mapSize) - 1.f);
  return texture(heightmap, uv).r;
}

float getNeighbourHeight(ivec2 texCoord) {
  return getHeightmapValue((texCoord + mapSize) % mapSize);
}

void main() {
  // [6][7][8]
  // [3][4][5]
  // [0][1][2]

  ivec2 texCoord = ivec2(texCoordUV * vec2(mapSize));
  float tc6 = getNeighbourHeight(texCoord + ivec2(-1,  1));
  float tc7 = getNeighbourHeight(texCoord + ivec2( 0,  1));
  float tc8 = getNeighbourHeight(texCoord + ivec2( 1,  1));
  float tc3 = getNeighbourHeight(texCoord + ivec2(-1,  0));
  float tc4 = getNeighbourHeight(texCoord + ivec2( 0,  0));
  float tc5 = getNeighbourHeight(texCoord + ivec2( 1,  0));
  float tc0 = getNeighbourHeight(texCoord + ivec2(-1, -1));
  float tc1 = getNeighbourHeight(texCoord + ivec2( 0, -1));
  float tc2 = getNeighbourHeight(texCoord + ivec2( 1,  1));

  vec3 normal;
  normal.x = scale * -(tc2 - tc0 + 2.f * (tc5 - tc3) + tc8 - tc6);
  normal.y = scale * -(tc6 - tc0 + 2.f * (tc7 - tc1) + tc8 - tc2);
  normal.z = 1.f;
  normal = (normalize(normal) + 1.f) * 0.5f;

  FragColor = vec4(normal, tc4);
}

