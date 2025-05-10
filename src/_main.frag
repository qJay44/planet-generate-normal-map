// For GLSLS Viewer tests

#version 460 core

out vec4 FragColor;

layout(binding = 0) uniform sampler2D heightmap;

const float seaLevel = -10.f;
const float heightMutliplier = 40.f;
const float worldRadius = 40.f;

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
  const ivec2 texCoord = ivec2(gl_FragCoord.xy * (vec2(2560, 1280) / vec2(1600, 900)));
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

