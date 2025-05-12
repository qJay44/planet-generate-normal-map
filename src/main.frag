#version 460 core

in vec2 texCoordUV;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D heightmap;

layout(location = 2) uniform float seaLevel;
layout(location = 3) uniform float heightMutliplier;
layout(location = 4) uniform float worldRadius;

#define PI 3.141592265359f

ivec2 mapSize = textureSize(heightmap, 0);

vec3 calculateWorldPoint(ivec2 texCoord) {
  texCoord = (texCoord + mapSize) % mapSize;

  vec2 uv = vec2(texCoord) / (vec2(mapSize) - 1.f);
  vec2 coord = (uv - 0.5f) * PI * vec2(2.f, 1.f);

  float r = cos(coord.y);
  vec3 spherePoint = vec3(sin(coord.x) * r, sin(coord.y), -cos(coord.x) * r);

  float height = texture(heightmap, uv).r;
  float worldHeight = worldRadius + (height - seaLevel) * heightMutliplier;

  return spherePoint * worldHeight;
}

void main() {
  const ivec2 texCoord = ivec2(texCoordUV * vec2(mapSize));
  vec3 posNorth = calculateWorldPoint(texCoord + ivec2( 0,  1));
  vec3 posSouth = calculateWorldPoint(texCoord + ivec2( 0, -1));
  vec3 posEast  = calculateWorldPoint(texCoord + ivec2( 1,  0));
  vec3 posWest  = calculateWorldPoint(texCoord + ivec2(-1,  0));

  vec3 dirNorth = normalize(posNorth - posSouth);
  vec3 dirEast = normalize(posEast - posWest);
  vec3 normal = normalize(cross(dirNorth, dirEast));

  normal = (normal + 1.f) * 0.5f;

  float height = texture(heightmap, vec2(texCoord) / vec2(mapSize)).r;

  FragColor = vec4(normal, height);
}

